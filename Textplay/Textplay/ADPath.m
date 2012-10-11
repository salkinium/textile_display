//
//  CAPath.m
//  flightcore-cockpit
//
//  Created by Niklas Hauser on 3/11/11.
//  Copyright 2011 Niklas Hauser. All rights reserved.
//

#import "ADPath.h"

@implementation ADPath

@synthesize lineCap;
@synthesize lineJoin;
@synthesize fillColor;
@synthesize shadow=_shadow;
@synthesize path=_path;
@synthesize lineColor;
@synthesize lineWidth;

-(id)init
{
	return nil;
}

-(id)initWithPath:(CGMutablePathRef)path
	withLineColor:(UIColor *)color
	withLineWidth:(CGFloat)width
	   withShadow:(BOOL)shadow
{
	if (self = [super init])
	{
		_path = path;
		lineColor = color;
		lineWidth = width;
		fillColor = nil;
		_shadow = shadow;
		lineCap = kCGLineCapButt;
		lineJoin = kCGLineJoinMiter;
	}
	return self;
}

-(void)dealloc
{
	CGPathRelease(_path);
}

#pragma mark -
#pragma mark private path methods

-(void)addPath:(CGMutablePathRef)otherPath
{
	CGPathAddPath(_path, NULL, otherPath);
}

-(void)addLayerPath:(ADPath *)layerPath
{
	CGPathAddPath(_path, NULL, [layerPath path]);
}

#pragma mark private paint methods

-(void)drawPathInContext:(CGContextRef)ctx
{
	[ADPath drawPath:_path inContext:ctx inColor:lineColor
		   withLineWidth:lineWidth withLineCap:lineCap
			withLineJoin:lineJoin withShadow:shadow];
}

-(void)drawPathInContext:(CGContextRef)ctx
	   withTransformation:(const CGAffineTransform *)m
{// create an empty path and buffer the transformed path in it.
	CGMutablePathRef tempPath = CGPathCreateMutable();
	CGPathAddPath(tempPath, m, _path);
	// Paint the buffered path
	[ADPath drawPath:tempPath inContext:ctx inColor:lineColor
		   withLineWidth:lineWidth withLineCap:lineCap
			withLineJoin:lineJoin withShadow:shadow];
	
	CGPathRelease(tempPath);
}

-(void)drawPathInContext:(CGContextRef)ctx
		  withTranslation:(CGPoint)translate
{
	CGContextSaveGState(ctx);
	CGContextTranslateCTM(ctx, translate.x, translate.y);
	[ADPath drawPath:_path inContext:ctx inColor:lineColor
		   withLineWidth:lineWidth withLineCap:lineCap
			withLineJoin:lineJoin withShadow:shadow];
	CGContextRestoreGState(ctx);
}

-(void)drawPathInContext:(CGContextRef)ctx
				  addPath:(CGMutablePathRef)otherPath
{
	CGPathAddPath(otherPath, NULL, _path);
	// Paint the buffered path
	[ADPath drawPath:otherPath inContext:ctx inColor:lineColor
		   withLineWidth:lineWidth withLineCap:lineCap
			withLineJoin:lineJoin withShadow:shadow];
}

#pragma mark private fill methods

-(BOOL)fillPathInContext:(CGContextRef)ctx
{
	if (fillColor != nil) {
		[ADPath fillPath:_path inContext:ctx withColor:fillColor];
		return TRUE;
	}
	return FALSE;
}

-(void)fillPathInContext:(CGContextRef)ctx
		 withTranslation:(CGPoint)translate
{
	CGContextSaveGState(ctx);
	CGContextTranslateCTM(ctx, translate.x, translate.y);
	[self fillPathInContext:ctx];
	CGContextRestoreGState(ctx);
}

#pragma mark -
#pragma mark public paint methods

