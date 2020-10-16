//
//  USDCrate.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/11/20.
//  Copyright © 2020 Robert Crosby. All rights reserved.
//

#include "USDCrate.h"
#include "CompressionUtils.h"


using namespace std;
using namespace fx;


USDCrate::USDCrate(): _fileOffset(0), _fileLength(0) {
  
}

USDCrate::~USDCrate() {
  
}

bool USDCrate::open(const string &filePath, size_t offset, size_t length) {
  _filePath = filePath;
  _fileOffset = offset;
  _fileLength = length;
  
  bool success = true;
  ifstream usdcFile;
  
  usdcFile.open(filePath, ios::in | ios::binary);
  if (!usdcFile.is_open())
  {
    cerr << "Error Opening USDC File" << endl;
    success = false;
  }
  else {
    success = read(usdcFile);
    usdcFile.close();
  }
  return success;
}

bool USDCrate::read(istream &is) {
  // Check that a USD Crate file is being read
  is.seekg(_fileOffset);
  string fileStr = readStr(9, is);
  if (fileStr != "PXR-USDC") {
    cerr << "Error attempting to read non usdc file" << endl;
    return false;
  }
  
  // Get Table of Contents Offset
  is.seekg(_fileOffset + 16);
  long tocOffset;
  readL(tocOffset, is);
  
  // Get number of Sections in Table of Contents
  is.seekg(_fileOffset + tocOffset);
  long numSections;
  readL(numSections, is);
  
  // Get the Section Names and Offsets
  USDTableOfContents toc;
  for (; numSections > 0; --numSections) {
    string sectionName = readStr(16, is);
    readL(toc[sectionName].start, is);
    readL(toc[sectionName].size, is);
  }
  
  // Read Tokens
  StringVector tokens = readTokens(toc["TOKENS"].start, is);
  vector<int> fields = readFields(toc["FIELDS"].start, is);
  
  return true;
}

StringVector USDCrate::readTokens(long start, istream &is) {
  // Get Number of Tokens and Compressed Size
  is.seekg(_fileOffset + start);
  long numTokens = readLongL(is);
  long compressedSize = readLongL(is);
  
  // Read and Decompress Data
  FileData buffer;
  CompressionUtils::decompressLZ4(buffer, is, compressedSize);
  
  // Get the Tokens
  StringVector tokens;
  char *ptr = &buffer[0];
  while (numTokens-- > 0) {
    tokens.push_back(ptr);
    ptr += tokens.back().length() + 1;
  }
  return tokens;
}

IntVector USDCrate::readFields(long start, istream &is) {
  // Get Number of Fields
  is.seekg(_fileOffset + start);
  long numFields = readLongL(is);
  long compressedSize = readLongL(is);
  
  FileData buffer;
  CompressionUtils::decompressLZ4(buffer, is, compressedSize);
  
  IntVector fields;
  CompressionUtils::decompressUSD32(fields, buffer, numFields);
  return fields;
}
