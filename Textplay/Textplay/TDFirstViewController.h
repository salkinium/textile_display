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

@interface TDFirstViewController : UIViewController<HRColorPickerViewControllerDelegate, TDMatrixDelegate>

@end
