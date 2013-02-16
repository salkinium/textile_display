//
//  TDMatrix.m
//  Textplay
//
//  Created by Niklas Hauser on 24.09.12.
//  Copyright (c) 2012 Niklas Hauser. All rights reserved.
//

#import "TDSettingsViewController.h"
#import "TDMatrix.h"
#import "TDPixel.h"
#import "all_caps_3x5.h"
#import "arcade_classic.h"
#import "fixed_width_5x8.h"
#import "scripto_narrow.h"

DDDefineContext(kTDSettingsValueChanged);

@interface TDMatrix ()
{
	CALayer *_frameLayer;
	CGPoint _cursor;
	NSInteger _offset;
	NSTimer *_renderTimer;
	const uint8_t *_fontBuffer;
	NSInteger _scrollSpeed;
	
	int _previousColomn;
	int _previousRow;
	
	TDSettingsViewController *_settingsController;
	
	NSTimer *_dreamTimer;
	NSTimer *_dreamRed;
	NSTimer *_dreamGreen;
	NSTimer *_dreamBlue;
	NSTimer *_dreamUpdater;
	
	struct RgbColor colorCounter;
	struct RgbColor colorFading;
	struct RgbColor colorMaximum;
}

-(void)renderText;
-(BOOL)writeChar:(char)c;
-(const uint8_t *)fontForName:(NSString *)name;
-(void)updateDream;
-(void)updateRed;
-(void)updateGreen;
-(void)updateBlue;
-(void)updateMatrix;

@end

@implementation TDMatrix

-(void)awakeFromNib
{
	_rows = 8;
	_colomns = 8;
	_pixelArray = [[NSMutableArray alloc] init];
	_cursor = CGPointZero;
	_scrollSpeed = 13;
	_fontBuffer = [self fontForName:@"Fixed Width 5x8"];
	_previousColomn = -1;
	_previousRow = -1;
	_text = @"";
	
	CGFloat width = self.frame.size.width/_colomns;
	CGFloat height = self.frame.size.height/_rows;
	
	for (NSInteger row=0; row < _rows; row++)
	{
		for (NSInteger colomn=0; colomn < _colomns; colomn++)
		{
			TDPixel *pixel = [[TDPixel alloc] initWithSize:CGSizeMake(width, height) position:CGPointMake((colomn+0.5)*width, (row+0.5)*height)];
			[_pixelArray addObject:pixel];
			[self.layer addSublayer:pixel];
		}
	}
	
	_settingsController = [TDSettingsViewController sharedInstance];
	[_settingsController addObserver:self
						  forKeyPath:@"font"
							 options:NSKeyValueObservingOptionNew
							 context:kTDSettingsValueChanged];
	[_settingsController addObserver:self
						  forKeyPath:@"scrollSpeed"
							 options:NSKeyValueObservingOptionNew
							 context:kTDSettingsValueChanged];
	srand(70);
}

#pragma mark - data outputs

-(NSData *)frameData
{
	uint8_t *bytes = (uint8_t *) malloc(_rows*_colomns*3);
	
	if (bytes)
	{
		uint8_t *buffer = bytes;
		CGFloat red, green, blue, alpha;
		for (TDPixel *pixel in _pixelArray)
		{
			UIColor *color = pixel.color;
			[color red:&red green:&green blue:&blue alpha:&alpha];
			
			*buffer++ = (uint8_t)(red * 255);
			*buffer++ = (uint8_t)(green * 255);
			*buffer++ = (uint8_t)(blue * 255);
		}
		
		return [NSData dataWithBytesNoCopy:bytes length:_rows*_colomns*3];
	}
	return nil;
}

-(TDPixel *)pixelAtColomn:(NSInteger)colomn row:(NSInteger)row
{
	if (colomn < 0 || row < 0 || colomn >= _colomns || row >= _rows)
		return nil;
	
	return [_pixelArray objectAtIndex:row*_colomns + colomn];
}

