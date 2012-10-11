//
//  CAPath.h
//  
//
//  Created by Niklas Hauser on 3/11/11.
//  Copyright 2011 Niklas Hauser. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>

@interface ADPath : NSObject

@property (nonatomic) CGMutablePathRef path;
@property (strong, nonatomic) UIColor *lineColor;
@property (strong, nonatomic) UIColor *fillColor;
@property (nonatomic) CGFloat lineWidth;
@property (nonatomic) CGLineCap lineCap;
@property (nonatomic) CGLineJoin lineJoin;
@property (nonatomic) BOOL shadow;

-(id)initWithPath:(CGMutablePathRef)_path withLineColor:(UIColor *)color
	withLineWidth:(CGFloat)width withShadow:(BOOL)shadow;

-(void)addPath:(CGMutablePathRef)otherPath;
-(void)addLayerPath:(ADPath *)layerPath;

-(void)drawPathInContext:(CGContextRef)ctx;
-(void)drawPathInContext:(CGContextRef)ctx withTransformation:(const CGAffineTransform *)m;
-(void)drawPathInContext:(CGContextRef)ctx withTranslation:(CGPoint)translate;
-(void)drawPathInContext:(CGContextRef)ctx addPath:(CGMutablePathRef)otherPath;

-(BOOL)fillPathInContext:(CGContextRef)ctx;
-(void)fillPathInContext:(CGContextRef)ctx withTranslation:(CGPoint)translate;


+(void)drawPath:(CGPathRef)path inContext:(CGContextRef)ctx inColor:(UIColor *)color
   withLineWidth:(CGFloat)width withLineCap:(CGLineCap)cap withLineJoin:(CGLineJoin)join withShadow:(BOOL)shadow;
+(void)drawRect:(CGRect)rect inContext:(CGContextRef)ctx inColor:(UIColor *)color
   withLineWidth:(CGFloat)width withLineJoin:(CGLineJoin)join withShadow:(BOOL)shadow;
+(void)drawEllipse:(CGRect)rect inContext:(CGContextRef)ctx inColor:(UIColor *)color
	  withLineWidth:(CGFloat)width withShadow:(BOOL)shadow;

+(void)fillPath:(CGPathRef)path inContext:(CGContextRef)ctx withColor:(UIColor *)color;
+(void)fillRect:(CGRect)rect inContext:(CGContextRef)ctx inColor:(UIColor *)color;
+(void)fillEllipse:(CGRect)rect inContext:(CGContextRef)ctx inColor:(UIColor *)color;

@end
