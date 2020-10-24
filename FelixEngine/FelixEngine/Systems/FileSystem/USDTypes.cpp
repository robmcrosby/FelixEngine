//
//  USDTypes.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/18/20.
//  Copyright © 2020 Robert Crosby. All rights reserved.
//

#include "USDTypes.h"


using namespace fx;
using namespace std;


void USDItem::setToPath(Path &path, StringMap &paths, StringVector &tokens, size_t fileOffset, istream &is) {
  name = path.token;
  reps = path.spec.reps;
  if (reps.count("typeName") > 0 && reps["typeName"].type == USD_TOKEN)
    typeName = tokens[reps["typeName"].payload];
  if (reps.count("default")) {
    Rep rep = reps["default"];
    if (rep.type == USD_TOKEN && !rep.array && rep.inlined)
      setTokenValue(tokens[rep.payload]);
    else if (rep.type == USD_ASSET && !rep.array && rep.inlined)
      setAssetValue(tokens[rep.payload]);
    else if (rep.type == USD_FLOAT && !rep.array && rep.inlined)
      setValue((float)rep.payload);
    else if (rep.type == USD_VEC2_F) {
      if (!rep.inlined && !rep.array) {
        is.seekg(fileOffset + rep.payload);
        vec2 value;
        readL(&value.x, 2, is);
        setValue(value);
      }
    }
    else if (rep.type == USD_VEC3_F) {
      if (!rep.inlined && !rep.array) {
        is.seekg(fileOffset + rep.payload);
        vec3 value;
        readL(&value.x, 3, is);
        setValue(value);
      }
    }
    else if (rep.type == USD_VEC4_F) {
      if (!rep.inlined && !rep.array) {
        is.seekg(fileOffset + rep.payload);
        vec4 value;
        readL(&value.x, 4, is);
        setValue(value);
      }
    }
  }
  else if (reps.count("targetChildren") > 0) {
    typeName = "rel";
    is.seekg(fileOffset + reps["targetChildren"].payload + 8);
    setPathValue(paths[readIntL(is)]);
  }
  else if (reps.count("connectionChildren") > 0) {
    is.seekg(fileOffset + reps["connectionChildren"].payload + 8);
    setPathValue(paths[readIntL(is)]);
  }
}

void USDItem::print(ostream &os, string indent) const {
  if (isAttribute()) {
    os << indent << "-" << typeName << " " << name;
    if (dataType != USD_INVALID && data.size() > 0) {
      if (dataType == USD_TOKEN)
        os << " = \"" << tokenValue() << "\"" << endl;
      else if (dataType == USD_ASSET)
        os << " = <" << assetValue() << ">" << endl;
      else if (dataType == USD_FLOAT)
        os << " = " << floatValue() << endl;
      else if (dataType == USD_VEC2_F)
        os << " = " << vec2Value() << endl;
      else if (dataType == USD_VEC3_F)
        os << " = " << vec3Value() << endl;
      else if (dataType == USD_VEC4_F)
        os << " = " << vec4Value() << endl;
      else if (dataType == USD_PATH_VECTOR) {
        if (typeName == "rel")
          os << " = <" << pathValue() << ">" << endl;
        else
          os << ".connect = <" << pathValue() << ">" << endl;
      }
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
      itr->second.print(os, indent + "  ");
    
    // Print Children
    for (auto itr = children.begin(); itr != children.end(); ++itr)
      itr->print(os, indent + "  ");
  }
}

void USDItem::setTokenValue(const string &token) {
  dataType = USD_TOKEN;
  data.resize(token.size()+1);
  memcpy(&data[0], &token[0], token.size());
  data[token.size()] = '\0';
}

string USDItem::tokenValue() const {
  return &data[0];
}

void USDItem::setAssetValue(const string &asset) {
  dataType = USD_ASSET;
  data.resize(asset.size()+1);
  memcpy(&data[0], &asset[0], asset.size());
  data[asset.size()] = '\0';
}

string USDItem::assetValue() const {
  return &data[0];
}

void USDItem::setPathValue(const string &path) {
  dataType = USD_PATH_VECTOR;
  data.resize(path.size()+1);
  memcpy(&data[0], &path[0], path.size());
  data[path.size()] = '\0';
}

string USDItem::pathValue() const {
  return &data[0];
}

void USDItem::setValue(float value) {
  dataType = USD_FLOAT;
  data.resize(sizeof(float));
  memcpy(&data[0], &value, sizeof(float));
}

float USDItem::floatValue() const {
  return (float)*(&data[0]);
}

void USDItem::setValue(vec2 value) {
  dataType = USD_VEC2_F;
  data.resize(sizeof(vec2));
  memcpy(&data[0], value.ptr(), sizeof(vec2));
}

vec2 USDItem::vec2Value() const {
  return ((vec2*)(&data[0]))[0];
}

void USDItem::setValue(vec3 value) {
  dataType = USD_VEC3_F;
  data.resize(sizeof(vec3));
  memcpy(&data[0], value.ptr(), sizeof(vec3));
}

vec3 USDItem::vec3Value() const {
  return ((vec3*)(&data[0]))[0];
}

void USDItem::setValue(vec4 value) {
  dataType = USD_VEC4_F;
  data.resize(sizeof(vec4));
  memcpy(&data[0], value.ptr(), sizeof(vec4));
}

vec4 USDItem::vec4Value() const {
  return ((vec4*)(&data[0]))[0];
}
