//
//  TDCommunicator.h
//  Textplay
//
//  Created by Niklas Hauser on 07.10.12.
//  Copyright (c) 2012 Niklas Hauser. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef enum
{
	kTDMessageTypeAny,
	kTDMessageTypeUnicast,
	kTDMessageTypeMulticast,
	kTDMessageTypeBroadcast,
} TDMessageType;

@interface TDCommunicator : NSObject

@property (nonatomic) NSString *connectionURL;
@property (nonatomic, readonly) NSString *connectionStatus;
@property (nonatomic, readonly) NSString *connectionError;
@property (nonatomic) uint16_t address;
@property (nonatomic) uint16_t groupAddress;

+(id)sharedInstance;

-(void)sendFrameWithData:(NSData *)data;

@end
