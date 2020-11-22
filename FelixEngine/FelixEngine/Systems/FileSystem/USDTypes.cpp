//
//  USDTypes.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/18/20.
//  Copyright © 2020 Robert Crosby. All rights reserved.
//

#include "USDTypes.h"
#include "USDCrate.h"
#include "CompressionUtils.h"


using namespace fx;
using namespace std;


void USDItem::setToPath(Path &path, string &pathStr, StringVector &tokens, USDCrate *crate) {
  name = path.token;
  pathString = pathStr;
  isArray = false;
  reps = path.spec.reps;
  
  size_t fileOffset = crate->_fileOffset;
  istream &is = *crate->_fileStream;
  
  if (reps.count("typeName") > 0 && reps["typeName"].type == USD_TOKEN)
    typeName = tokens[reps["typeName"].payload];
  if (reps.count("default")) {
    Rep rep = reps["default"];
    if (rep.array)
      setArray(rep.type, rep.payload, rep.compressed);
    else if (rep.type == USD_TOKEN && rep.inlined)
      setTokenValue(tokens[rep.payload]);
    else if (rep.type == USD_ASSET && rep.inlined)
      setAssetValue(tokens[rep.payload]);
    else if (rep.type == USD_INT && rep.inlined)
      setValue((int)rep.payload);
    else if (rep.type == USD_FLOAT && rep.inlined)
      setValue((float)rep.payload);
    else if (rep.type == USD_VEC2_F) {
      if (!rep.inlined) {
        is.seekg(fileOffset + rep.payload);
        vec2 value;
        readL(&value.x, 2, is);
        setValue(value);
      }
      else {
        setValue(vec2());
      }
    }
    else if (rep.type == USD_VEC3_F) {
      if (!rep.inlined) {
        is.seekg(fileOffset + rep.payload);
        vec3 value;
        readL(&value.x, 3, is);
        setValue(value);
      }
      else {
        setValue(vec3());
      }
    }
    else if (rep.type == USD_VEC4_F) {
      if (!rep.inlined) {
        is.seekg(fileOffset + rep.payload);
        vec4 value;
        readL(&value.x, 4, is);
        setValue(value);
      }
      else {
        setValue(vec4());
      }
    }
    else if (rep.type == USD_MATRIX_2D) {
      if (!rep.inlined) {
        is.seekg(fileOffset + rep.payload);
        double2x2 value;
        readL(&value.x.x, 4, is);
        setValue(value);
      }
      else {
        setValue(double2x2());
      }
    }
    else if (rep.type == USD_MATRIX_3D) {
      if (!rep.inlined) {
        is.seekg(fileOffset + rep.payload);
        double3x3 value;
        readL(&value.x.x, 9, is);
        setValue(value);
      }
      else {
        setValue(double3x3());
      }
    }
    else if (rep.type == USD_MATRIX_4D) {
      if (!rep.inlined) {
        is.seekg(fileOffset + rep.payload);
        double4x4 value;
        readL(&value.x.x, 16, is);
        setValue(value);
      }
      else {
        setValue(double4x4());
      }
    }
  }
  else if (reps.count("targetChildren") > 0) {
    typeName = "rel";
    is.seekg(fileOffset + reps["targetChildren"].payload + 8);
    setPathValue(readIntL(is));
  }
  else if (reps.count("connectionChildren") > 0) {
    is.seekg(fileOffset + reps["connectionChildren"].payload + 8);
    setPathValue(readIntL(is));
  }
}

