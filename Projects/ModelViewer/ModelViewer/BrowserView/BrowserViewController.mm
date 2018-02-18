//
//  BrowserViewController.m
//  ModelViewer
//
//  Created by Robert Crosby on 1/7/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#import "BrowserViewController.h"
#import "DetailViewController.h"

@interface BrowserViewController ()

//@property (nonatomic) NSURL *iCloudRoot;
//@property (nonatomic) BOOL iCloudAvailable;

//@property (nonatomic) NSMetadataQuery *query;
//@property (nonatomic) BOOL iCloudURLsReady;
//@property (nonatomic) NSMutableArray *iCloudURLs;


@property (nonatomic) NSURL *rootDocumentsDir;
@property (nonatomic) NSURL *workingDir;
@property (nonatomic) NSMutableArray *fileURLs;

@property (weak, nonatomic) IBOutlet UIBarButtonItem *backButton;

@property (nonatomic) UIDocumentPickerViewController *documentPicker;

@end


@implementation BrowserViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
  // Uncomment the following line to preserve selection between presentations.
  // self.clearsSelectionOnViewWillAppear = NO;
    
  // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
  self.navigationItem.rightBarButtonItem = self.editButtonItem;
  
  [self initDocumentsUrls];
  
//  _query = nil;
//  _iCloudURLsReady = NO;
//  _iCloudURLs = [NSMutableArray new];
  
  [self refresh];
}

- (void)viewWillAppear:(BOOL)animated {
//  [_query enableUpdates];
}

- (void)viewWillDisappear:(BOOL)animated {
//  [_query disableUpdates];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)initDocumentsUrls {
  NSArray *paths = [[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask];
  _rootDocumentsDir = [paths lastObject];
  _workingDir = _rootDocumentsDir;
  
  _fileURLs = [NSMutableArray new];
}

- (void)refresh {
  [_fileURLs removeAllObjects];
  
  NSFileManager *manager = [NSFileManager defaultManager];
  NSArray *files = [manager contentsOfDirectoryAtURL:_workingDir
                          includingPropertiesForKeys:nil
                                             options:NSDirectoryEnumerationSkipsHiddenFiles
                                               error:nil];
  
  if (files != nil) {
    [_fileURLs addObjectsFromArray:files];
  }
  
  [self.tableView reloadData];
}

#pragma mark - UIDocumentPicker Delegate Methods

- (void)showImportDocumentPicker {
  NSArray *types = @[@"public.image", @"public.plain-text", @"public.rtf"];
  _documentPicker = [[UIDocumentPickerViewController alloc] initWithDocumentTypes:types
                                                                           inMode:UIDocumentPickerModeImport];
  _documentPicker.delegate = self;
  _documentPicker.modalPresentationStyle = UIModalPresentationFormSheet;
  [self presentViewController:_documentPicker animated:YES completion:nil];
}

- (void)documentPicker:(UIDocumentPickerViewController *)controller didPickDocumentAtURL:(NSURL *)url {
  if (controller.documentPickerMode == UIDocumentPickerModeImport) {
    NSFileManager *manager = [NSFileManager defaultManager];
    
    if ([manager isReadableFileAtPath:url.path]) {
      NSURL *dst = [_workingDir URLByAppendingPathComponent:[url lastPathComponent]];
      [manager copyItemAtURL:url toURL:dst error:nil];
      [self refresh];
    }
    else {
      NSLog(@"File Not Readable");
    }
//
//    if ([[NSFileManager defaultManager] isReadableFileAtPath:source] )
//      [[NSFileManager defaultManager] copyItemAtURL:source toURL:destination error:nil];
    
//    NSString *alertMessage = [NSString stringWithFormat:@"Successfully imported %@", [url lastPathComponent]];
//    dispatch_async(dispatch_get_main_queue(), ^{
//      UIAlertController *alertController = [UIAlertController
//                                            alertControllerWithTitle:@"Import"
//                                            message:alertMessage
//                                            preferredStyle:UIAlertControllerStyleAlert];
//      [alertController addAction:[UIAlertAction actionWithTitle:@"Ok" style:UIAlertActionStyleDefault handler:nil]];
//      [self presentViewController:alertController animated:YES completion:nil];
//    });
  }
}

- (IBAction)browseButtonPress:(id)sender {
  [self showImportDocumentPicker];
}

- (IBAction)backButtonPress:(id)sender {
  NSString *rootDir = [_rootDocumentsDir.path stringByResolvingSymlinksInPath];
  NSString *workingDir = [_workingDir.path stringByResolvingSymlinksInPath];
  if (![rootDir isEqualToString:workingDir]) {
    _workingDir = [_workingDir URLByDeletingLastPathComponent];
    [self refresh];
  }
}

- (IBAction)createDirectoryButtonPress:(id)sender {
  NSLog(@"Create Directory Button Press");
  
  dispatch_async(dispatch_get_main_queue(), ^{
    UIAlertController *alertController = [UIAlertController
                                          alertControllerWithTitle:@"Create Directory"
                                          message:@""
                                          preferredStyle:UIAlertControllerStyleAlert];
    [alertController addTextFieldWithConfigurationHandler:^(UITextField *textField) {
      textField.placeholder = @"Directory Name";
    }];
    [alertController addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault
                                                      handler:^(UIAlertAction * action) {
                                                        UITextField *textField = [alertController.textFields firstObject];
                                                        [self createDirector:textField.text];
                                                      }]];
    [alertController addAction:[UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleDefault handler:nil]];
    [self presentViewController:alertController animated:YES completion:nil];
  });
}

