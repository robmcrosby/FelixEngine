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

USDCrate::USDCrate(const USDCrate &crate): _fileOffset(0), _fileLength(0), _fileStream(0) {
  *this = crate;
}

USDCrate::~USDCrate() {
  
}

USDCrate& USDCrate::operator=(const USDCrate &crate) {
  _filePath    = crate._filePath;
  _fileOffset  = crate._fileOffset;
  _fileLength  = crate._fileLength;
  _fileVersion = crate._fileVersion;
  _rootItem    = crate._rootItem;
  
  _usdItems = crate._usdItems;
  for (auto &item : _usdItems)
    item.crate = this;
  
  _pathMap   = crate._pathMap;
  _objects   = crate._objects;
  _meshes    = crate._meshes;
  _materials = crate._materials;
  
  return *this;
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
  if (hasItem(path))
    return getItem(path).name;
  return "";
}

string USDCrate::getStringValue(const string &path) const {
  if (hasItem(path))
    return getItem(path).stringValue();
  return "";
}

string USDCrate::getParentPath(const string &path) const {
  size_t pos = path.find_last_of("/");
  return pos != string::npos ? path.substr(0, pos) : "";
}

bool USDCrate::isTextureInput(const string &itemPath, const string &name) const {
  string path = getInputPath(itemPath, name);
  if (path != "")
    return getItem(path).dataType == USD_PATH_VECTOR;
  return false;
}

string USDCrate::getAttributePath(const string &itemPath, const string &attribute) const {
  string path = itemPath + "/" + attribute;
  return hasItem(path) ? path : "";
}

string USDCrate::getInputPath(const string &itemPath, const string &name) const {
  return getAttributePath(itemPath, "inputs:" + name);
}

string USDCrate::getTexturePath(const string &itemPath, const string &name) const {
  string path = getInputPath(itemPath, name);
  if (path != "") {
    int index = getItem(path).pathValue();
    if (index >= 0 && index < _usdItems.size())
      return getParentPath(_usdItems.at(index).pathString);
  }
  return "";
}

string USDCrate::getShaderPath(const string &materialPath) const {
  string attributePath = getAttributePath(materialPath, "outputs:surface");
  if (attributePath != "") {
    int index = getItem(attributePath).pathValue();
    if (index >= 0 && index < _usdItems.size())
      return getParentPath(_usdItems.at(index).pathString);
  }
  return "";
}

string USDCrate::getTextureFile(const string &itemPath, const string &name) const {
  return getStringValue(getInputPath(getTexturePath(itemPath, name), "file"));
}

RGBA USDCrate::getColorInput(const string &itemPath, const string &name) const {
  if (isTextureInput(itemPath, name))
    return getFloat4(getInputPath(getTexturePath(itemPath, name), "fallback")) * 255.0f;
  else if (hasInput(itemPath, name)) {
    const USDItem &item = getItem(getInputPath(itemPath, name));
    if (item.dataType == USD_FLOAT)
      return RGBA(vec3(item.floatValue() * 255.0f), 255);
    if (item.dataType == USD_VEC3_F)
      return RGBA(item.float3Value() * 255.0f, 255);
  }
  return RGBA(0, 0, 0, 255);
}

string USDCrate::getFirstMeshPath(const string &path) const {
  StringVector meshPaths;
  getMeshPaths(meshPaths, path);
  return meshPaths.size() > 0 ? meshPaths.front() : "";
}

bool USDCrate::getMeshPaths(StringVector &paths, const string &path) const {
  if (hasItem(path)) {
    const USDItem &item = getItem(path);
    for (int index : item.children) {
      const USDItem &child = _usdItems.at(index);
      if (child.typeName == "Mesh")
        paths.push_back(child.pathString);
    }
    return paths.size() > 0;
  }
  return false;
}

string USDCrate::getMaterialPath(const string &path) const {
  string attPath = getAttributePath(path, "material:binding");
  if (attPath != "") {
    int index = getItem(attPath).pathValue();
    if (index >= 0 && index < _usdItems.size())
      return _usdItems.at(index).pathString;
  }
  return "";
}

vec4 USDCrate::getFloat4(const string &path) const {
  return hasItem(path) ? getItem(path).float4Value() : vec4();
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
  
  for (auto &path : paths) {
    std::string pathStr = (pathStack.size() > 0 ? (pathStack.back() + "/" + path.token) : "");
    _pathMap[pathStr] = path.index;
    USDItem &item = _usdItems[path.index];
    item.setToPath(path, pathStr, tokens, this);
    
    if (path.leaf) {
      USDItem &parent = _usdItems[itemStack.back()];
      parent.attributes.push_back(path.index);
      if (path.jump == -2) {
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
        parent.children.push_back(path.index);
      }
      else {
        _rootItem = path.index;
      }
      itemStack.push_back(path.index);
      pathStack.push_back(pathStr);
    }
  }
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

ostream& USDCrate::print(ostream &os) const {
  return os << _usdItems.at(_rootItem);
}

int USDCrate::findAttributeIndex(const string &itemPath, const string &name) const {
  if (hasItem(itemPath)) {
    const USDItem &item = getItem(itemPath);
    for (int index : item.attributes) {
      const USDItem &attribute = _usdItems.at(index);
      if (attribute.name == name)
        return index;
    }
  }
  return -1;
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
