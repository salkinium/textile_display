//
//  TDCommunicator.m
//  Textplay
//
//  Created by Niklas Hauser on 07.10.12.
//  Copyright (c) 2012 Niklas Hauser. All rights reserved.
//

#import "TDCommunicator.h"
#import "GCDAsyncSocket.h"

const uint8_t startDelimiterByte = 0x7e;
const uint8_t endDelimiterByte = 0x7c;
const uint8_t controlEscapeByte = 0x7d;
const uint16_t crcInitialValue = 0xffff;

typedef enum
{
	kTDAddressIndividualGroup = 0x8000,
	kTDAddressBroadcast = 0xffff,
	kTDAddressValue = 0x7fff,
} TDAddress;

@interface TDCommunicator ()
// tricking the KVO to give us a setter for this, hihi ;)
@property (nonatomic, readwrite) NSString *connectionStatus;
@property (nonatomic, readwrite) NSString *connectionError;

- (BOOL)validateUrl:(NSString *)url;
- (void)attemptToConnect;

-(NSData *)messageTo:(uint16_t)destination
			  ofType:(TDMessageType)type
		 withCommand:(uint8_t)command
		 withPayload:(NSData *)payload;
-(uint16_t)updateCrc:(uint16_t)crc withData:(uint8_t)data;
-(uint8_t *)escapeByte:(uint8_t)data intoBuffer:(uint8_t *)buffer;
@end

@implementation TDCommunicator

@synthesize connectionURL = _connectionURL;
@synthesize connectionStatus = _connectionStatus;
@synthesize address = _address;
@synthesize groupAddress = _groupAddress;

-(id)init
{
	if (self = [super init])
	{
		_address = 0;
		_groupAddress = 0x7fff;
		_connectionURL = @"";
		self.connectionError = @"no URL";
		self.connectionStatus = @"disconnected";
		
		dispatch_queue_t mainQueue = dispatch_get_main_queue();
		_asyncSocket = [[GCDAsyncSocket alloc] initWithDelegate:self delegateQueue:mainQueue];
		
		[self attemptToConnect];
	}
	return self;
}

+ (id)sharedInstance
{
	DEFINE_SHARED_INSTANCE_USING_BLOCK(^{
		return [[self alloc] init];
	});
}

#pragma mark - connection

- (void)attemptToConnect
{
	if (![_asyncSocket isConnected])
	{
		[self setConnectionError:_connectionURL];
	}
	
	[NSTimer scheduledTimerWithTimeInterval:1.0
									 target:self
								   selector:@selector(attemptToConnect)
								   userInfo:nil
									repeats:NO];
}

-(void)setConnectionURL:(NSString *)connectionURL
{
	[self willChangeValueForKey:@"connectionURL"];
	_connectionURL = connectionURL;
	[self didChangeValueForKey:@"connectionURL"];
	
	if ( (_validURL = [self validateUrl:_connectionURL]) )
	{
		// default port
		UInt16 port = 2000;
		// split ip and port, check if port is given
		NSArray *components = [connectionURL componentsSeparatedByString:@":"];
		if (components.count == 2)
		{
			NSNumberFormatter *formatter = [[NSNumberFormatter alloc] init];
			[formatter setNumberStyle:NSNumberFormatterNoStyle];
			port = [[formatter numberFromString:components[1]] intValue];
		}
		
		if ([_asyncSocket.connectedHost isEqualToString:components[0]] && _asyncSocket.connectedPort == port)
		{
			NSLog(@"already connected to %@:%d", components[0], port);
		}
		else
		{
			self.connectionStatus = @"disconnecting...";
			[_asyncSocket disconnect];
			
			self.connectionStatus = @"connecting...";
//			NSLog(@"connecting to host %@ on port %d", components[0], port);
			
			NSError *error = nil;
			if (![_asyncSocket connectToHost:components[0] onPort:port withTimeout:1.5 error:&error])
			{
				self.connectionStatus = @"connection error";
				NSLog(@"connection error: %@", error);
			}
		}
	}
	else
	{
		self.connectionError = @"invalid IP format";
	}
}

-(void)sendFrameWithData:(NSData *)data
{
	// send out all the data
	for (uint_fast8_t groupId=0; groupId < 4; ++groupId)
	{
		NSData *subData = [data subdataWithRange:NSMakeRange(groupId*48, 48)];
		NSData *message = [self messageTo:(0x1200 | groupId)
								   ofType:kTDMessageTypeMulticast
							  withCommand:'L'
							  withPayload:subData];
		if (_asyncSocket.isConnected)
		{
			[_asyncSocket writeData:message withTimeout:-1 tag:'L'+groupId];
		}
	}
	NSData *message = [self messageTo:0
							   ofType:kTDMessageTypeBroadcast
						  withCommand:'S'
						  withPayload:nil];
	if (_asyncSocket.isConnected)
	{
		[_asyncSocket writeData:message withTimeout:-1 tag:'S'];
	}
}

