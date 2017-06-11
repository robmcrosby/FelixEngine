//
//  ViewController.m
//  HelloTriangle
//
//  Created by Robert Crosby on 6/11/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#import "ViewController.h"
#import <FelixEngine/Engine.h>

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
  [super viewDidLoad];
  
  [[Engine instance] test];
}


- (void)didReceiveMemoryWarning {
  [super didReceiveMemoryWarning];
  // Dispose of any resources that can be recreated.
}


@end
