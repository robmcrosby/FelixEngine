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
  map<string, long> toc;
  for (; numSections > 0; --numSections) {
    string sectionName = readStr(16, is);
    toc[sectionName] = readLongL(is);
    readLongL(is);
  }
  
  StringVector tokens = readTokens(toc["TOKENS"], is);
  IntVector fields = readIntVector(toc["FIELDS"], is);
  LongVector reps = readReps(toc["FIELDS"], is);
  IntVector fieldSets = readIntVector(toc["FIELDSETS"], is);
  PathVector paths = readPaths(toc["PATHS"], is, tokens);
  SpecMap specs = readSpecMap(toc["SPECS"], is);
  
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

LongVector USDCrate::readReps(long start, istream &is) {
  is.seekg(_fileOffset + start + 8);
  long compressedSize = readLongL(is);
  
  is.seekg(_fileOffset + start + 16 + compressedSize);
  compressedSize = readLongL(is);

  FileData buffer;
  CompressionUtils::decompressLZ4(buffer, is, compressedSize);
  
  LongVector reps;
  CompressionUtils::decodeLongsL(reps, buffer);
  
  return reps;
}

PathVector USDCrate::readPaths(long start, istream &is, StringVector &tokens) {
  is.seekg(_fileOffset + start + 8);
  long numPaths = readLongL(is);
  IntVector pathInts = decompressIntVector(numPaths, is);
  IntVector tokenInts = decompressIntVector(numPaths, is);
  IntVector jumpInts = decompressIntVector(numPaths, is);
  
  PathVector paths(numPaths);
  for (int i = 0; i < numPaths; ++i) {
    paths[i].path = pathInts[i];
    paths[i].jump = jumpInts[i];
    paths[i].leaf = tokenInts[i] < 0;
    paths[i].token = tokens[abs(tokenInts[i])];
  }
  return paths;
}

SpecMap USDCrate::readSpecMap(long start, istream &is) {
  is.seekg(_fileOffset + start);
  long numSpecs = readLongL(is);
  IntVector paths = decompressIntVector(numSpecs, is);
  IntVector fsets = decompressIntVector(numSpecs, is);
  IntVector types = decompressIntVector(numSpecs, is);
  
  SpecMap specs;
  for (int i = 0; i < numSpecs; ++i) {
    specs[paths[i]].fset = fsets[i];
    specs[paths[i]].type = types[i];
  }
  return specs;
}

IntVector USDCrate::readIntVector(long start, istream &is) {
  is.seekg(_fileOffset + start);
  long numItems = readLongL(is);
  return decompressIntVector(numItems, is);
}

IntVector USDCrate::decompressIntVector(long numItems, istream &is) {
  long compressedSize = readLongL(is);

  FileData buffer;
  CompressionUtils::decompressLZ4(buffer, is, compressedSize);
  
  IntVector items;
  CompressionUtils::decompressUSD32(items, buffer, numItems);
  
  return items;
}
