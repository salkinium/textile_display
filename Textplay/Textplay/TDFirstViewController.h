//
//  TDFirstViewController.h
//  Textplay
//
//  Created by Niklas Hauser on 24.09.12.
//  Copyright (c) 2012 Niklas Hauser. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "HRColorPickerViewController.h"
#import "TDSettingsViewController.h"
#import "TDMatrix.h"
@class TDSettingsViewController;
@class TDCommunicator;

@interface TDFirstViewController : UIViewController<HRColorPickerViewControllerDelegate, TDMatrixDelegate>
{
	IBOutlet TDMatrix *_matrix;
	IBOutlet UIButton *_colorButton;
	IBOutlet UIButton *_playButton;
	IBOutlet UITextField *_textField;
	IBOutlet UILabel *_connectionStatusLabel;
	IBOutlet UIButton *_clearButton;
	
	BOOL _continousRefresh;
	
	TDSettingsViewController *_settingsController;
	TDCommunicator *_communicator;
}

-(IBAction)selectPlayback:(id)sender;
-(IBAction)selectColor:(id)sender;
-(IBAction)resetMatrix:(id)sender;
-(IBAction)setText:(id)sender;
-(IBAction)showSettings:(id)sender;

@end
