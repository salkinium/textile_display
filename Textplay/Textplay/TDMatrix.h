//
//  TDMatrix.h
//  Textplay
//
//  Created by Niklas Hauser on 24.09.12.
//  Copyright (c) 2012 Niklas Hauser. All rights reserved.
//

#import <UIKit/UIKit.h>
@class TDPixel;
@class TDSettingsViewController;

@protocol TDMatrixDelegate
-(void)matrixModified:(BOOL)matrixWritten;
@end

@interface TDMatrix : UIView
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
}

@property (unsafe_unretained) id<TDMatrixDelegate> delegate;
@property (nonatomic) UIColor *currentColor;
@property (nonatomic, readonly) NSInteger rows;
@property (nonatomic, readonly) NSInteger colomns;
@property (nonatomic, readonly) NSMutableArray *pixelArray;
@property (nonatomic) NSString *text;

-(NSData *)frameData;
-(TDPixel *)pixelAtColomn:(NSInteger)colomn row:(NSInteger)row;
-(void)clear;
-(void)pauseRendering;
-(void)resumeRendering;

-(void)fillWithColor:(UIColor *)color;
-(void)drawImage:(const uint8_t *)data
	   withFrame:(CGRect)frame;
-(void)drawVerticalLineAtPoint:(CGPoint)start
					withLength:(NSInteger)length
					   inColor:(UIColor *)color;

@end
