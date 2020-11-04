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


USDCrate::USDCrate(): _fileOffset(0), _fileLength(0), _fileStream(0) {
  
}

USDCrate::~USDCrate() {
  
}

bool USDCrate::open(const string &filePath, size_t offset, size_t length) {
  _filePath = filePath;
  _fileOffset = offset;
  _fileLength = length;
  
  bool success = open();
  if (success) {
    success = readTableOfContents();
    if (!success)
      close();
  }
  return success;
}

bool USDCrate::open() const {
  _fileStream = new ifstream();
  _fileStream->open(_filePath, ios::in | ios::binary);
  if (!_fileStream->is_open()) {
    cerr << "Error Opening USDC File: " << _filePath << endl;
    delete _fileStream;
    _fileStream = nullptr;
    return false;
  }
  return true;
}

void USDCrate::close() const {
  if (_fileStream) {
    _fileStream->close();
    delete _fileStream;
  }
  _fileStream = nullptr;
}

string USDCrate::getName(const string &path) const {
  if (_pathMap.count(path))
    return _usdItems.at(_pathMap.at(path)).name;
  return "";
}

bool USDCrate::getArray(IntVector &buffer, const string &itemPath, const string &attribute) const {
  string path = itemPath + "/" + attribute;
  if (_pathMap.count(path) > 0) {
    const USDItem &item = _usdItems[_pathMap.at(path)];
    return item.getArray(buffer, this);
  }
  cerr << "Error finding path: " << path << endl;
  return false;
}

bool USDCrate::getArray(vector<vec3> &buffer, const string &itemPath, const string &attribute) const {
  string path = itemPath + "/" + attribute;
  if (_pathMap.count(path) > 0) {
    const USDItem &item = _usdItems[_pathMap.at(path)];
    return item.getArray(buffer, this);
  }
  cerr << "Error finding path: " << path << endl;
  return false;
}

bool USDCrate::appendBuffer(VertexBuffer &buffer, const string &itemPath, const string &attribute) const {
  string path = itemPath + "/" + attribute;
  if (_pathMap.count(path) > 0) {
    const USDItem &item = _usdItems[_pathMap.at(path)];
    return item.appendBuffer(buffer, this);
  }
  cerr << "Error finding path: " << path << endl;
  return false;
}

bool USDCrate::readTableOfContents() {
  istream &is = *_fileStream;
  
  // Check that a USD Crate file is being read
  is.seekg(_fileOffset);
  string fileStr = readStr(9, is);
  if (fileStr != "PXR-USDC") {
    cerr << "Error attempting to read non usdc file" << endl;
    return false;
  }
  
  // Get USDC Version
  _fileVersion = is.get();
  
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
  RepVector reps = readReps(toc["FIELDS"], is);
  IntVector fieldSets = readIntVector(toc["FIELDSETS"], is);
  SpecMap specs = readSpecMap(toc["SPECS"], is, fieldSets, fields, tokens, reps);
  PathVector paths = readPaths(toc["PATHS"], is, tokens, specs);
  
  _usdItems.resize(paths.size());
  _pathMap.clear();
  _objects.clear();
  _meshes.clear();
  _materials.clear();
  
  StringVector pathStack;
  IntVector itemStack;
  
  for (auto path = paths.begin(); path != paths.end(); ++path) {
    std::string pathStr = (pathStack.size() > 0 ? (pathStack.back() + "/" + path->token) : "");
    _pathMap[pathStr] = path->index;
    USDItem &item = _usdItems[path->index];
    item.setToPath(*path, pathStr, tokens, this);
    
    if (path->leaf) {
      USDItem &parent = _usdItems[itemStack.back()];
      parent.attributeNames.push_back(item.name);
      parent.attributes[item.name] = path->index;
      if (path->jump == -2) {
        itemStack.pop_back();
        pathStack.pop_back();
      }
    }
    else {
      if (item.typeName == "Xform")
        _objects.push_back(item.pathString);
      else if (item.typeName == "Mesh")
        _meshes.push_back(item.pathString);
      else if (item.typeName == "Material")
        _materials.push_back(item.pathString);
      
      if (itemStack.size() > 0) {
        USDItem &parent = _usdItems[itemStack.back()];
        parent.children.push_back(path->index);
      }
      else {
        _rootItem = path->index;
      }
      itemStack.push_back(path->index);
      pathStack.push_back(pathStr);
    }
  }
  //printUSD();
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

RepVector USDCrate::readReps(long start, istream &is) {
  is.seekg(_fileOffset + start + 8);
  long compressedSize = readLongL(is);
  
  is.seekg(_fileOffset + start + 16 + compressedSize);
  compressedSize = readLongL(is);

  FileData buffer;
  CompressionUtils::decompressLZ4(buffer, is, compressedSize);
  
  LongVector values;
  CompressionUtils::decodeLongsL(values, buffer);
  
  RepVector reps(values.size());
  for (int i = 0; i < values.size(); ++i) {
    reps[i].type = (USD_TYPE)(values[i] >> 48 & 0xff);
    reps[i].array = values[i] >> 63 & 1;
    reps[i].inlined = values[i] >> 62 & 1;
    reps[i].compressed = values[i] >> 61 & 1;
    reps[i].payload = values[i] & 0xffffffffffff;
  }
  
  return reps;
}

SpecMap USDCrate::readSpecMap(long start, istream &is, IntVector &fieldSets, IntVector &fields, StringVector &tokens, RepVector &reps) {
  is.seekg(_fileOffset + start);
  long numSpecs = readLongL(is);
  IntVector paths = decompressIntVector(numSpecs, is);
  IntVector fsets = decompressIntVector(numSpecs, is);
  IntVector types = decompressIntVector(numSpecs, is);
  
  SpecMap specs;
  for (int i = 0; i < numSpecs; ++i) {
    specs[paths[i]].type = types[i];
    int fIndex = fsets[i];
    while (fIndex < fieldSets.size() && fieldSets[fIndex] >= 0) {
      int f = fieldSets[fIndex++];
      string name = tokens[fields[f]];
      specs[paths[i]].reps[name] = reps[f];
    }
  }
  return specs;
}

PathVector USDCrate::readPaths(long start, istream &is, StringVector &tokens, SpecMap &specs) {
  is.seekg(_fileOffset + start + 8);
  long numPaths = readLongL(is);
  IntVector pathInts = decompressIntVector(numPaths, is);
  IntVector tokenInts = decompressIntVector(numPaths, is);
  IntVector jumpInts = decompressIntVector(numPaths, is);
  
  PathVector paths(numPaths);
  for (int i = 0; i < numPaths; ++i) {
    paths[i].index = pathInts[i];
    paths[i].jump = jumpInts[i];
    paths[i].leaf = tokenInts[i] < 0;
    paths[i].spec = specs[pathInts[i]];
    paths[i].token = tokens[abs(tokenInts[i])];
  }
  return paths;
}

void USDCrate::printUSD() const {
  const USDItem &rootItem = _usdItems.at(_rootItem);
  rootItem.print(cout, this);
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