void USDItem::print(ostream &os, const USDCrate *crate, string indent) const {
  if (isAttribute()) {
    os << indent << "-" << typeName << " " << name;
    if (dataType != USD_INVALID && data.size() > 0 && !isArray) {
      if (dataType == USD_TOKEN)
        os << " = \"" << tokenValue() << "\"" << endl;
      else if (dataType == USD_ASSET)
        os << " = <" << assetValue() << ">" << endl;
      else if (dataType == USD_INT)
        os << " = " << intValue() << endl;
      else if (dataType == USD_FLOAT)
        os << " = " << floatValue() << endl;
      else if (dataType == USD_VEC2_F)
        os << " = " << float2Value() << endl;
      else if (dataType == USD_VEC3_F)
        os << " = " << float3Value() << endl;
      else if (dataType == USD_VEC4_F)
        os << " = " << float4Value() << endl;
      else if (dataType == USD_PATH_VECTOR) {
        if (typeName == "rel")
          os << " = <" << crate->_usdItems[pathValue()].pathString << ">" << endl;
        else
          os << ".connect = <" << crate->_usdItems[pathValue()].pathString << ">" << endl;
      }
      else if (dataType == USD_MATRIX_2D)
        os << " = " << endl << double2x2Value();
      else if (dataType == USD_MATRIX_3D)
        os << " = " << endl << double3x3Value();
      else if (dataType == USD_MATRIX_4D)
        os << " = " << endl << double4x4Value();
    }
    else if (dataType != USD_INVALID && isArray) {
      os << " = {...}" << endl;
    }
    else {
      os << endl;
    }
    // Print Attribute Meta Data
//    for (auto itr = reps.begin(); itr != reps.end(); ++itr) {
//      os << indent << "  " << itr->first << ": " << itr->second.type;
//      if (itr->second.array)
//        os << " []";
//      if (itr->second.inlined)
//        os << " inlined: " << itr->second.payload;
//      os << endl;
//    }
  }
  else {
    os << indent << "+" << typeName << " " << name << endl;
    
    // Print Meta Data
//    for (auto itr = reps.begin(); itr != reps.end(); ++itr)
//      os << indent << "   " << itr->first << ": " << itr->second.type << endl;
    
    // Print Attributes
    for (auto itr = attributes.begin(); itr != attributes.end(); ++itr)
      crate->_usdItems[*itr].print(os, crate, indent + "  ");
    
    // Print Children
    for (auto itr = children.begin(); itr != children.end(); ++itr)
      crate->_usdItems[*itr].print(os, crate, indent + "  ");
  }
}

void USDItem::setTokenValue(const string &token) {
  setValue(token);
  isArray = false;
  dataType = USD_TOKEN;
}

string USDItem::tokenValue() const {
  return &data[0];
}

void USDItem::setAssetValue(const string &asset) {
  setValue(asset);
  isArray = false;
  dataType = USD_ASSET;
}

string USDItem::assetValue() const {
  return &data[0];
}

void USDItem::setPathValue(int path) {
  setValue(path);
  isArray = false;
  dataType = USD_PATH_VECTOR;
}

int USDItem::pathValue() const {
  return intValue();
}

void USDItem::setValue(const string &str) {
  dataType = USD_STRING;
  isArray = false;
  data.resize(str.size()+1);
  memcpy(&data[0], &str[0], str.size());
  data[str.size()] = '\0';
}

string USDItem::stringValue() const {
  return &data[0];
}

void USDItem::setValue(int value) {
  dataType = USD_INT;
  isArray = false;
  data.resize(sizeof(int));
  memcpy(&data[0], &value, sizeof(int));
}

int USDItem::intValue() const {
  return (int)*(&data[0]);
}

void USDItem::setValue(long value) {
  dataType = USD_INT64;
  isArray = false;
  data.resize(sizeof(long));
  memcpy(&data[0], &value, sizeof(long));
}

long USDItem::longValue() const {
  return ((long*)&data[0])[0];
}

void USDItem::setValue(float value) {
  dataType = USD_FLOAT;
  isArray = false;
  data.resize(sizeof(float));
  memcpy(&data[0], &value, sizeof(float));
}

float USDItem::floatValue() const {
  return (float)*(&data[0]);
}

void USDItem::setValue(vec2 value) {
  dataType = USD_VEC2_F;
  isArray = false;
  data.resize(sizeof(vec2));
  memcpy(&data[0], value.ptr(), sizeof(vec2));
}

vec2 USDItem::float2Value() const {
  return ((vec2*)(&data[0]))[0];
}

void USDItem::setValue(vec3 value) {
  dataType = USD_VEC3_F;
  isArray = false;
  data.resize(sizeof(vec3));
  memcpy(&data[0], value.ptr(), sizeof(vec3));
}

vec3 USDItem::float3Value() const {
  return ((vec3*)(&data[0]))[0];
}

void USDItem::setValue(vec4 value) {
  dataType = USD_VEC4_F;
  isArray = false;
  data.resize(sizeof(vec4));
  memcpy(&data[0], value.ptr(), sizeof(vec4));
}

