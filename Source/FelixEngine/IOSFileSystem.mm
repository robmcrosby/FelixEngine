//
//  IOSFileSystem.mm
//  FelixEngine
//
//  Created by Robert Crosby on 12/6/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "IOSFileSystem.h"
#import <Foundation/Foundation.h>


using namespace fx;
using namespace std;


#if __IPHONEOS__

IOSFileSystem::IOSFileSystem()
{
}

IOSFileSystem::~IOSFileSystem()
{
}

bool IOSFileSystem::setToXml(const XMLTree::Node *node)
{
  return true;
}

bool IOSFileSystem::init()
{
  NSFileManager *manager = [NSFileManager defaultManager];
  id icloudToken = manager.ubiquityIdentityToken;
  
  if (icloudToken)
    cout << "Got icloud token" << endl;
  else
    cout << "icloud token is nil" << endl;
  return true;
}



#else

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

#endif
