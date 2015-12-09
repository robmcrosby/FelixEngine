//
//  FileSystem.mm
//  FelixEngine
//
//  Created by Robert Crosby on 12/1/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "FileSystem.h"
#include "ImageLoader.h"
#include "MeshLoader.h"

#include <fstream>
#include <SDL2/SDL.h>

#import <Foundation/Foundation.h>


using namespace fx;
using namespace std;


FileSystem* FileSystem::sInstance = nullptr;

string FileSystem::GetResourcesPath()
{
  NSBundle* mainBundle = [NSBundle mainBundle];
  if (mainBundle != nil)
  {
    NSString* path = [mainBundle resourcePath];
    return [path UTF8String] + std::string("/");
  }
  return "";
}

File FileSystem::GetResources()
{
  NSBundle* mainBundle = [NSBundle mainBundle];
  return string([[[mainBundle resourceURL] absoluteString] UTF8String]);
}

File FileSystem::GetDocuments()
{
  if (sInstance)
    return sInstance->getDocuments();
  
  NSArray *paths = [[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask];
  NSURL *url = [paths lastObject];
  return string([[url absoluteString] UTF8String]);
}

bool FileSystem::LoadText(string &buffer, const string &filePath)
{
  bool success = false;
  fstream fileIn;
  
  fileIn.open(filePath.c_str(), ios::in);
  if (fileIn.is_open())
  {
    buffer = string((istreambuf_iterator<char>(fileIn)), istreambuf_iterator<char>());
    fileIn.close();
    success = true;
  }
  else
    cerr << "Error Reading Text File: " << filePath << endl;
  return success;
}

string FileSystem::LoadText(const string &filePath)
{
  string contents;
  LoadText(contents, filePath);
  return contents;
}

long FileSystem::Read(unsigned int *buffer, long size, std::istream &is)
{
  is.read((char*)buffer, size*sizeof(unsigned int));
  
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
  for (long i = 0; i < size; ++i, ++buffer)
    *buffer = SDL_Swap32(*buffer);
#endif
  
  return is.gcount();
}


File::File(const string &url): mUrl(url), mType(FILE_OTHER)
{
  setType();
}

void File::setType()
{
  if (mUrl != "")
  {
    NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:mUrl.c_str()]];
    NSNumber *isDirectory;
    
    [url getResourceValue:&isDirectory forKey:NSURLIsDirectoryKey error:nil];
    if ([isDirectory boolValue])
      mType = FILE_DIR;
    else
      mType = TypeFromExtension([[url pathExtension] UTF8String]);
  }
}

string File::name() const
{
  return [[[NSURL URLWithString:[NSString stringWithUTF8String:mUrl.c_str()]] lastPathComponent] UTF8String];
}

string File::path() const
{
  return [[[NSURL URLWithString:[NSString stringWithUTF8String:mUrl.c_str()]] path] UTF8String];
}

string File::extension() const
{
  return [[[NSURL URLWithString:[NSString stringWithUTF8String:mUrl.c_str()]] pathExtension] UTF8String];
}

bool File::exists() const
{
  NSFileManager *manager = [NSFileManager defaultManager];
  NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:mUrl.c_str()]];
  return [manager fileExistsAtPath:[url path]];
}

bool File::load(Directory &dir) const
{
  NSError *error = nil;
  NSFileManager *manager = [NSFileManager defaultManager];
  NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:mUrl.c_str()]];
  NSArray *contents = [manager contentsOfDirectoryAtURL:url
                             includingPropertiesForKeys:@[]
                                                options:NSDirectoryEnumerationSkipsHiddenFiles
                                                  error:&error];
  if (contents == nil)
  {
    NSLog(@"%@", error);
    return false;
  }
  dir.clear();
  for (NSURL *fileUrl in contents)
    dir.push_back(File([[fileUrl absoluteString] UTF8String]));
  return true;
}


