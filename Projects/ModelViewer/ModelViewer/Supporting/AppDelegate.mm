//
//  AppDelegate.m
//  ModelViewer
//
//  Created by Robert Crosby on 12/21/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#import "AppDelegate.h"
#import "DetailViewController.h"
#import "BrowserViewController.h"

@interface AppDelegate ()

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
  
  [self setupSplitView];
  [self setupUbiquity];
  
  return YES;
}

- (void)setupSplitView {
  UISplitViewController *splitViewController = (UISplitViewController*)self.window.rootViewController;
  UINavigationController *navigationController = (UINavigationController*)[splitViewController.viewControllers firstObject];
  
  BrowserViewController *browserViewController = (BrowserViewController*)navigationController.topViewController;
  browserViewController.detailViewController = (DetailViewController*)[splitViewController.viewControllers lastObject];
}

- (void)setupUbiquity {
  NSFileManager* fileManager = [NSFileManager defaultManager];
  id currentiCloudToken = fileManager.ubiquityIdentityToken;
  
  if (currentiCloudToken) {
    NSData *newTokenData = [NSKeyedArchiver archivedDataWithRootObject: currentiCloudToken];
    [[NSUserDefaults standardUserDefaults] setObject: newTokenData forKey: @"com.FelixEngine.ModelViewer.UbiquityIdentityToken"];
  } else {
    [[NSUserDefaults standardUserDefaults] removeObjectForKey: @"com.FelixEngine.ModelViewer.UbiquityIdentityToken"];
  }
}


- (void)applicationWillResignActive:(UIApplication *)application {
  // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
  // Use this method to pause ongoing tasks, disable timers, and invalidate graphics rendering callbacks. Games should use this method to pause the game.
}


- (void)applicationDidEnterBackground:(UIApplication *)application {
  // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
  // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}


- (void)applicationWillEnterForeground:(UIApplication *)application {
  // Called as part of the transition from the background to the active state; here you can undo many of the changes made on entering the background.
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
  // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}


- (void)applicationWillTerminate:(UIApplication *)application {
  // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}


@end
