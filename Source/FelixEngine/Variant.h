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
#include <stdint.h>

#include "Vector.h"
#include "Matrix.h"
#include "Color.h"

#define RGBA_STR "rgba"

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
    VAR_FLOAT,
    VAR_FLOAT_2,
    VAR_FLOAT_3,
    VAR_FLOAT_4,
    
    VAR_CHAR_4,
    
    VAR_INT,
    VAR_INT_2,
    VAR_INT_3,
    VAR_INT_4,
    
    VAR_MTX_2X2,
    VAR_MTX_3X3,
    VAR_MTX_4X4,
    
    VAR_UNKNOWN,
  };
  
  class Variant
  {
  public:
    static size_t GetTypeSize(VAR_TYPE type)
    {
      switch (type)
      {
        case VAR_FLOAT:
        case VAR_CHAR_4:
        case VAR_INT:
          return sizeof(uint32_t);
        case VAR_FLOAT_2:
        case VAR_INT_2:
          return 2*sizeof(uint32_t);
        case VAR_FLOAT_3:
        case VAR_INT_3:
          return 3*sizeof(uint32_t);
        case VAR_FLOAT_4:
        case VAR_INT_4:
        case VAR_MTX_2X2:
          return 4*sizeof(uint32_t);
        case VAR_MTX_3X3:
          return 9*sizeof(uint32_t);
        case VAR_MTX_4X4:
          return 16*sizeof(uint32_t);
        case VAR_UNKNOWN:
          return 0;
      }
      return 0;
    }
    
    static VAR_TYPE GetVariantType(const std::string &str)
    {
      if (str == RGBA_STR)
        return VAR_CHAR_4;
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
    Variant(VAR_TYPE type = VAR_UNKNOWN, size_t width = 1, size_t height = 1): mType(VAR_UNKNOWN) {resize(type, width, height);}
    Variant(const std::string &type, const std::string &str): mType(VAR_UNKNOWN) {parse(type, str);}
    Variant(const XMLTree::Node *node): mType(VAR_UNKNOWN) {setToXml(node);}
    Variant(const XMLTree::Node &node): mType(VAR_UNKNOWN) {setToXml(node);}
    
    Variant(const float &value): mType(VAR_UNKNOWN) {setValues(&value);}
    Variant(const vec2  &value): mType(VAR_UNKNOWN) {setValues(&value);}
    Variant(const vec3  &value): mType(VAR_UNKNOWN) {setValues(&value);}
    Variant(const vec4  &value): mType(VAR_UNKNOWN) {setValues(&value);}
    
    Variant(const RGBA &value): mType(VAR_UNKNOWN) {setValues(&value);}
    Variant(const RGBAf &value): mType(VAR_UNKNOWN) {setValues(&value);}
    
    Variant(const int   &value): mType(VAR_UNKNOWN) {setValues(&value);}
    Variant(const ivec2 &value): mType(VAR_UNKNOWN) {setValues(&value);}
    Variant(const ivec3 &value): mType(VAR_UNKNOWN) {setValues(&value);}
    Variant(const ivec4 &value): mType(VAR_UNKNOWN) {setValues(&value);}
    
    Variant(const mat2 &value): mType(VAR_UNKNOWN) {setValues(&value);}
    Variant(const mat3 &value): mType(VAR_UNKNOWN) {setValues(&value);}
    Variant(const mat4 &value): mType(VAR_UNKNOWN) {setValues(&value);}
    
    virtual ~Variant() {}
    
    void resize(size_t width, size_t height = 1)
    {
      mWidth = width;
      size_t size = width * height * mTypeSize;
      if (mData.size() < size)
        mData.resize(size);
    }
    void resize(VAR_TYPE type, size_t width, size_t height = 1)
    {
      mType = type;
      mTypeSize = GetTypeSize(mType);
      resize(width, height);
    }
    void setValues(VAR_TYPE type, const void *ptr, size_t width = 1, size_t height = 1)
    {
      resize(type, width, height);
      memcpy(&mData.at(0), ptr, sizeInBytes());
    }
    void clearData() {mData.clear();}
    
    void setValues(const float *ptr, size_t width = 1, size_t height = 1) {setValues(VAR_FLOAT,   (const void*)ptr, width, height);}
    void setValues(const vec2  *ptr, size_t width = 1, size_t height = 1) {setValues(VAR_FLOAT_2, (const void*)&ptr->x, width, height);}
    void setValues(const vec3  *ptr, size_t width = 1, size_t height = 1) {setValues(VAR_FLOAT_3, (const void*)&ptr->x, width, height);}
    void setValues(const vec4  *ptr, size_t width = 1, size_t height = 1) {setValues(VAR_FLOAT_4, (const void*)&ptr->x, width, height);}
    
    void setValues(const RGBA *ptr, size_t width = 1, size_t height = 1) {setValues(VAR_CHAR_4, (const void*)&ptr->red, width, height);}
    void setValues(const RGBAf *ptr, size_t width = 1, size_t height = 1) {setValues(VAR_FLOAT_4, (const void*)&ptr->red, width, height);}
    
    void setValues(const int   *ptr, size_t width = 1, size_t height = 1) {setValues(VAR_INT,   (const void*)ptr, width, height);}
    void setValues(const ivec2 *ptr, size_t width = 1, size_t height = 1) {setValues(VAR_INT_2, (const void*)&ptr->x, width, height);}
    void setValues(const ivec3 *ptr, size_t width = 1, size_t height = 1) {setValues(VAR_INT_3, (const void*)&ptr->x, width, height);}
    void setValues(const ivec4 *ptr, size_t width = 1, size_t height = 1) {setValues(VAR_INT_4, (const void*)&ptr->x, width, height);}
    
    void setValues(const mat2 *ptr, size_t width = 1, size_t height = 1) {setValues(VAR_MTX_2X2, (const void*)&ptr->x.x, width, height);}
    void setValues(const mat3 *ptr, size_t width = 1, size_t height = 1) {setValues(VAR_MTX_3X3, (const void*)&ptr->x.x, width, height);}
    void setValues(const mat4 *ptr, size_t width = 1, size_t height = 1) {setValues(VAR_MTX_4X4, (const void*)&ptr->x.x, width, height);}
    
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
    
    Variant& operator=(const XMLTree::Node *node) {setToXml(node); return *this;}
    Variant& operator=(const XMLTree::Node &node) {setToXml(node); return *this;}
    
    bool operator==(const Variant &other) const
    {
      if (mType != other.mType || mData.size() != other.mData.size())
        return false;
      return mData.size() == 0 || !memcmp(&mData[0], &other.mData[0], mData.size());
    }
    bool operator!=(const Variant &other) const {return !(*this == other);}
    
    VAR_TYPE type() const {return mType;}
    bool isIntType() const {return mType == VAR_CHAR_4 || mType == VAR_INT || mType == VAR_INT_2 || mType == VAR_INT_3 || mType == VAR_INT_4;}
    bool isFloatType() const {return !isIntType();}
    
    size_t typeSize() const {return mTypeSize;}
    size_t size() const {return mData.size()/mTypeSize;}
    size_t width() const {return mWidth;}
    size_t height() const {return size()/mWidth;}
    size_t sizeInBytes() const {return mData.size();}
    
    void* ptr() {return mData.size() ? &mData.at(0) : nullptr;}
    const void* ptr() const {return mData.size() ? &mData.at(0) : nullptr;}
    
    uint8_t& operator[](int index) {return mData.at(index * mTypeSize);}
    const uint8_t& operator[](int index) const {return mData.at(index * mTypeSize);}
    
    float& floatAt(int x = 0, int y = 0) {return (float&)mData.at((x+y*mWidth)*GetTypeSize(VAR_FLOAT));}
    vec2&  vec2At(int x = 0, int y = 0)  {return (vec2&)mData.at((x+y*mWidth)*GetTypeSize(VAR_FLOAT_2));}
    vec3&  vec3At(int x = 0, int y = 0)  {return (vec3&)mData.at((x+y*mWidth)*GetTypeSize(VAR_FLOAT_3));}
    vec4&  vec4At(int x = 0, int y = 0)  {return (vec4&)mData.at((x+y*mWidth)*GetTypeSize(VAR_FLOAT_4));}
    
    RGBA& RGBAAt(int x = 0, int y = 0) {return (RGBA&)mData.at((x+y*mWidth)*GetTypeSize(VAR_CHAR_4));}
    RGBAf& RGBAfAt(int x = 0, int y = 0) {return (RGBAf&)mData.at((x+y*mWidth)*GetTypeSize(VAR_FLOAT_4));}
    
    int&   intAt(int x = 0, int y = 0)   {return (int&)mData.at((x+y*mWidth)*GetTypeSize(VAR_INT));}
    ivec2& ivec2At(int x = 0, int y = 0) {return (ivec2&)mData.at((x+y*mWidth)*GetTypeSize(VAR_INT_2));}
    ivec3& ivec3At(int x = 0, int y = 0) {return (ivec3&)mData.at((x+y*mWidth)*GetTypeSize(VAR_INT_3));}
    ivec4& ivec4At(int x = 0, int y = 0) {return (ivec4&)mData.at((x+y*mWidth)*GetTypeSize(VAR_INT_4));}
    
    mat2& mat2At(int x = 0, int y = 0) {return (mat2&)mData.at((x+y*mWidth)*GetTypeSize(VAR_MTX_2X2));}
    mat3& mat3At(int x = 0, int y = 0) {return (mat3&)mData.at((x+y*mWidth)*GetTypeSize(VAR_MTX_3X3));}
    mat4& mat4At(int x = 0, int y = 0) {return (mat4&)mData.at((x+y*mWidth)*GetTypeSize(VAR_MTX_4X4));}
    
    float floatValue(int x = 0, int y = 0) const {return isIntType() ? (float)intValue(x, y) : (const float&)mData.at((x+y*mWidth)*GetTypeSize(VAR_FLOAT));}
    vec2 vec2Value(int x = 0, int y = 0) const {return isIntType() ? vec2(ivec2Value(x, y)) : (const vec2&)mData.at((x+y*mWidth)*GetTypeSize(VAR_FLOAT_2));}
    vec3 vec3Value(int x = 0, int y = 0) const {return isIntType() ? vec3(ivec3Value(x, y)) : (const vec3&)mData.at((x+y*mWidth)*GetTypeSize(VAR_FLOAT_3));}
    vec4 vec4Value(int x = 0, int y = 0) const {return isIntType() ? vec4(ivec4Value(x, y)) : (const vec4&)mData.at((x+y*mWidth)*GetTypeSize(VAR_FLOAT_4));}
    RGBA RGBAValue(int x = 0, int y = 0) const {return (const RGBA&)mData.at((x+y*mWidth)*GetTypeSize(VAR_CHAR_4));}
    RGBAf RGBAfValue(int x = 0, int y = 0) const
    {
      if (isIntType())
      {
        vec4 color = vec4(ivec4Value(x, y));
        return RGBAf(color.x, color.y, color.z, color.w);
      }
      return *((const RGBAf*)&mData.at((x+y*mWidth)*GetTypeSize(VAR_FLOAT_4)));
    }
    
    int intValue(int x = 0, int y = 0) const {return isIntType() ? (const int&)mData.at((x+y*mWidth)*GetTypeSize(VAR_INT)) : (int)floatValue(x, y);}
    ivec2 ivec2Value(int x = 0, int y = 0) const {return isIntType() ? (const ivec2&)mData.at((x+y*mWidth)*GetTypeSize(VAR_INT_2)) : ivec2(vec2Value(x, y));}
    ivec3 ivec3Value(int x = 0, int y = 0) const {return isIntType() ? (const ivec3&)mData.at((x+y*mWidth)*GetTypeSize(VAR_INT_3)) : ivec3(vec3Value(x, y));}
    ivec4 ivec4Value(int x = 0, int y = 0) const {return isIntType() ? (const ivec4&)mData.at((x+y*mWidth)*GetTypeSize(VAR_INT_4)) : ivec4(vec4Value(x, y));}
    
    mat2 mat2Value(int x = 0, int y = 0) const {return (const mat2&)mData.at((x+y*mWidth)*GetTypeSize(VAR_MTX_2X2));}
    mat3 mat3Value(int x = 0, int y = 0) const {return (const mat3&)mData.at((x+y*mWidth)*GetTypeSize(VAR_MTX_3X3));}
    mat4 mat4Value(int x = 0, int y = 0) const {return (const mat4&)mData.at((x+y*mWidth)*GetTypeSize(VAR_MTX_4X4));}
    
    operator float() const {return floatValue();}
    operator int()   const {return intValue();}
    
    operator vec2() const {return vec2Value();}
    operator vec3() const {return vec3Value();}
    operator vec4() const {return vec4Value();}
    
    operator RGBA() const {return RGBAValue();}
    operator RGBAf() const {return RGBAfValue();}
    
    operator ivec2() const {return ivec2Value();}
    operator ivec3() const {return ivec3Value();}
    operator ivec4() const {return ivec4Value();}
    
    operator mat2() const {return mat2Value();}
    operator mat3() const {return mat3Value();}
    operator mat4() const {return mat4Value();}
    
    bool setToXml(const XMLTree::Node *node) {return node && setToXml(*node);}
    bool setToXml(const XMLTree::Node &node)
    {
      // TODO: Update to handle arrays.
      return parse(node.attribute("type"), node.contents());
    }
    
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
    
    void invert()
    {
      if (mData.size() > 0)
      {
        size_t rowSize = mWidth * mTypeSize;
        std::vector<uint8_t> temp(rowSize);
        
        uint8_t *ptrA = &mData.at(0);
        uint8_t *ptrB = ptrA + rowSize*(height()-1);
        while (ptrA < ptrB)
        {
          memcpy(&temp[0], ptrA, rowSize);
          memcpy(ptrA, ptrB, rowSize);
          memcpy(ptrB, &temp[0], rowSize);
          ptrA += rowSize;
          ptrB -= rowSize;
        }
      }
    }
    
  private:
    VAR_TYPE mType;
    size_t mTypeSize;
    size_t mWidth;
    std::vector<uint8_t> mData;
  };
  
  typedef std::map<std::string, Variant> VariantMap;
  typedef std::list<std::pair<std::string, Variant> > VariantList;
}

#endif /* Variant_h */
