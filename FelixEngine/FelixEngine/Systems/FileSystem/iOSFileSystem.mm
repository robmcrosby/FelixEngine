//
//  iOSFileSystem.m
//  FelixEngine
//
//  Created by Robert Crosby on 6/14/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "iOSFileSystem.h"

#import <Foundation/Foundation.h>


using namespace fx;
using namespace std;

iOSFileSystem::iOSFileSystem() {
  FileSystem::instance = this;
}

iOSFileSystem::~iOSFileSystem() {

}

string iOSFileSystem::resourcesPath() const {
  NSBundle* mainBundle = [NSBundle mainBundle];
  return string([[[mainBundle resourceURL] absoluteString] UTF8String]);
}

string iOSFileSystem::documentsPath() const {
  NSArray *paths = [[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask];
  NSURL *url = [paths lastObject];
  return string([[url absoluteString] UTF8String]);
}