+(void)drawPath:(CGPathRef)path
	   inContext:(CGContextRef)ctx
		 inColor:(UIColor *)color
   withLineWidth:(CGFloat)width
	 withLineCap:(CGLineCap)cap
	withLineJoin:(CGLineJoin)join
	  withShadow:(BOOL)shadow
{
	CGContextSaveGState(ctx);
	
	CGContextSetLineCap(ctx, cap);
	CGContextSetLineJoin(ctx, join);
	if (shadow) {
		CGContextSetStrokeColorWithColor(ctx, [UIColor  blackColor].CGColor);
		CGContextBeginPath(ctx);
		CGContextAddPath(ctx, path);
		CGContextSetLineWidth(ctx, width * 1.2);
		CGContextStrokePath(ctx);
	}
	CGContextSetStrokeColorWithColor(ctx, color.CGColor);
	CGContextBeginPath(ctx);
	CGContextAddPath(ctx, path);
	CGContextSetLineWidth(ctx, width);
	CGContextStrokePath(ctx);
	
	CGContextRestoreGState(ctx);
}

+(void)drawRect:(CGRect)rect
	   inContext:(CGContextRef)ctx
		 inColor:(UIColor *)color
   withLineWidth:(CGFloat)width
	withLineJoin:(CGLineJoin)join
	  withShadow:(BOOL)shadow
{
	CGContextSaveGState(ctx);
	
	CGContextSetLineJoin(ctx, join);
	if (shadow) {
		CGContextBeginPath(ctx);
		CGContextSetStrokeColorWithColor(ctx, [UIColor  blackColor].CGColor);
		CGContextStrokeRectWithWidth(ctx, rect, width*1.2);
		CGContextStrokePath(ctx);
	}
	CGContextBeginPath(ctx);
	CGContextSetStrokeColorWithColor(ctx, color.CGColor);
	CGContextStrokeRectWithWidth(ctx, rect, width);
	CGContextStrokePath(ctx);
	
	CGContextRestoreGState(ctx);
}

+(void)drawEllipse:(CGRect)rect
		  inContext:(CGContextRef)ctx
			inColor:(UIColor *)color
	  withLineWidth:(CGFloat)width
		 withShadow:(BOOL)shadow
{
	CGContextSaveGState(ctx);
	
	if (shadow) {
		CGContextBeginPath(ctx);
		CGContextSetStrokeColorWithColor(ctx, [UIColor  blackColor].CGColor);
		CGContextSetLineWidth(ctx, width * 1.2);
		CGContextStrokeEllipseInRect(ctx, rect);
		CGContextStrokePath(ctx);
	}
	CGContextBeginPath(ctx);
	CGContextSetStrokeColorWithColor(ctx, color.CGColor);
	CGContextSetLineWidth(ctx, width);
	CGContextStrokeEllipseInRect(ctx, rect);
	CGContextStrokePath(ctx);
	
	CGContextRestoreGState(ctx);
}

#pragma mark public fill methods

+(void)fillPath:(CGPathRef)path
	  inContext:(CGContextRef)ctx
	  withColor:(UIColor *)color
{
	CGContextSaveGState(ctx);
	
	CGContextBeginPath(ctx);
	CGContextAddPath(ctx, path);
	CGContextSetFillColorWithColor(ctx, color.CGColor);
	CGContextFillPath(ctx);
	
	CGContextRestoreGState(ctx);
}

+(void)fillRect:(CGRect)rect
	  inContext:(CGContextRef)ctx
		inColor:(UIColor *)color
{
	CGContextSaveGState(ctx);
	
	CGContextBeginPath(ctx);
	CGContextSetFillColorWithColor(ctx, color.CGColor);
	CGContextFillRect(ctx, rect);
	CGContextStrokePath(ctx);
	
	CGContextRestoreGState(ctx);
}

+(void)fillEllipse:(CGRect)rect
		 inContext:(CGContextRef)ctx
		   inColor:(UIColor *)color
{
	CGContextSaveGState(ctx);
	
	CGContextBeginPath(ctx);
	CGContextSetFillColorWithColor(ctx, color.CGColor);
	CGContextFillEllipseInRect(ctx, rect);
	CGContextStrokePath(ctx);
	
	CGContextRestoreGState(ctx);
}


@end
