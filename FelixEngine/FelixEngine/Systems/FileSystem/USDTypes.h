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
    int jump;
    bool leaf;
    Spec spec;
    std::string token;
  };
  typedef std::vector<Path> PathVector;

  struct USDAttribute {
    std::string name;
    std::string typeName;
    USD_TYPE dataType;
    FileData data;
    RepMap reps;
    
    void setToPath(Path &path, StringVector &tokens);
    void print(std::ostream &os, std::string indent = "") const;
    
    void setTokenValue(const std::string &token);
    std::string tokenValue() const;
    
    void setAssetValue(const std::string &asset);
    std::string assetValue() const;
    
    void setValue(float value);
    float floatValue() const;
  };
  typedef std::map<std::string, USDAttribute> USDAttributeMap;

  struct USDItem {
    std::string name;
    std::string typeName;
    USDAttributeMap attributes;
    std::vector<USDItem> children;
    RepMap reps;
    
    void setToPath(Path &path, StringVector &tokens);
    void print(std::ostream &os, std::string indent = "") const;
  };
  typedef std::vector<USDItem> USDData;
}


#endif /* Header_h */
