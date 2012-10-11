//
//  TDSettingsViewController.h
//  Textplay
//
//  Created by Niklas Hauser on 27.09.12.
//  Copyright (c) 2012 Niklas Hauser. All rights reserved.
//

#import <UIKit/UIKit.h>
@class TDCommunicator;

@interface TDSettingsViewController : UIViewController
{
	IBOutlet UITextField *_ipAddressPortText;
	IBOutlet UILabel *_statusLabel;
	IBOutlet UITextField *_fontName;
	IBOutlet UIStepper *_speedStepper;
	IBOutlet UILabel *_speedLabel;
	
	NSArray *_fontArray;
	
	TDCommunicator *_communicator;
}

@property (nonatomic, readonly) NSString *connectionURL;
@property (nonatomic, readonly) NSString *font;
@property (nonatomic, readonly) NSInteger scrollSpeed;

+ (id)sharedInstance;

-(IBAction)setIpAddressPort:(id)sender;
-(IBAction)showPicker:(id)sender;
-(IBAction)speedValueChanged:(id)sender;

@end
