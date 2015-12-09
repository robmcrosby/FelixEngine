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

namespace fx
{
  struct IOSFileSystemInfo
  {
    IOSFileSystemInfo()
    {
      mUbiquityContainerUrl = nil;
    }
    ~IOSFileSystemInfo()
    {
      mUbiquityContainerUrl = nil;
    }
    
    NSURL *mUbiquityContainerUrl;
  };
}

using namespace fx;
using namespace std;


IOSFileSystem::IOSFileSystem()
{
  mInfo = new IOSFileSystemInfo();
  sInstance = this;
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



#else

using namespace fx;
using namespace std;

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

#endif
