//
//  Variant.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/11/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Variant_h
#define Variant_h

#include <vector>
#include <map>

#include "Vector.h"
#include "Matrix.h"
#include "Color.h"

#define FLOAT_STR "float"
#define VEC2_STR "vec2"
#define VEC3_STR "vec3"
#define VEC4_STR "vec4"

#define INT_STR "int"
#define IVEC2_STR "ivec2"
#define IVEC3_STR "ivec3"
#define IVEC4_STR "ivec4"


namespace fx
{
  enum VAR_TYPE
  {
    VAR_FLOAT = 0,
    VAR_FLOAT_2,
    VAR_FLOAT_3,
    VAR_FLOAT_4,
    
    VAR_INT,
    VAR_INT_2,
    VAR_INT_3,
    VAR_INT_4,
    
    VAR_MTX_2X2,
    VAR_MTX_3X3,
    VAR_MTX_4X4,
    
    VAR_UNKNOWN,
  };
  
  enum VAR_SIZE
  {
    VAR_SIZE_1   = 4,
    VAR_SIZE_2   = 8,
    VAR_SIZE_3   = 12,
    VAR_SIZE_4   = 16,
    VAR_SIZE_2X2 = 16,
    VAR_SIZE_3X3 = 36,
    VAR_SIZE_4X4 = 64,
    VAR_SIZE_UNKNOWN = 0
  };
  
  class Variant
  {
  public:
    static size_t GetTypeSize(VAR_TYPE type)
    {
      switch (type)
      {
        case VAR_FLOAT:
        case VAR_INT:
          return VAR_SIZE_1;
        case VAR_FLOAT_2:
        case VAR_INT_2:
          return VAR_SIZE_2;
        case VAR_FLOAT_3:
        case VAR_INT_3:
          return VAR_SIZE_3;
        case VAR_FLOAT_4:
        case VAR_INT_4:
          return VAR_SIZE_4;
        case VAR_MTX_2X2:
          return VAR_SIZE_2X2;
        case VAR_MTX_3X3:
          return VAR_SIZE_3X3;
        case VAR_MTX_4X4:
          return VAR_SIZE_4X4;
        case VAR_UNKNOWN:
          return VAR_SIZE_UNKNOWN;
      }
      return VAR_SIZE_UNKNOWN;
    }
    
    static VAR_TYPE GetVariantType(const std::string &str)
    {
      if (str == FLOAT_STR)
        return VAR_FLOAT;
      if (str == VEC2_STR)
        return VAR_FLOAT_2;
      if (str == VEC3_STR)
        return VAR_FLOAT_3;
      if (str == VEC4_STR)
        return VAR_FLOAT_4;
      if (str == INT_STR)
        return VAR_INT;
      if (str == IVEC2_STR)
        return VAR_INT_2;
      if (str == IVEC3_STR)
        return VAR_INT_3;
      if (str == IVEC4_STR)
        return VAR_INT_4;
      return VAR_UNKNOWN;
    }
    
  public:
    Variant(VAR_TYPE type = VAR_UNKNOWN, size_t size = 1): mType(VAR_UNKNOWN) {resize(type, size);}
    Variant(const std::string &type, const std::string &str): mType(VAR_UNKNOWN) {parse(type, str);}
    
    void resize(size_t size) {mData.resize(mTypeSize * size);}
    void resize(VAR_TYPE type, size_t size)
    {
      mType = type;
      mTypeSize = GetTypeSize(mType);
      resize(size);
    }
    void setValues(VAR_TYPE type, const void *ptr, size_t size = 1)
    {
      resize(type, size);
      memcpy(&mData.at(0), ptr, mTypeSize*size);
    }
    
    void setValues(const float *ptr, size_t size = 1) {setValues(VAR_FLOAT,   (const void*)ptr, size);}
    void setValues(const vec2  *ptr, size_t size = 1) {setValues(VAR_FLOAT_2, (const void*)&ptr->x, size);}
    void setValues(const vec3  *ptr, size_t size = 1) {setValues(VAR_FLOAT_3, (const void*)&ptr->x, size);}
    void setValues(const vec4  *ptr, size_t size = 1) {setValues(VAR_FLOAT_4, (const void*)&ptr->x, size);}
    void setValues(const RGBAf *ptr, size_t size = 1) {setValues(VAR_FLOAT_4, (const void*)&ptr->red, size);}
    
