//
//  TDPixel.h
//  Textplay
//
//  Created by Niklas Hauser on 24.09.12.
//  Copyright (c) 2012 Niklas Hauser. All rights reserved.
//

@interface TDPixel : CALayer
{
}

@property (nonatomic) UIColor *color;

-(id)initWithSize:(CGSize)size position:(CGPoint)position;

@end
