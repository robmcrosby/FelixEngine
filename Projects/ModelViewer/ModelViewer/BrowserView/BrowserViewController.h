//
//  BrowserViewController.h
//  ModelViewer
//
//  Created by Robert Crosby on 1/7/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#import <UIKit/UIKit.h>

@class DetailViewController;

@interface BrowserViewController : UITableViewController <UIDocumentPickerDelegate>

@property (nonatomic) DetailViewController *detailViewController;

@end
