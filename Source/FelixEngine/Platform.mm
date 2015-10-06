//
//  Platform.m
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "Platform.h"
#include <string>

#import <Foundation/Foundation.h>


using namespace std;
using namespace fx;


/**
 *
 */
string Platform::GetResourcePath()
{
  NSBundle* mainBundle = [NSBundle mainBundle];
  
  if (mainBundle != nil)
  {
    NSString* path = [mainBundle resourcePath];
    return [path UTF8String] + std::string("/");
  }
  
  return "";
}