- (void)createDirector:(NSString*)name {
  if (![name isEqualToString:@""]) {
    NSURL *newDir = [_workingDir URLByAppendingPathComponent:name];
    NSFileManager *manager = [NSFileManager defaultManager];
    if (![manager fileExistsAtPath:newDir.path]) {
      [manager createDirectoryAtURL:newDir withIntermediateDirectories:NO attributes:nil error:nil];
      [self refresh];
    }
  }
}

- (void)setEditing:(BOOL)editing animated:(BOOL)animated {
  [super setEditing:editing animated:animated];
  [self refresh];
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 2;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
  NSInteger rows = 0;
  if (section == 0) {
    rows += self.tableView.isEditing ? 1 : 0;
  }
  else {
    rows = [_fileURLs count];
  }
  return rows;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
  UITableViewCell *cell = nil;
  
  if (indexPath.section == 0) {
    cell = [tableView dequeueReusableCellWithIdentifier:@"BrowseCell" forIndexPath:indexPath];
  }
  else {
    cell = [tableView dequeueReusableCellWithIdentifier:@"FileCell" forIndexPath:indexPath];
    NSURL *fileUrl = [_fileURLs objectAtIndex:indexPath.row];
    cell.textLabel.text = [fileUrl lastPathComponent];
  }
  return cell;
}

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
    return indexPath.section > 0;
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        // Delete the row from the data source
        //[tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationFade];
      
      NSURL *url = [_fileURLs objectAtIndex:indexPath.row];
      NSFileManager *manager = [NSFileManager defaultManager];
      if ([manager isDeletableFileAtPath:url.path]) {
        [manager removeItemAtURL:url error:nil];
        [self refresh];
      }
      else {
        NSLog(@"File not Deleteable");
      }
    }
}


#pragma mark - Table view methods

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
  if (indexPath.section == 1) {
    NSURL *url = [_fileURLs objectAtIndex:indexPath.row];
    BOOL isDirectory;
    NSFileManager *manager = [NSFileManager defaultManager];
    if ([manager fileExistsAtPath:url.path isDirectory:&isDirectory] && isDirectory) {
      _workingDir = url;
      [self refresh];
    }
    else {
      [self.detailViewController showPictureAtUrl:url];
    }
  }
}

/*
// Override to support rearranging the table view.
- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath {
}
*/

/*
// Override to support conditional rearranging of the table view.
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the item to be re-orderable.
    return YES;
}
*/

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/