-(void)clear
{
	[_renderTimer invalidate];
	[self fillWithColor:[UIColor blackColor]];
	[_delegate matrixModified:NO];
	_previousColomn = -1;
	_previousRow = -1;
	_text = @"";
	[self stopDreaming];
}

#pragma mark - dreaming

-(void)startDreaming
{
	[self clear];
	_dreamTimer = [NSTimer scheduledTimerWithTimeInterval:1.f/60
												   target:self
												 selector:@selector(updateDream)
												 userInfo:nil
												  repeats:YES];
	
	_dreamUpdater = [NSTimer scheduledTimerWithTimeInterval:1.f/20
												   target:self
												 selector:@selector(updateMatrix)
												 userInfo:nil
												  repeats:YES];
	
	_dreamRed   = [NSTimer scheduledTimerWithTimeInterval:(rand()%70)*1.f/1000.f
												   target:self
												 selector:@selector(updateRed)
												 userInfo:nil
												  repeats:NO];
	_dreamGreen = [NSTimer scheduledTimerWithTimeInterval:(rand()%80)*1.f/1000
												   target:self
												 selector:@selector(updateGreen)
												 userInfo:nil
												  repeats:NO];
	_dreamBlue  = [NSTimer scheduledTimerWithTimeInterval:(rand()%90)*1.f/1000
												   target:self
												 selector:@selector(updateBlue)
												 userInfo:nil
												  repeats:NO];
//	NSLog(@"startDreaming");
}

-(void)stopDreaming
{
//	NSLog(@"stopDreaming");
	[_dreamTimer invalidate];
	[_dreamRed   invalidate];
	[_dreamGreen invalidate];
	[_dreamBlue  invalidate];
	[_dreamUpdater invalidate];
}

-(void)updateRed
{
	colorFading.red -= 1;
//	colorCounter.red -= 1;
//	colorFading.red = colorCounter.red;
//	
//	if (colorCounter.red > colorMaximum.red)
//		colorFading.red = colorMaximum.red;
	
	_dreamRed   = [NSTimer scheduledTimerWithTimeInterval:(rand()%70)*1.f/1000
												   target:self
												 selector:@selector(updateRed)
												 userInfo:nil
												  repeats:NO];
}

-(void)updateGreen
{
	colorFading.green -= 1;
//	colorCounter.green -= 1;
//	colorFading.green = colorCounter.green;
//	
//	if (colorCounter.green > colorMaximum.green)
//		colorFading.green = colorMaximum.green;
	
	_dreamGreen = [NSTimer scheduledTimerWithTimeInterval:(rand()%80)*1.f/1000
												   target:self
												 selector:@selector(updateGreen)
												 userInfo:nil
												  repeats:NO];
}

-(void)updateBlue
{
	colorFading.blue -= 1;
//	colorCounter.blue -= 1;
//	colorFading.blue = colorCounter.blue;
//	
//	if (colorCounter.blue > colorMaximum.blue)
//		colorFading.blue = colorMaximum.blue;
	
	_dreamBlue  = [NSTimer scheduledTimerWithTimeInterval:(rand()%90)*1.f/1000
												   target:self
												 selector:@selector(updateBlue)
												 userInfo:nil
												  repeats:NO];
}

-(void)updateMatrix
{
	[self.delegate matrixModified:YES];
}