vec4 USDItem::float4Value() const {
  return ((vec4*)(&data[0]))[0];
}

void USDItem::setValue(double2x2 value) {
  dataType = USD_MATRIX_2D;
  isArray = false;
  data.resize(sizeof(double2x2));
  memcpy(&data[0], value.ptr(), sizeof(double2x2));
}

double2x2 USDItem::double2x2Value() const {
  return ((double2x2*)(&data[0]))[0];
}

void USDItem::setValue(double3x3 value) {
  dataType = USD_MATRIX_3D;
  isArray = false;
  data.resize(sizeof(double3x3));
  memcpy(&data[0], value.ptr(), sizeof(double3x3));
}

double3x3 USDItem::double3x3Value() const {
  return ((double3x3*)(&data[0]))[0];
}

void USDItem::setValue(double4x4 value) {
  dataType = USD_MATRIX_4D;
  isArray = false;
  data.resize(sizeof(double4x4));
  memcpy(&data[0], value.ptr(), sizeof(double4x4));
}

double4x4 USDItem::double4x4Value() const {
  return ((double4x4*)(&data[0]))[0];
}

void USDItem::setArray(USD_TYPE type, long offset, bool compressed) {
  setValue(offset);
  dataType = type;
  isArray = true;
  isCompressed = compressed;
}

bool USDItem::getArray(IntVector &dst, const USDCrate *crate) const {
  istream &is = *crate->_fileStream;
  is.seekg(crate->_fileOffset + longValue());
  long count = crate->_fileVersion < 7 ? (long)readIntL(is) : readLongL(is);
  if (isCompressed) {
    long compressedSize = readLongL(is);
    FileData buffer;
    return CompressionUtils::decompressLZ4(buffer, is, compressedSize) && CompressionUtils::decompressUSD32(dst, buffer, count);
  }
  dst.resize(count);
  return readL(&dst[0], count, is) == count * sizeof(int);
}

bool USDItem::getArray(std::vector<float> &dst, const USDCrate *crate) const {
  istream &is = *crate->_fileStream;
  is.seekg(crate->_fileOffset + longValue());
  size_t count = crate->_fileVersion < 7 ? readIntL(is) : readLongL(is);
  dst.resize(count);
  return readL(&dst[0], count, is) == count * sizeof(float);
}

bool USDItem::getArray(std::vector<vec2> &dst, const USDCrate *crate) const {
  istream &is = *crate->_fileStream;
  is.seekg(crate->_fileOffset + longValue());
  size_t count = crate->_fileVersion < 7 ? readIntL(is) : readLongL(is);
  dst.resize(count);
  return readL(&dst[0].x, count*2, is) == count*2 * sizeof(float);
}

bool USDItem::getArray(std::vector<vec3> &dst, const USDCrate *crate) const {
  istream &is = *crate->_fileStream;
  is.seekg(crate->_fileOffset + longValue());
  size_t count = crate->_fileVersion < 7 ? readIntL(is) : readLongL(is);
  dst.resize(count);
  return readL(&dst[0].x, count*3, is) == count*3 * sizeof(float);
}

bool USDItem::getArray(std::vector<vec4> &dst, const USDCrate *crate) const {
  istream &is = *crate->_fileStream;
  is.seekg(crate->_fileOffset + longValue());
  size_t count = crate->_fileVersion < 7 ? readIntL(is) : readLongL(is);
  dst.resize(count);
  return readL(&dst[0].x, count*4, is) == count*4 * sizeof(float);
}

bool USDItem::appendBuffer(VertexBuffer &dst, const USDCrate *crate) const {
  istream &is = *crate->_fileStream;
  is.seekg(crate->_fileOffset + longValue());
  size_t count = crate->_fileVersion < 7 ? readIntL(is) : readLongL(is);
  if (dataType == USD_VEC2_F)
    count *= 2;
  else if (dataType == USD_VEC3_F)
    count *= 3;
  else if (dataType == USD_VEC4_F)
    count *= 4;
  else if (dataType != USD_FLOAT) {
    cerr << "Error appending to Vertex Buffer: ";
    cerr << "Unsupported Data Type" << endl;
    return false;
  }
  size_t base = dst.size();
  dst.resize(base + count);
  return readL(&dst[base], count, is) == count * sizeof(float);
}