//- (void)refresh {
//  _iCloudURLsReady = NO;
//  [_iCloudURLs removeAllObjects];
//
//  [self initializeiCloudAccessWithCompletion:^(BOOL available) {
//    _iCloudAvailable = available;
//
//    if (_iCloudAvailable) {
//      //      NSFileManager *manager = [NSFileManager defaultManager];
//      //      NSURL *iCloudDocuments = [_iCloudRoot URLByAppendingPathComponent:@"Documents" isDirectory:YES];
//      //      if (![manager fileExistsAtPath:iCloudDocuments.path]) {
//      //        NSError *error;
//      //        [manager createDirectoryAtURL:iCloudDocuments withIntermediateDirectories:NO attributes:nil error:&error];
//      //      }
//
//      //      NSURL *readmeUrl = [iCloudDocuments URLByAppendingPathComponent:@"readme.text"];
//      //      NSURL *localReadme = [[NSURL fileURLWithPath:NSTemporaryDirectory()] URLByAppendingPathComponent:@"readme.text"];
//      //      //if (![manager fileExistsAtPath:readmeUrl.path]) {
//      //        NSString *contents = @"It Works!";
//      //        [contents writeToURL:localReadme atomically:NO encoding:NSStringEncodingConversionAllowLossy error:nil];
//      //      //}
//
//      //NSURL *localReadme = [[NSBundle mainBundle] URLForResource:@"readme" withExtension:@"txt"];
//      //      if (localReadme != nil) {
//      //        NSError *error;
//      //        [manager setUbiquitous:YES itemAtURL:localReadme destinationURL:readmeUrl error:&error];
//      //        if (error != nil) {
//      //          NSLog(@"Problem");
//      //        }
//      //      }
//
//      [self startQuery];
//    }
//  }];
//}
//
//
//#pragma mark - iCloud Methods
//
//- (void)initializeiCloudAccessWithCompletion:(void (^)(BOOL available)) completion {
//  dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
//    _iCloudRoot = [[NSFileManager defaultManager] URLForUbiquityContainerIdentifier:nil];
//    if (_iCloudRoot != nil) {
//      dispatch_async(dispatch_get_main_queue(), ^{
//        NSLog(@"iCloud available at: %@", _iCloudRoot);
//        completion(TRUE);
//      });
//    }
//    else {
//      dispatch_async(dispatch_get_main_queue(), ^{
//        NSLog(@"iCloud not available");
//        completion(FALSE);
//      });
//    }
//  });
//}
//
//- (NSMetadataQuery *)createQuery {
//  NSMetadataQuery *query = [NSMetadataQuery new];
//  if (query) {
//
//    // Search documents subdir only
//    [query setSearchScopes:[NSArray arrayWithObject:NSMetadataQueryUbiquitousDocumentsScope]];
//
//    // Add a predicate for finding the documents
//    NSString * filePattern = @"*"; //[NSString stringWithFormat:@"*.%@", PTK_EXTENSION];
//    [query setPredicate:[NSPredicate predicateWithFormat:@"%K LIKE %@",
//                         NSMetadataItemFSNameKey, filePattern]];
//  }
//  return query;
//}
//
//
//- (void)stopQuery {
//  if (_query) {
//    NSLog(@"No longer watching iCloud dir...");
//
//    [[NSNotificationCenter defaultCenter] removeObserver:self name:NSMetadataQueryDidFinishGatheringNotification object:nil];
//    [[NSNotificationCenter defaultCenter] removeObserver:self name:NSMetadataQueryDidUpdateNotification object:nil];
//    [_query stopQuery];
//    _query = nil;
//  }
//
//}
//
//- (void)startQuery {
//  [self stopQuery];
//  NSLog(@"Starting to watch iCloud dir...");
//
//  _query = [self createQuery];
//  [[NSNotificationCenter defaultCenter] addObserver:self
//                                           selector:@selector(processiCloudFiles:)
//                                               name:NSMetadataQueryDidFinishGatheringNotification
//                                             object:nil];
//  [[NSNotificationCenter defaultCenter] addObserver:self
//                                           selector:@selector(processiCloudFiles:)
//                                               name:NSMetadataQueryDidUpdateNotification
//                                             object:nil];
//
//  [_query startQuery];
//}
//
//- (void)processiCloudFiles:(NSNotification *)notification {
//  // Always disable updates while processing results
//  [_query disableUpdates];
//  [_iCloudURLs removeAllObjects];
//
//  // The query reports all files found, every time.
//  NSArray * queryResults = [_query results];
//  for (NSMetadataItem * result in queryResults) {
//    NSURL * fileURL = [result valueForAttribute:NSMetadataItemURLKey];
//    NSNumber * aBool = nil;
//
//    // Don't include hidden files
//    [fileURL getResourceValue:&aBool forKey:NSURLIsHiddenKey error:nil];
//    if (aBool && ![aBool boolValue]) {
//      [_iCloudURLs addObject:fileURL];
//    }
//  }
//  [self.tableView reloadData];
//
//  NSLog(@"Found %lu iCloud files.", (unsigned long)_iCloudURLs.count);
//  _iCloudURLsReady = YES;
//  //  if ([self iCloudOn]) {
//  //    // Remove deleted files
//  //    // Iterate backwards because we need to remove items form the array
//  //    for (int i = _objects.count -1; i >= 0; --i) {
//  //      PTKEntry * entry = [_objects objectAtIndex:i];
//  //      if (![_iCloudURLs containsObject:entry.fileURL]) {
//  //        [self removeEntryWithURL:entry.fileURL];
//  //      }
//  //    }
//  //
//  //    // Add new files
//  //    for (NSURL * fileURL in _iCloudURLs) {
//  //      [self loadDocAtURL:fileURL];
//  //    }
//  //  }
//
//  [_query enableUpdates];
//}


@end