    void setValues(const int   *ptr, size_t size = 1) {setValues(VAR_INT,   (const void*)ptr, size);}
    void setValues(const ivec2 *ptr, size_t size = 1) {setValues(VAR_INT_2, (const void*)&ptr->x, size);}
    void setValues(const ivec3 *ptr, size_t size = 1) {setValues(VAR_INT_3, (const void*)&ptr->x, size);}
    void setValues(const ivec4 *ptr, size_t size = 1) {setValues(VAR_INT_4, (const void*)&ptr->x, size);}
    
    void setValues(const mat2 *ptr, size_t size = 1) {setValues(VAR_MTX_2X2, (const void*)&ptr->x.x, size);}
    void setValues(const mat3 *ptr, size_t size = 1) {setValues(VAR_MTX_3X3, (const void*)&ptr->x.x, size);}
    void setValues(const mat4 *ptr, size_t size = 1) {setValues(VAR_MTX_4X4, (const void*)&ptr->x.x, size);}
    
    Variant& operator=(const float &value) {setValues(&value); return *this;}
    Variant& operator=(const vec2  &value) {setValues(&value); return *this;}
    Variant& operator=(const vec3  &value) {setValues(&value); return *this;}
    Variant& operator=(const vec4  &value) {setValues(&value); return *this;}
    Variant& operator=(const RGBAf &value) {setValues(&value); return *this;}
    
    Variant& operator=(const int   &value) {setValues(&value); return *this;}
    Variant& operator=(const ivec2 &value) {setValues(&value); return *this;}
    Variant& operator=(const ivec3 &value) {setValues(&value); return *this;}
    Variant& operator=(const ivec4 &value) {setValues(&value); return *this;}
    
    Variant& operator=(const mat2 &value) {setValues(&value); return *this;}
    Variant& operator=(const mat3 &value) {setValues(&value); return *this;}
    Variant& operator=(const mat4 &value) {setValues(&value); return *this;}
    
    VAR_TYPE type() const {return mType;}
    bool isIntType() const {return mType == VAR_INT || mType == VAR_INT_2 || mType == VAR_INT_3 || mType == VAR_INT_4;}
    bool isFloatType() const {return !isIntType();}
    
    size_t typeSize() const {return mTypeSize;}
    size_t size() const {return mData.size()/mTypeSize;}
    
    void* ptr() {return mData.size() ? &mData.at(0) : nullptr;}
    const void* ptr() const {return mData.size() ? &mData.at(0) : nullptr;}
    
    char& operator[](int index) {return mData.at(index * mTypeSize);}
    const char& operator[](int index) const {return mData.at(index * mTypeSize);}
    
    float& floatAt(int index) {return (float&)mData.at(index*GetTypeSize(VAR_FLOAT));}
    vec2&  vec2At(int index)  {return (vec2&)mData.at(index*GetTypeSize(VAR_FLOAT_2));}
    vec3&  vec3At(int index)  {return (vec3&)mData.at(index*GetTypeSize(VAR_FLOAT_3));}
    vec4&  vec4At(int index)  {return (vec4&)mData.at(index*GetTypeSize(VAR_FLOAT_4));}
    RGBAf& RGBAfAt(int index) {return (RGBAf&)mData.at(index*GetTypeSize(VAR_FLOAT_4));}
    
    int&   intAt(int index)   {return (int&)mData.at(index*GetTypeSize(VAR_INT));}
    ivec2& ivec2At(int index) {return (ivec2&)mData.at(index*GetTypeSize(VAR_INT_2));}
    ivec3& ivec3At(int index) {return (ivec3&)mData.at(index*GetTypeSize(VAR_INT_3));}
    ivec4& ivec4At(int index) {return (ivec4&)mData.at(index*GetTypeSize(VAR_INT_4));}
    
    mat2& mat2At(int index) {return (mat2&)mData.at(index*GetTypeSize(VAR_MTX_2X2));}
    mat3& mat3At(int index) {return (mat3&)mData.at(index*GetTypeSize(VAR_MTX_3X3));}
    mat4& mat4At(int index) {return (mat4&)mData.at(index*GetTypeSize(VAR_MTX_4X4));}
    
