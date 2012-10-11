//
//  CALayer.m
//  Flight Display
//
//  Created by Niklas Hauser on 30.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "CALayer+Init.h"

@implementation CALayer (CALayer_Init)

- (id)initWithBounds:(CGRect)bounds
			position:(CGPoint)position
{
	return [self initWithBounds:bounds position:position anchorPoint:CGPointMake(0.5, 0.5) delegate:nil];
}

- (id)initWithBounds:(CGRect)bounds
			position:(CGPoint)position
		 anchorPoint:(CGPoint)anchorPoint
{
	return [self initWithBounds:bounds position:position anchorPoint:anchorPoint delegate:nil];
}

- (id)initWithBounds:(CGRect)bounds
			position:(CGPoint)position
			delegate:(id)delegate
{
	return [self initWithBounds:bounds position:position anchorPoint:CGPointMake(0.5, 0.5) delegate:delegate];
}

- (id)initWithBounds:(CGRect)bounds
			position:(CGPoint)position
		anchorPoint:(CGPoint)anchorPoint
		   delegate:(id)delegate
{
	if (self = [self init])
	{
		self.bounds = bounds;
		self.position = position;
		self.anchorPoint = anchorPoint;
		self.delegate = delegate;
		self.contentsScale = UIScreen.mainScreen.scale;
		
		self.actions = @{
			@"onOrderIn" : [NSNull null],
			@"onOrderOut" : [NSNull null],
			@"sublayers" : [NSNull null],
			@"contents" : [NSNull null],
			@"bounds" : [NSNull null],
			@"position" : [NSNull null],
			@"hidden" : [NSNull null],
			@"anchorPoint" : [NSNull null],
			@"transform" : [NSNull null],
			@"backgroundColor" : [NSNull null]
		};
		
#if LAYER_DEBUG_BORDER
		self.borderWidth = 0.5;
		self.borderColor = [UIColor whiteColor].CGColor;
#endif
		
		if (delegate) [self setNeedsDisplay];
	}
	return self;
}


+ (id)layerWithBounds:(CGRect)bounds
			 position:(CGPoint)position
{
	return [CALayer layerWithBounds:bounds position:position anchorPoint:CGPointMake(0.5, 0.5) delegate:nil];
}

+ (id)layerWithBounds:(CGRect)bounds
			 position:(CGPoint)position
		 anchorPoint:(CGPoint)anchorPoint
{
	return [CALayer layerWithBounds:bounds position:position anchorPoint:anchorPoint delegate:nil];
}

+ (id)layerWithBounds:(CGRect)bounds
			 position:(CGPoint)position
			 delegate:(id)delegate
{
	return [[CALayer alloc] initWithBounds:bounds position:position anchorPoint:CGPointMake(0.5, 0.5) delegate:delegate];
}

+ (id)layerWithBounds:(CGRect)bounds
			 position:(CGPoint)position
		 anchorPoint:(CGPoint)anchorPoint
			delegate:(id)delegate
{
	return [[CALayer alloc] initWithBounds:bounds position:position anchorPoint:anchorPoint delegate:delegate];
}

@end
