//
//  iOSFileSystem.m
//  FelixEngine
//
//  Created by Robert Crosby on 6/14/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "iOSFileSystem.h"
#include "MeshLoader.h"

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
  return [[[mainBundle resourceURL] path] UTF8String];
}

string iOSFileSystem::documentsPath() const {
  NSArray *paths = [[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask];
  NSURL *url = [paths lastObject];
  return [[url absoluteString] UTF8String];
}

bool iOSFileSystem::loadMeshFile(VertexMeshData &mesh, const std::string &file) const {
  string fileType = getFilePostfix(file);
  string filePath = findPathForFile(file);
  
  if (fileType == "mesh") {
    return MeshLoader::loadFromBinaryFile(mesh, filePath);
  }
  
  cerr << "Unknown Mesh File: " << file << endl;
  return false;
}

bool iOSFileSystem::fileExistsAtPath(const std::string &filePath) const {
  NSString *path = [NSString stringWithUTF8String:filePath.c_str()];
  return [[NSFileManager defaultManager] fileExistsAtPath:path];
}

string iOSFileSystem::findPathForFile(const string &file) const {
  // Check if file is already valid
  if (fileExistsAtPath(file)) {
    return file;
  }
  
  // Check the Documents
  string filePath = documentsPath() + file;
  if (fileExistsAtPath(filePath)) {
    return filePath;
  }
  
  // Check the Resources
  filePath = resourcesPath() + file;
  if (fileExistsAtPath(filePath)) {
    return filePath;
  }
  
  NSString *fileName = [NSString stringWithUTF8String:getFileName(file).c_str()];
  NSString *fileType = [NSString stringWithUTF8String:getFilePostfix(file).c_str()];
  
  NSBundle *mainBundle = [NSBundle mainBundle];
  NSString *path = [mainBundle pathForResource:fileName ofType:fileType];
  if (path != nil) {
    return [path UTF8String];
  }
  
  cerr << "Error Finding File: " << file << endl;
  return file;
}
