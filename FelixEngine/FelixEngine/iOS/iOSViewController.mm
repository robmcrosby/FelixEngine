//
//  iOSViewController.mm
//  FelixEngine
//
//  Created by Robert Crosby on 6/13/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#import "iOSViewController.h"
#import "Application.h"

@interface iOSViewController ()

@property fx::Application *application;

@end


@implementation iOSViewController

- (void)viewDidLoad {
  [super viewDidLoad];
  self.application = nil;
}

- (void)didReceiveMemoryWarning {
  [super didReceiveMemoryWarning];
}

//-(void)setApplication:(void*)app {
//  self.application = static_cast<fx::Application*>(app);
//}

@end
