//
//  TDSettingsViewController.m
//  Textplay
//
//  Created by Niklas Hauser on 27.09.12.
//  Copyright (c) 2012 Niklas Hauser. All rights reserved.
//

#import "TDSettingsViewController.h"
#import "ActionSheetStringPicker.h"
#import "TDCommunicator.h"

DDDefineContext(kTDCommunicatorValueChanged);

@interface TDSettingsViewController ()
@property (nonatomic, readwrite) NSString *font;
@property (nonatomic, readwrite) NSInteger scrollSpeed;

@end

@implementation TDSettingsViewController

@synthesize connectionURL = _connectionURL;
@synthesize font = _font;
@synthesize scrollSpeed = _scrollSpeed;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])
	{
        // Custom initialization
		self.title = @"Settings";
		
		_connectionURL = @"";
		self.font = @"Fixed Width 5x8";
		self.scrollSpeed = 13;
		_fontArray = @[@"All Caps 3x5", @"Arcade Classic", @"Fixed Width 5x8", @"Scripto Narrow"];
		
		_communicator = [TDCommunicator sharedInstance];
    }
    return self;
}

+ (id)sharedInstance
{
	DEFINE_SHARED_INSTANCE_USING_BLOCK(^{
		return [[self alloc] initWithNibName:@"TDSettingsViewController" bundle:nil];
	});
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	
//	_ipAddressPortText.text = _connectionURL;
	_statusLabel.text = [_communicator connectionStatus];
	_fontName.text = _font;
	
	_speedStepper.minimumValue = 1;
	_speedStepper.maximumValue = 25;
	_speedStepper.stepValue = 1;
	_speedStepper.wraps = NO;
	_speedStepper.autorepeat = YES;
	_speedStepper.continuous = YES;
	_speedStepper.value = _scrollSpeed;
	_speedLabel.text = [NSString stringWithFormat:@"%d", _scrollSpeed];
	
	[_communicator addObserver:self forKeyPath:@"connectionStatus"
					   options:NSKeyValueObservingOptionNew
					   context:kTDCommunicatorValueChanged];
	[_communicator addObserver:self forKeyPath:@"connectionError"
					   options:NSKeyValueObservingOptionNew
					   context:kTDCommunicatorValueChanged];
	[self setIpAddressPort:_ipAddressPortText];
	
    // Do any additional setup after loading the view from its nib.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
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
			_statusLabel.text = [change objectForKey:NSKeyValueChangeNewKey];
		}
		else if ([keyPath isEqual:@"connectionError"])
		{
			_statusLabel.text = [change objectForKey:NSKeyValueChangeNewKey];
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

-(IBAction)speedValueChanged:(id)sender
{
	self.scrollSpeed = _speedStepper.value;
	_speedLabel.text = [NSString stringWithFormat:@"%d", _scrollSpeed];
}

-(IBAction)setIpAddressPort:(id)sender
{
	_connectionURL = [sender text];
	NSLog(@"%@", _connectionURL);
	
	_communicator.connectionURL = _connectionURL;
}

-(IBAction)showPicker:(id)sender
{
	[ActionSheetStringPicker showPickerWithTitle:@"Pick a font" rows:_fontArray initialSelection:[_fontArray indexOfObject:_font] doneBlock:^(ActionSheetStringPicker *picker, NSInteger selectedIndex, id selectedValue) {
		self.font = selectedValue;
		_fontName.text = selectedValue;
	} cancelBlock:^(ActionSheetStringPicker *picker) {
		NSLog(@"font picking cancelled");
	} origin:sender];
}

@end