-(void)updateDream
{
//	NSLog(@"current: %i %i %i", colorFading.red, colorFading.green, colorFading.blue);
	int i=1;
	for (TDPixel *pixel in _pixelArray)
	{
		uint8_t red   = colorFading.red   + (i+32)* 8;
		if (red > colorMaximum.red) red = colorMaximum.red;
		
		uint8_t green = colorFading.green + (i+32)*10;
		if (green > colorMaximum.green) green = colorMaximum.green;
		
		uint8_t blue  = colorFading.blue  + (i+32)*12;
		if (blue > colorMaximum.blue) blue = colorMaximum.blue;
		
		UIColor *color = [UIColor colorWithRGBHex:((red << 16) | (green << 8) | blue)];
		
		pixel.color = color;
		i++;
	}
	
	CGFloat red, green, blue, alpha;
	[_currentColor red:&red green:&green blue:&blue alpha:&alpha];
	struct RgbColor max = {red*255, green*255, blue*255};
	
//	NSLog(@"max: %i %i %i", max.red, max.green, max.blue);
	
	if (colorMaximum.red > max.red)
		colorMaximum.red--;
	else if (colorMaximum.red < max.red)
		colorMaximum.red++;
	
	if (colorMaximum.green > max.green)
		colorMaximum.green--;
	else if (colorMaximum.green < max.green)
		colorMaximum.green++;
	
	if (colorMaximum.blue > max.blue)
		colorMaximum.blue--;
	else if (colorMaximum.blue < max.blue)
		colorMaximum.blue++;
	
//	NSLog(@"current max: %i %i %i", colorMaximum.red, colorMaximum.green, colorMaximum.blue);
}

#pragma mark - font rendering

-(void)setCurrentColor:(UIColor *)currentColor
{
	_currentColor = currentColor;
	_previousColomn = -1;
	_previousRow = -1;
	
	if (!([_text isEqualToString:@""] || !_renderTimer || [_renderTimer isValid]))
	{
		_offset++;
		[self renderText];
		[self pauseRendering];
	}
}

-(void)setText:(NSString *)text
{
	_text = text;
	[self stopDreaming];
	
	if (![text isEqualToString:@""])
	{
		_offset = 2;
		
		[_renderTimer invalidate];
		[self renderText];
	}
	else
	{
		[self clear];
	}
}

-(void)renderText
{
	[self fillWithColor:[UIColor blackColor]];
	
	_cursor = CGPointMake(_offset, _cursor.y);
	
	for (NSInteger i=0; i < [_text length]; ++i)
	{
		[self writeChar:[_text characterAtIndex:i]];
	}
	
	int width = _cursor.x - _offset;
	_offset--;
	
	if (_offset < -(width + 10))
		_offset = _colomns;
	
	[self.delegate matrixModified:YES];
	
	_renderTimer = [NSTimer scheduledTimerWithTimeInterval:1.f/_scrollSpeed
													target:self
												  selector:@selector(renderText)
												  userInfo:nil
												   repeats:NO];
}

-(void)pauseRendering
{
	[_renderTimer invalidate];
}

-(void)resumeRendering
{
	if (!([_text isEqualToString:@""] || !_renderTimer || [_renderTimer isValid]))
	{
		[self renderText];
	}
}

-(BOOL)writeChar:(char)c
{
	if (!_fontBuffer)
		return NO;
	
	const uint8_t character = (uint8_t) c;
	const uint8_t height = _fontBuffer[3];
	//	const uint8_t hspace = _fontBuffer[4];
	const uint8_t vspace = _fontBuffer[5];
	
	const uint8_t first = _fontBuffer[6];
	const uint8_t count = _fontBuffer[7];
	
	if (character >= (first + count) || character < first) {
		// character is not contained in this font set
		return NO;
	}
	
	const uint8_t offsetWidthTable = 8;
	
	uint16_t offset = count + offsetWidthTable;
	uint8_t position = character - first + offsetWidthTable;
	const uint8_t usedRows = (height + 7) / 8;	// round up
	for (uint8_t i = offsetWidthTable; i < position; i++)
	{
		offset += _fontBuffer[i] * usedRows;
	}
	uint8_t width = _fontBuffer[position];
	
	CGRect frame = CGRectMake(_cursor.x, _cursor.y, width, height);
	[self drawImage:(_fontBuffer + offset) withFrame:frame];
	
	_cursor = CGPointMake(_cursor.x + width, _cursor.y);
	
	// all characters beyond 128 have whitespaces afterwards (number given
	// by vspace).
	if (character < 128)
	{
		for (uint_fast8_t i = 0; i < vspace; ++i)
		{
			[self drawVerticalLineAtPoint:_cursor withLength:height inColor:[UIColor blackColor]];
			_cursor = CGPointMake(_cursor.x + 1, _cursor.y);
		}
	}
	
	return YES;
}