bool File::createDirectory() const
{
  NSError *error = nil;
  NSFileManager *manager = [NSFileManager defaultManager];
  NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:mUrl.c_str()]];
  
  if (![manager createDirectoryAtURL:url withIntermediateDirectories:YES attributes:nil error:&error])
  {
    NSLog(@"%@", error);
    return false;
  }
  return true;
}

bool File::write(const std::string &text) const
{
  NSError *error = nil;
  NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:mUrl.c_str()]];
  NSString *contents = [NSString stringWithUTF8String:text.c_str()];
  
  if (![contents writeToURL:url atomically:YES encoding:NSUTF8StringEncoding error:&error])
  {
    NSLog(@"%@", error);
    return false;
  }
  return true;
}

bool File::append(const std::string &text) const
{
  NSError *error = nil;
  NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:mUrl.c_str()]];
  NSString *contents = [NSString stringWithUTF8String:text.c_str()];
  
  NSFileHandle *fileHandle = [NSFileHandle fileHandleForWritingToURL:url error:&error];
  if (fileHandle != nil)
  {
    [fileHandle seekToEndOfFile];
    [fileHandle writeData:[contents dataUsingEncoding:NSUTF8StringEncoding]];
    [fileHandle closeFile];
    return true;
  }
  else if (![contents writeToURL:url atomically:YES encoding:NSUTF8StringEncoding error:&error])
  {
    NSLog(@"%@", error);
    return false;
  }
  return true;
}

bool File::remove() const
{
  NSError *error = nil;
  NSFileManager *manager = [NSFileManager defaultManager];
  NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:mUrl.c_str()]];
  
  if (![manager removeItemAtURL:url error:&error])
  {
    NSLog(@"%@", error);
    return false;
  }
  return true;
}

bool File::addDirectory(const std::string &name) const
{
  NSError *error = nil;
  NSFileManager *manager = [NSFileManager defaultManager];
  NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:mUrl.c_str()]];
  url = [url URLByAppendingPathComponent:[NSString stringWithUTF8String:name.c_str()]];
  
  if (![manager createDirectoryAtURL:url withIntermediateDirectories:YES attributes:nil error:&error])
  {
    NSLog(@"%@", error);
    return false;
  }
  return true;
}

bool File::load(std::string &text) const
{
  NSError *error = nil;
  NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:mUrl.c_str()]];
  NSString *contents = [NSString stringWithContentsOfURL:url
                                                encoding:NSUTF8StringEncoding
                                                   error:&error];
  if (contents == nil)
  {
    NSLog(@"%@", error);
    return false;
  }
  text = [contents UTF8String];
  return true;
}

bool File::load(fx::XMLTree &tree) const
{
  return tree.loadFile(path());
}

bool File::load(ImageRGBA &image) const
{
  return ImageLoader::LoadImageFromFile(image, path());
}

bool File::load(VertexBufferMap &bufferMap) const
{
  return MeshLoader::LoadMeshFromFile(bufferMap, path());
}

File File::operator+(const std::string &path) const
{
  NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:mUrl.c_str()]];
  url = [url URLByAppendingPathComponent:[NSString stringWithUTF8String:path.c_str()]];
  return string([[url absoluteString] UTF8String]);
}

string File::TypeString(FILE_TYPE type)
{
  switch (type) {
    case FILE_DIR:
      return "Dir";
    case FILE_TEXT:
      return "Text";
    case FILE_XML:
      return "Xml";
    case FILE_MESH:
      return "Mesh";
    case FILE_SHADER:
      return "Shader";
    case FILE_IMAGE:
      return "Image";
    case FILE_OTHER:
      return "Other";
  }
  return "Invalid";
}

FILE_TYPE File::TypeFromExtension(const std::string &ext)
{
  if (ext == "png" || ext == "jpg")
    return FILE_IMAGE;
  if (ext == ".vert" || ext == ".frag" || ext == "metallib")
    return FILE_SHADER;
  if (ext == "xml")
    return FILE_XML;
  if (ext == "mesh")
    return FILE_MESH;
  if (ext == "txt")
    return FILE_TEXT;
  return FILE_OTHER;
}
