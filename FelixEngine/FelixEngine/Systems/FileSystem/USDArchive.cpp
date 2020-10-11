//
//  USDArchive.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/20.
//  Copyright © 2020 Robert Crosby. All rights reserved.
//

#include "USDArchive.h"
#include <fstream>


using namespace fx;
using namespace std;

#define CDR_END_SIGNATURE 0x504b0506
#define CDR_ENTRY_SIGNATURE 0x504b0102



USDArchive::USDArchive() {
  
}

USDArchive::~USDArchive() {
  
}

bool USDArchive::open(const string &filePath) {
  bool success = true;
  ifstream usdzFile;
  
  usdzFile.open(filePath, ios::in | ios::binary);
  if (!usdzFile.is_open())
  {
    cerr << "Error Opening USDArchive" << endl;
    success = false;
  }
  else {
    _filePath = filePath;
    success = read(usdzFile);
    usdzFile.close();
  }
  return success;
}

FileList USDArchive::allFiles() const {
  FileList files;
  for (auto itr = _entries.begin(); itr != _entries.end(); ++itr) {
    files.push_back(itr->name);
  }
  return files;
}

FileList USDArchive::sceneFiles() const {
  FileList files;
  for (auto itr = _entries.begin(); itr != _entries.end(); ++itr) {
    string ext = itr->name.substr(itr->name.length()-4);
    if (ext == "usdc")
      files.push_back(itr->name);
  }
  return files;
}

FileList USDArchive::imageFiles() const {
  FileList files;
  for (auto itr = _entries.begin(); itr != _entries.end(); ++itr) {
    string ext = itr->name.substr(itr->name.length()-3);
    if (ext == "png")
      files.push_back(itr->name);
  }
  return files;
}

bool USDArchive::read(istream &is) {
  _entries.clear();
  
  // Find End of Central Directory Record
  is.seekg(-22, is.end);
  if (!findPrevSignature(is, CDR_END_SIGNATURE)) {
    cerr << "Error Finding End of Central Directory Record of Zip File" << endl;
    return false;
  }
  
  // Read the Number of Entries
  int numEntries;
  if (readL(numEntries, is) != sizeof(int) || readL(numEntries, is) != sizeof(int)) {
    cerr << "Error Getting Number of Entries of Zip File" << endl;
    return false;
  }
  numEntries &= 0xffff;
  _entries.resize(numEntries);
  //cout << "numEntries: " << numEntries << endl;
  
  // Read the Centeral Directory Length and Offset
  int cdLength, cdOffset;
  if (readL(cdLength, is) != sizeof(int) || readL(cdOffset, is) != sizeof(int)) {
    cerr << "Error Finding Central Directory Record of Zip File" << endl;
    return false;
  }
  is.seekg(cdOffset);

  for (int i = 0; i < numEntries; ++i) {
    // Find the Start of the Entry
    if (!findNextSignature(is, CDR_ENTRY_SIGNATURE)) {
      cerr << "Error Reading USD Archive Entry" << endl;
      return false;
    }
    size_t entryOffset = is.tellg();
    
    int headerOffset;
    is.seekg(entryOffset + 38);
    readL(headerOffset, is);
    _entries[i].offset = headerOffset;
    //cout << "headerOffset: " << _entries[i].offset << endl;
    
    int strLength;
    is.seekg(entryOffset + 24);
    readL(strLength, is);
    strLength &= 0xffff;
    
    is.seekg(entryOffset + 42);
    _entries[i].name = readStr(strLength, is);
    //cout << "name: " << _entries[i].name << endl;
  }
  
  return true;
}

bool USDArchive::findNextSignature(std::istream &is, int signature) {
  size_t offset = is.tellg();
  int value;
  
  do {
    is.seekg(offset++);
    if (readB(value, is) != sizeof(int))
      return false;
  } while (value != signature);
  
  return true;
}

bool USDArchive::findPrevSignature(std::istream &is, int signature) {
  size_t offset = is.tellg();
  int value;
  
  do {
    is.seekg(offset--);
    if (offset == 0 || readB(value, is) != sizeof(int))
      return false;
  } while (value != signature);
  
  return true;
}