-(const uint8_t *)fontForName:(NSString *)name
{
	_cursor = CGPointMake(_cursor.x, 0);
	if ([name isEqualToString:@"All Caps 3x5"])
	{
		_cursor = CGPointMake(_cursor.x, 1);
		return AllCaps3x5;
	}
	else if ([name isEqualToString:@"Arcade Classic"])
	{
		return ArcadeClassic;
	}
	else if ([name isEqualToString:@"Fixed Width 5x8"])
	{
		return FixedWidth5x8;
	}
	else if ([name isEqualToString:@"Scripto Narrow"])
	{
		_cursor = CGPointMake(_cursor.x, 1);
		return ScriptoNarrow;
	}
	return 0;
}

#pragma mark - primitive rendering

-(void)fillWithColor:(UIColor *)color
{
	for (TDPixel *pixel in _pixelArray)
	{
		[pixel setColor:color];
	}
}

-(void)drawImage:(const uint8_t *)data
	   withFrame:(CGRect)frame
{
	uint8_t rows = (frame.size.height + 7) / 8;
	for (uint8_t i = 0; i < frame.size.width; i++)
	{
		for (uint8_t k = 0; k < rows; k++)
		{
			uint8_t byte = data[i + k * (int)frame.size.width];
			uint8_t rowHeight = frame.size.height - k * 8;
			
			if (rowHeight > 8)
			{
				rowHeight = 8;
			}
			for (uint8_t j = 0; j < rowHeight; j++)
			{
				TDPixel *pixel = [self pixelAtColomn:(frame.origin.x + i) row:(frame.origin.y + k * 8 + j)];
				
				if (byte & 0x01)
				{
					[pixel setColor:_currentColor];
				}
				else
				{
					[pixel setColor:[UIColor blackColor]];
				}
				byte >>= 1;
			}
		}
	}
}

-(void)drawVerticalLineAtPoint:(CGPoint)start
					withLength:(NSInteger)length
					   inColor:(UIColor *)color;
{
	for (uint8_t i = start.y; i < (start.y + length); ++i)
	{
		[[self pixelAtColomn:start.x row:i] setColor:color];
	}
}

#pragma mark - observers

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object
						change:(NSDictionary *)change
					   context:(void *)context
{
	if (context == kTDSettingsValueChanged)
	{
		if ([keyPath isEqual:@"font"])
		{
			_fontBuffer = [self fontForName:[change objectForKey:NSKeyValueChangeNewKey]];
		}
		else if ([keyPath isEqual:@"scrollSpeed"])
		{
			_scrollSpeed = [[change objectForKey:NSKeyValueChangeNewKey] integerValue];
		}
	}
	else
	{
		[super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
	}
}

#pragma mark - touch rendering

-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	[super touchesBegan:touches withEvent:event];
	[self touchesMoved:touches withEvent:event];
}

-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	[super touchesMoved:touches withEvent:event];
	
	CGPoint tappedPt = [[touches anyObject] locationInView:self];
	
	int colomn = tappedPt.x / self.frame.size.width * _colomns;
	if (colomn >= _colomns) colomn = _colomns - 1;
	
	int row = tappedPt.y / self.frame.size.height * _rows;
	if (row >= _rows) row = _rows - 1;
	
	// minimize display updates to pixel updates
	if (_previousColomn != colomn || _previousRow != row)
	{
		_previousColomn = colomn;
		_previousRow = row;
		
		TDPixel *pixel = [self pixelAtColomn:colomn row:row];
		[pixel setColor:self.currentColor];
		
		[self.delegate matrixModified:YES];
	}
}


@end
