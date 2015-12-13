//
//  IOSFileSystem.mm
//  FelixEngine
//
//  Created by Robert Crosby on 12/6/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "IOSFileSystem.h"
#include "TaskGroup.h"
#import <Foundation/Foundation.h>

#if __IPHONEOS__



@interface UbiquityListener : NSObject

-(void)queryDidFinishGatheringNotification:(NSNotification*)notification;
-(void)queryDidReceiveNotification:(NSNotification *)notification;
//-(void)printQuery;
-(void)updateFiles;

@end


@implementation UbiquityListener
{
  NSMetadataQuery *_query;
}

-(id)init
{
  self = [super init];
  if (self != nil)
  {
    _query = [[NSMetadataQuery alloc]init];
    
    _query.searchScopes = @[NSMetadataQueryUbiquitousDocumentsScope];
    _query.predicate = [NSPredicate predicateWithFormat:@"%K LIKE '*'", NSMetadataItemFSNameKey];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(queryDidFinishGatheringNotification:)
                                                 name:NSMetadataQueryDidFinishGatheringNotification
                                               object:_query];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(queryDidReceiveNotification:)
                                                 name:NSMetadataQueryDidUpdateNotification
                                               object:_query];
    if (![_query startQuery])
      NSLog(@"Query didn't start");
  }
  return self;
}

-(void)queryDidFinishGatheringNotification:(NSNotification*)notification
{
  [_query disableUpdates];
  //[self printQuery];
  [self updateFiles];
  fx::IOSFileSystem *fileSys = fx::IOSFileSystem::Instance();
  if (fileSys)
    fileSys->handleUpdate();
  [_query enableUpdates];
}

-(void)queryDidReceiveNotification:(NSNotification*)notification
{
  [_query disableUpdates];
  //[self printQuery];
  [self updateFiles];
  fx::IOSFileSystem *fileSys = fx::IOSFileSystem::Instance();
  if (fileSys)
    fileSys->handleUpdate();
  [_query enableUpdates];
}

//-(void)printQuery
//{
//  NSArray *results = [_query results];
//  for(NSMetadataItem *item in results)
//  {
//    NSString *filename = [item valueForAttribute:NSMetadataItemDisplayNameKey];
//    //NSString *url = [item valueForAttribute:NSMetadataItemURLKey];
//    NSNumber *filesize = [item valueForAttribute:NSMetadataItemFSSizeKey];
//    NSDate *updated = [item valueForAttribute:NSMetadataItemFSContentChangeDateKey];
//    NSString *status = [item valueForAttribute:NSMetadataUbiquitousItemDownloadingStatusKey];
//    NSLog(@"%@ (%@ bytes, status %@)", filename, filesize, status);
//  }
//}

-(void)updateFiles
{
  NSFileManager *manager = [NSFileManager defaultManager];
  NSArray *results = [_query results];
  for(NSMetadataItem *item in results)
  {
    NSURL *url = [item valueForAttribute:NSMetadataItemURLKey];
    NSString *status = [item valueForAttribute:NSMetadataUbiquitousItemDownloadingStatusKey];
    if (url != nil && status != nil && [status isEqualToString:NSMetadataUbiquitousItemDownloadingStatusNotDownloaded])
      [manager startDownloadingUbiquitousItemAtURL:url error:nil];

  }
}

@end




namespace fx
{
  struct IOSFileSystemInfo
  {
    IOSFileSystemInfo()
    {
      mUbiquityContainerUrl = nil;
      mUbiquityListener = nil;
    }
    ~IOSFileSystemInfo()
    {
      mUbiquityContainerUrl = nil;
      mUbiquityListener = nil;
    }
    
    NSURL *mUbiquityContainerUrl;
    UbiquityListener *mUbiquityListener;
  };
}

using namespace fx;
using namespace std;

IOSFileSystem* IOSFileSystem::sInstance = nullptr;


IOSFileSystem::IOSFileSystem()
{
  mInfo = new IOSFileSystemInfo();
  FileSystem::sInstance = this;
  IOSFileSystem::sInstance = this;
}

IOSFileSystem::~IOSFileSystem()
{
  delete mInfo;
}

bool IOSFileSystem::setToXml(const XMLTree::Node *node)
{
  return true;
}

bool IOSFileSystem::init()
{
  NSFileManager *manager = [NSFileManager defaultManager];
  id icloudToken = manager.ubiquityIdentityToken;
  NSString *key = @"com.robmcrosby.ModelViewer.UbiquityIdentityToken";
  
  if (icloudToken)
  {
    NSData *newTokenData = [NSKeyedArchiver archivedDataWithRootObject: icloudToken];
    [[NSUserDefaults standardUserDefaults] setObject:newTokenData
                                              forKey:key];
  }
  else
    [[NSUserDefaults standardUserDefaults] removeObjectForKey:key];
  
  Delegate<void, void*> delegate = Delegate<void, void*>::Create<IOSFileSystem, &IOSFileSystem::initUbiquity>(this);
  TaskingSystem::Instance()->dispatch(delegate);
  
  mInfo->mUbiquityListener = [[UbiquityListener alloc] init];
  return true;
}

File IOSFileSystem::getDocuments()
{
  // Attempt to get the Documents directory in the Ubiquity Container.
  mCondition.wait();
  if (mInfo->mUbiquityContainerUrl != nil)
    return getUbiquity() + "Documents";
  
  // If Ubiquity is not avalible, get the default Documents Directory.
  NSArray *paths = [[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask];
  NSURL *url = [paths lastObject];
  return string([[url absoluteString] UTF8String]);
}

void IOSFileSystem::initUbiquity(void*)
{
  mInfo->mUbiquityContainerUrl = [[NSFileManager defaultManager] URLForUbiquityContainerIdentifier:nil];
  if (mInfo->mUbiquityContainerUrl == nil)
    cerr << "Failed to get Ubiquity Container" << endl;
  mCondition.broadcast(false);
}

File IOSFileSystem::getUbiquity()
{
  return string([[mInfo->mUbiquityContainerUrl absoluteString] UTF8String]);
}

void IOSFileSystem::handleUpdate()
{
}

#else

using namespace fx;
using namespace std;

IOSFileSystem* IOSFileSystem::sInstance = nullptr;

IOSFileSystem::IOSFileSystem()
{
}

IOSFileSystem::~IOSFileSystem()
{
}

bool IOSFileSystem::setToXml(const XMLTree::Node *node)
{
  return false;
}

bool IOSFileSystem::init()
{
  return false;
}

File IOSFileSystem::getDocuments()
{
  NSArray *paths = [[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask];
  NSURL *url = [paths lastObject];
  return string([[url absoluteString] UTF8String]);
}

void IOSFileSystem::initUbiquity(void*)
{
}

void IOSFileSystem::handleUpdate()
{
}

#endif