    float floatValue(int index = 0) const {return isIntType() ? (float)intValue(index) : *((const float*)&mData.at(index*GetTypeSize(VAR_FLOAT)));}
    vec2 vec2Value(int index = 0) const {return isIntType() ? vec2(ivec2Value(index)) : *((const vec2*)&mData.at(index*GetTypeSize(VAR_FLOAT_2)));}
    vec3 vec3Value(int index = 0) const {return isIntType() ? vec3(ivec3Value(index)) : *((const vec3*)&mData.at(index*GetTypeSize(VAR_FLOAT_3)));}
    vec4 vec4Value(int index = 0) const {return isIntType() ? vec4(ivec4Value(index)) : *((const vec4*)&mData.at(index*GetTypeSize(VAR_FLOAT_4)));}
    RGBAf RGBAfValue(int index = 0) const
    {
      if (isIntType())
      {
        vec4 color = vec4(ivec4Value(index));
        return RGBAf(color.x, color.y, color.z, color.w);
      }
      return *((const RGBAf*)&mData.at(index*GetTypeSize(VAR_FLOAT_4)));
    }
    
    int intValue(int index = 0) const {return isIntType() ? *((const int*)&mData.at(index*GetTypeSize(VAR_INT))) : (int)floatValue(index);}
    ivec2 ivec2Value(int index = 0) const {return isIntType() ? *((const ivec2*)&mData.at(index*GetTypeSize(VAR_INT_2))) : ivec2(vec2Value(index));}
    ivec3 ivec3Value(int index = 0) const {return isIntType() ? *((const ivec3*)&mData.at(index*GetTypeSize(VAR_INT_3))) : ivec3(vec3Value(index));}
    ivec4 ivec4Value(int index = 0) const {return isIntType() ? *((const ivec4*)&mData.at(index*GetTypeSize(VAR_INT_4))) : ivec4(vec4Value(index));}
    
    mat2 mat2Value(int index = 0) const {return *((const mat2*)&mData.at(index*GetTypeSize(VAR_MTX_2X2)));}
    mat3 mat3Value(int index = 0) const {return *((const mat3*)&mData.at(index*GetTypeSize(VAR_MTX_3X3)));}
    mat4 mat4Value(int index = 0) const {return *((const mat4*)&mData.at(index*GetTypeSize(VAR_MTX_4X4)));}
    
    operator float() const {return floatValue();}
    operator int()   const {return intValue();}
    
    operator vec2() const {return vec2Value();}
    operator vec3() const {return vec3Value();}
    operator vec4() const {return vec4Value();}
    
    operator RGBAf() const {return RGBAfValue();}
    
    operator ivec2() const {return ivec2Value();}
    operator ivec3() const {return ivec3Value();}
    operator ivec4() const {return ivec4Value();}
    
    operator mat2() const {return mat2Value();}
    operator mat3() const {return mat3Value();}
    operator mat4() const {return mat4Value();}
    
    bool parse(const std::string &type, const std::string &str) {return parse(GetVariantType(type), str);}
    bool parse(VAR_TYPE type, const std::string &str)
    {
      bool success = false;
      
      if (type == VAR_FLOAT)
      {
        float value(0.0f);
        success = sscanf(str.c_str(), " %f", &value) == 1;
        *this = value;
      }
      else if (type == VAR_FLOAT_2)
      {
        vec2 value(0.0f, 0.0f);
        success = value.parse(str);
        *this = value;
      }
      else if (type == VAR_FLOAT_3)
      {
        vec3 value(0.0f, 0.0f, 0.0f);
        success = value.parse(str);
        *this = value;
      }
      else if (type == VAR_FLOAT_4)
      {
        vec4 value(0.0f, 0.0f, 0.0f, 0.0f);
        success = value.parse(str);
        *this = value;
      }
      else if (type == VAR_INT)
      {
        int value(0);
        success = sscanf(str.c_str(), " %i", &value) == 1;
        *this = value;
      }
      else if (type == VAR_INT_2)
      {
        ivec2 value(0, 0);
        success = value.parse(str);
        *this = value;
      }
      else if (type == VAR_INT_3)
      {
        ivec3 value(0, 0, 0);
        success = value.parse(str);
        *this = value;
      }
      else if (type == VAR_INT_4)
      {
        ivec4 value(0, 0, 0, 0);
        success = value.parse(str);
        *this = value;
      }
      
      return success;
    }
    
  private:
    VAR_TYPE mType;
    size_t mTypeSize;
    std::vector<char> mData;
  };
  
  typedef std::map<std::string, Variant> VariantMap;
}

#endif /* Variant_h */
