//
//  iOSFileSystem.m
//  FelixEngine
//
//  Created by Robert Crosby on 6/14/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "iOSFileSystem.h"
#include "MeshLoader.h"
#include "USDArchive.h"
#include "XMLTree.h"

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>


using namespace fx;
using namespace std;

iOSFileSystem::iOSFileSystem() {
  FileSystem::instance = this;
}

iOSFileSystem::~iOSFileSystem() {

}

string iOSFileSystem::resourcesPath() const {
  NSBundle* mainBundle = [NSBundle mainBundle];
  return string([[mainBundle resourcePath] UTF8String]) + "/";
}

string iOSFileSystem::documentsPath() const {
  NSArray *paths = [[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask];
  NSURL *url = [paths lastObject];
  return [[url absoluteString] UTF8String];
}

bool iOSFileSystem::loadFileData(fx::FileData &data, const std::string &fileName) const {
  string filePath = findPathForFile(fileName);
  
  // Attempt to open the file
  ifstream file(filePath, ios::ate | ios::binary);
  if (!file.is_open()) {
    cerr << "Error Opening file: " << fileName << endl;
    return false;
  }
  
  // Get the file size and create the buffer
  size_t fileSize = (size_t)file.tellg();
  data.resize(fileSize);
  
  // Read to the buffer
  file.seekg(0);
  file.read(data.data(), fileSize);
  
  // Close the file
  file.close();
  return true;
}

bool iOSFileSystem::loadMeshFile(VertexMeshData &mesh, const string &file) const {
  string fileType = getFilePostfix(file);
  string filePath = findPathForFile(file);
  
  if (fileType == "mesh") {
    return MeshLoader::loadFromBinaryFile(mesh, filePath);
  }
  else if (fileType == "usdz") {
    USDArchive archive;
    if (archive.open(filePath)) {
      USDCrate crate = archive.getFirstUSDCrate();
      return MeshLoader::loadFromCrateFile(mesh, crate);
    }
    else {
      cerr << "Unable to open USD Archive" << endl;
    }
    //return MeshLoader::loadFromUsdzFile(mesh, filePath);
  }
  else {
    cerr << "Unknown Mesh File: " << file << endl;
  }
  return false;
}

bool iOSFileSystem::loadImageData(ImageBufferData &image, const string &file) const {
  string filePath = findPathForFile(file);
  
  UIImage *uiImage = [UIImage imageWithContentsOfFile:[NSString stringWithUTF8String:filePath.c_str()]];
  if (uiImage != nil) {
    CGImageRef imageRef = [uiImage CGImage];
    NSUInteger width = CGImageGetWidth(imageRef);
    NSUInteger height = CGImageGetHeight(imageRef);
    NSUInteger bytesPerPixel = 4;
    NSUInteger bytesPerRow = bytesPerPixel * width;
    NSUInteger bitsPerComponent = 8;
    
    // Resize the Image Buffer
    image.resize((int)width, (int)height);
    uint8_t *rawData = (uint8_t*)image.ptr();
    
    // Create a Core Graphics Context
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate(rawData, width, height,
                                                 bitsPerComponent, bytesPerRow, colorSpace,
                                                 kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
    CGColorSpaceRelease(colorSpace);
    
    // Flip the image
    //CGContextTranslateCTM(context, 0, height);
    //CGContextScaleCTM(context, 1, -1);
    
    // Draw the image to the Image Buffer
    CGContextDrawImage(context, CGRectMake(0, 0, width, height), imageRef);
    CGContextRelease(context);
    
    return true;
  }
  else {
    cerr << "Error Loading Image File: " << file << endl;
  }
  return false;
}

bool iOSFileSystem::loadXMLTreeFile(XMLTree &tree, const std::string &file) const {
  string filePath = findPathForFile(file);
  return tree.loadFile(filePath);
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