#pragma mark - Socket Delegate

- (void)socket:(GCDAsyncSocket *)sock didConnectToHost:(NSString *)host port:(UInt16)port
{
	self.connectionStatus = @"connected";
}

- (void)socket:(GCDAsyncSocket *)sock didWriteDataWithTag:(long)tag
{
//	NSLog(@"socket:%p didWriteDataWithTag:%ld", sock, tag);
}

- (void)socket:(GCDAsyncSocket *)sock didReadData:(NSData *)data withTag:(long)tag
{
//	NSLog(@"reading: %@", data);
}

- (void)socketDidDisconnect:(GCDAsyncSocket *)sock withError:(NSError *)err
{
	self.connectionStatus = @"disconnected";
	if (err.code == 3)
		self.connectionError = @"attempt timed out";
	else if (err.code == 61)
		self.connectionError = @"connection refused";
	else NSLog(@"disconnection error: %@", err);
}

#pragma mark - private

-(BOOL)validateUrl:(NSString *)url
{
	NSString *theURL = @"(\\d+.\\d+.\\d+.\\d+)(:[1-9][0-9]*)?";
	NSPredicate *urlTest = [NSPredicate predicateWithFormat:@"SELF MATCHES %@", theURL];
	return [urlTest evaluateWithObject:url];
}

-(NSData *)messageTo:(uint16_t)destination
			  ofType:(TDMessageType)type
		 withCommand:(uint8_t)command
		 withPayload:(NSData *)payload
{
	// doubling allocation size, since the escape sequence could
	// theoretically double (almost) every byte
	// max. 114 bytes for multicast messages
	uint8_t *buffer = malloc((1 + 2 + 2 + 1 + payload.length + 2 + 1)*2);
	uint8_t *firstElement = buffer;
	uint16_t crc = crcInitialValue;
	
	// Start Delimiter
	*buffer++ = startDelimiterByte;
	
	// HEADER
	// Destination Address
	if (type == kTDMessageTypeBroadcast)
	{
		destination = kTDAddressBroadcast;
	}
	else {
		destination &= kTDAddressValue;
		if (type == kTDMessageTypeMulticast)
			destination |= kTDAddressIndividualGroup;
	}
	
	buffer = [self escapeByte:destination intoBuffer:buffer];
	crc = [self updateCrc:crc withData:destination];
	buffer = [self escapeByte:(destination >> 8) intoBuffer:buffer];
	crc = [self updateCrc:crc withData:(destination >> 8)];
	
	// Source Address
	// LSB first
	buffer = [self escapeByte:_address intoBuffer:buffer];
	crc = [self updateCrc:crc withData:_address];
	buffer = [self escapeByte:(_address >> 8) intoBuffer:buffer];
	crc = [self updateCrc:crc withData:(_address >> 8)];
	
	// PAYLOAD
	// Command
	buffer = [self escapeByte:command intoBuffer:buffer];
	crc = [self updateCrc:crc withData:command];
	
	// Optional Payload
	if (payload)
	{
		int i = 0;
		const uint8_t *data = payload.bytes;
		
		while (i < payload.length)
		{
			buffer = [self escapeByte:data[i] intoBuffer:buffer];
			crc = [self updateCrc:crc withData:data[i]];
			i++;
		}
	}
	
	// FCS
	buffer = [self escapeByte:crc intoBuffer:buffer];
	buffer = [self escapeByte:(crc >> 8) intoBuffer:buffer];
	
	// End Delimiter
	*buffer++ = endDelimiterByte;
	
	return [NSData dataWithBytesNoCopy:firstElement length:(buffer - firstElement)];
}

-(uint16_t)updateCrc:(uint16_t)crc
			withData:(uint8_t)data
{
	// one could use a Framework function for this, but I am not sure which one
	// performance is not an issue, so what the heck
	
	crc ^= data;
	for (uint_fast8_t i = 0; i < 8; ++i)
	{
		if (crc & 1)
		{
			crc = (crc >> 1) ^ 0xA001;
		}
		else
		{
			crc = (crc >> 1);
		}
	}
	
	return crc;
}

-(uint8_t *)escapeByte:(uint8_t)data intoBuffer:(uint8_t *)buffer
{
	if (data == startDelimiterByte || data == endDelimiterByte || data == controlEscapeByte)
	{
		*buffer++ = controlEscapeByte;
		*buffer++ = data ^ 0x20;		// toggle bit 5
	}
	else
	{
		*buffer++ = data;
	}
	return buffer;
}

@end
