//
//  CALayer.h
//  Flight Display
//
//  Created by Niklas Hauser on 30.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>

@interface CALayer (CALayer_Init)

- (id)initWithBounds:(CGRect)bounds
		  position:(CGPoint)position;

- (id)initWithBounds:(CGRect)bounds
		  position:(CGPoint)position
	   anchorPoint:(CGPoint)anchorPoint;

- (id)initWithBounds:(CGRect)bounds
			position:(CGPoint)position
			delegate:(id)delegate;

- (id)initWithBounds:(CGRect)bounds
		  position:(CGPoint)position
	   anchorPoint:(CGPoint)anchorPoint
		  delegate:(id)delegate;


+ (id)layerWithBounds:(CGRect)bounds
		   position:(CGPoint)position;

+ (id)layerWithBounds:(CGRect)bounds
		   position:(CGPoint)position
		anchorPoint:(CGPoint)anchorPoint;

+ (id)layerWithBounds:(CGRect)bounds
			 position:(CGPoint)position
			 delegate:(id)delegate;

+ (id)layerWithBounds:(CGRect)bounds
		   position:(CGPoint)position
		anchorPoint:(CGPoint)anchorPoint
		   delegate:(id)delegate;
@end
