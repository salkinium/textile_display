//
//  TDPixel.m
//  Textplay
//
//  Created by Niklas Hauser on 24.09.12.
//  Copyright (c) 2012 Niklas Hauser. All rights reserved.
//

#import "TDPixel.h"
#import "ADPath.h"

@implementation TDPixel

-(id)initWithSize:(CGSize)size position:(CGPoint)position
{
	if (self = [super initWithBounds:CGRectMake(0, 0, size.width, size.height) position:position anchorPoint:CGPointMakeHalf delegate:self])
	{
		_color = [UIColor blackColor];
	}
	
	return self;
}

-(void)dealloc
{
	_color = nil;
}

-(void)drawInContext:(CGContextRef)ctx
{
	const CGFloat _radius = MIN(self.bounds.size.height/2, self.bounds.size.width/2);
	
	[ADPath fillEllipse:CGRectMake(_radius*0.95, _radius*0.95, _radius*0.1, _radius*0.1) inContext:ctx inColor:[UIColor whiteColor]];
}


-(void)setColor:(UIColor *)color
{
	_color = color;
	self.backgroundColor = color.CGColor;
}

@end
