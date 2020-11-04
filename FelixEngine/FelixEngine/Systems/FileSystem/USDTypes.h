//
//  Header.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/17/20.
//  Copyright © 2020 Robert Crosby. All rights reserved.
//

#ifndef Header_h
#define Header_h

#include "FileSystem.h"

namespace fx {

  enum USD_TYPE {
    USD_INVALID = 0,
    USD_BOOL,
    USD_UCHAR,
    USD_INT,
    USD_UINT,
    USD_INT64,
    USD_UINT64,
    USD_HALF,
    USD_FLOAT,
    USD_DOUBLE,
    USD_STRING,
    USD_TOKEN,
    USD_ASSET,
    USD_MATRIX_2D,
    USD_MATRIX_3D,
    USD_MATRIX_4D,
    USD_QUAT_D,
    USD_QUAT_F,
    USD_QUAT_H,
    USD_VEC2_D,
    USD_VEC2_F,
    USD_VEC2_H,
    USD_VEC2_I,
    USD_VEC3_D,
    USD_VEC3_F,
    USD_VEC3_H,
    USD_VEC3_I,
    USD_VEC4_D,
    USD_VEC4_F,
    USD_VEC4_H,
    USD_VEC4_I,
    USD_DICTIONARY,
    USD_TOKEN_LIST_OP,
    USD_STRING_LIST_OP,
    USD_PATH_LIST_OP,
    USD_REFERENCE_LIST_OP,
    USD_INT_LIST_OP,
    USD_INT64_LIST_OP,
    USD_UINT_LIST_OP,
    USD_UINT64_LIST_OP,
    USD_PATH_VECTOR,
    USD_TOKEN_VECTOR,
    USD_SPECIFIER,
    USD_PERMISSION,
    USD_VARIABILITY,
    USD_VARIANT_SELECTION_MAP,
    USD_TIME_SAMPLES,
    USD_PAYLOAD,
    USD_DOUBLE_VECTOR,
    USD_LAYER_OFFSET_VECTOR,
    USD_STRING_VECTOR,
    USD_VALUE_BLOCK,
    USD_VALUE,
    USD_UNREGISTERED_VALUE,
    USD_UNREGISTERED_VALUE_LIST_OP,
    USD_PAYLOAD_LIST_OP,
  };

  struct Rep {
    USD_TYPE type;
    bool array;
    bool inlined;
    bool compressed;
    long payload;
  };
  typedef std::vector<Rep> RepVector;
  typedef std::map<std::string, Rep> RepMap;

  struct Spec {
    int type;
    RepMap reps;
  };
  typedef std::map<int, Spec> SpecMap;

  struct Path {
    int index;
    int jump;
    bool leaf;
    Spec spec;
    std::string token;
  };
  typedef std::vector<Path> PathVector;

  class USDCrate;

  struct USDItem {
    std::string name;
    std::string pathString;
    std::string typeName;
    USD_TYPE dataType;
    FileData data;
    bool isArray;
    bool isCompressed;
    RepMap reps;
    
    StringVector attributeNames;
    std::map<std::string, int> attributes;
    IntVector children;
    
    bool isAttribute() const {return attributes.size() == 0 && children.size() == 0;}
    
    void setToPath(Path &path, std::string &pathStr, StringVector &tokens, USDCrate *crate);
    void print(std::ostream &os, const USDCrate *crate, std::string indent = "") const;
    
    void setTokenValue(const std::string &token);
    std::string tokenValue() const;
    
    void setAssetValue(const std::string &asset);
    std::string assetValue() const;
    
    void setPathValue(int path);
    int pathValue() const;
    
    void setValue(const std::string &str);
    std::string stringValue() const;
    
    void setValue(int value);
    int intValue() const;
    
    void setValue(long value);
    long longValue() const;
    
    void setValue(float value);
    float floatValue() const;
    
    void setValue(vec2 value);
    vec2 vec2Value() const;
    
    void setValue(vec3 value);
    vec3 vec3Value() const;
    
    void setValue(vec4 value);
    vec4 vec4Value() const;
    
    void setValue(double2x2 value);
    double2x2 double2x2Value() const;
    
    void setValue(double3x3 value);
    double3x3 double3x3Value() const;
    
    void setValue(double4x4 value);
    double4x4 double4x4Value() const;
    
    void setArray(USD_TYPE type, long offset, bool compressed);
    bool getArray(IntVector &dst, const USDCrate *crate) const;
    bool getArray(std::vector<float> &dst, const USDCrate *crate) const;
    bool getArray(std::vector<vec2> &dst, const USDCrate *crate) const;
    bool getArray(std::vector<vec3> &dst, const USDCrate *crate) const;
    bool getArray(std::vector<vec4> &dst, const USDCrate *crate) const;
    
    bool appendBuffer(VertexBuffer &dst, const USDCrate *crate) const;
  };
}


#endif /* Header_h */
