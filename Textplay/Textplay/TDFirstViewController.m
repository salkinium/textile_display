//
//  TDFirstViewController.m
//  Textplay
//
//  Created by Niklas Hauser on 24.09.12.
//  Copyright (c) 2012 Niklas Hauser. All rights reserved.
//

#import "TDFirstViewController.h"
#import "TDPixel.h"
#import "TDSettingsViewController.h"
#import "TDCommunicator.h"

DDDefineContext(kTDCommunicatorValueChanged);

@interface TDFirstViewController ()
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

@implementation TDFirstViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])
	{
		self.title = @"Matrix";
		
		_settingsController = [TDSettingsViewController sharedInstance];
		_communicator = [TDCommunicator sharedInstance];
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	
	_clearButton.hidden = YES;
	_playButton.hidden = YES;
	_matrix.delegate = self;
	_continousRefresh = YES;
	[self setSelectedColor:[UIColor yellowColor]];
	_colorButton.titleLabel.textColor = [UIColor blackColor];
	[_communicator setAddress:0x12ff];
	[_communicator setGroupAddress:0x12ff];
	// Do any additional setup after loading the view, typically from a nib.
	[_communicator addObserver:self forKeyPath:@"connectionStatus"
					   options:NSKeyValueObservingOptionNew
					   context:kTDCommunicatorValueChanged];
	_communicator.connectionURL = @"169.254.1.1";
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - delegates

- (void)setSelectedColor:(UIColor*)color
{
	_matrix.currentColor = color;
	_colorButton.backgroundColor = color;
	
	CGFloat red, green, blue, alpha;
	[color red:&red green:&green blue:&blue alpha:&alpha];
	
	_colorButton.titleLabel.textColor = ((red + green + blue) < 1.5) ? [UIColor whiteColor] : [UIColor blackColor];
}

-(void)matrixModified:(BOOL)matrixWritten
{
	_clearButton.hidden = !matrixWritten;
	_playButton.hidden = !([_connectionStatusLabel.text isEqualToString:@"connected"] || ![_matrix.text isEqualToString:@""]);
	
	if (_continousRefresh || ![_matrix.text isEqualToString:@""])
	{
		[_communicator sendFrameWithData:[_matrix frameData]];
	}
}

#pragma mark - observers

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object
						change:(NSDictionary *)change
					   context:(void *)context
{
	if (context == kTDCommunicatorValueChanged)
	{
		if ([keyPath isEqual:@"connectionStatus"])
		{
			_connectionStatusLabel.text = [change objectForKey:NSKeyValueChangeNewKey];
			_playButton.hidden = !([_connectionStatusLabel.text isEqualToString:@"connected"] || ![_matrix.text isEqualToString:@""]);
		}
	}
	else
	{
		[super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
	}
}

#pragma mark - actions

-(IBAction)textFieldFinished:(id)sender
{
	[sender resignFirstResponder];
}

-(IBAction)setText:(id)sender
{
	NSString *text = [sender text];
	
	if ([text isEqualToString:@":dc"])
	{
		[_matrix startDreaming];
		return;
	}
	
	[_matrix setText:text];
	
	if (!_continousRefresh)
	{
		[_matrix pauseRendering];
	}
}

-(IBAction)selectPlayback:(id)sender
{
	if ([_playButton.titleLabel.text isEqualToString:@"Paused"])
	{
		[_playButton setTitle:@"Running" forState:UIControlStateNormal];
		_continousRefresh = YES;
		[_communicator sendFrameWithData:[_matrix frameData]];
		[_matrix resumeRendering];
	}
	else
	{
		[_playButton setTitle:@"Paused" forState:UIControlStateNormal];
		_continousRefresh = NO;
		[_matrix pauseRendering];
	}
}

-(IBAction)selectColor:(id)sender
{
	HRColorPickerViewController* controller = [HRColorPickerViewController fullColorPickerViewControllerWithColor:_colorButton.backgroundColor];
    controller.delegate = self;
    [self.navigationController pushViewController:controller animated:YES];
}

-(IBAction)resetMatrix:(id)sender
{
	[_matrix clear];
	_textField.text = @"";
	_clearButton.hidden = YES;
	_playButton.hidden = !([_connectionStatusLabel.text isEqualToString:@"connected"] || ![_matrix.text isEqualToString:@""]);
}

-(IBAction)showSettings:(id)sender
{
	[self.navigationController pushViewController:_settingsController animated:YES];
}

@end
