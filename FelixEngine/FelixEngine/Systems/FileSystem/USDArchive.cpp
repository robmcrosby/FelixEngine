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
  _filePath = filePath;
  
  bool success = true;
  ifstream usdzFile;
  
  usdzFile.open(filePath, ios::in | ios::binary);
  if (!usdzFile.is_open())
  {
    cerr << "Error Opening USDArchive" << endl;
    success = false;
  }
  else {
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

FileList USDArchive::crateFiles() const {
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

USDEntry USDArchive::getEntry(const string &name) const {
  for (auto itr = _entries.begin(); itr != _entries.end(); ++itr) {
    if (itr->name == name)
      return *itr;
  }
  return USDEntry();
}

USDCrate USDArchive::getUSDCrate(const string &name) const {
  USDCrate crate;
  USDEntry entry = getEntry(name);
  crate.open(_filePath, entry.fileOffset, entry.fileLength);
  return crate;
}

USDCrate USDArchive::getFirstUSDCrate() const {
  FileList files = crateFiles();
  if (files.size() > 0)
    return getUSDCrate(files.front());
  cerr << "No USD Files in USD Archive" << endl;
  return USDCrate();
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
    
    // Get Header Offset
    is.seekg(entryOffset + 38);
    int headerOffset = readIntL(is);
    _entries[i].headerOffset = headerOffset;
    
    // Get File Length
    is.seekg(headerOffset + 18);
    _entries[i].fileLength = readIntL(is);
    
    // Get File Offset
    is.seekg(headerOffset + 26);
    int fileOffset = readIntL(is);
    fileOffset = (fileOffset & 0xffff) + (fileOffset >> 16);
    _entries[i].fileOffset = headerOffset + fileOffset + 30;
    
    // Get File Name Length
    is.seekg(entryOffset + 24);
    int strLength = readShortL(is);
    
    // Get the File Name
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
