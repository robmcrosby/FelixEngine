//
//  Variant.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/23/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Variant_h
#define Variant_h

#include <vector>
#include <map>
#include <list>
#include <stdint.h>

#include "Vector.h"
#include "Matrix.h"

#define RGBA_STR "rgba"

#define FLOAT_STR "float"
#define VEC2_STR "vec2"
#define VEC3_STR "vec3"
#define VEC4_STR "vec4"

#define INT_STR "int"
#define IVEC2_STR "ivec2"
#define IVEC3_STR "ivec3"
#define IVEC4_STR "ivec4"

#define STRING_STR "string"
#define STRUCT_STR "struct"


namespace fx
{
  enum VARIANT_TYPE
  {
    VARIANT_FLOAT,
    VARIANT_FLOAT_2,
    VARIANT_FLOAT_3,
    VARIANT_FLOAT_4,
    
    VARIANT_CHAR_4,
    
    VARIANT_INT,
    VARIANT_INT_2,
    VARIANT_INT_3,
    VARIANT_INT_4,
    
    VARIANT_MTX_2X2,
    VARIANT_MTX_3X3,
    VARIANT_MTX_4X4,
    
    VARIANT_STRING,
    
    VARIANT_STRUCT,
    VARIANT_UNKNOWN,
  };
  
  class Variant
  {
  private:
    VARIANT_TYPE _type;
    size_t _typeSize;
    size_t _width;
    std::vector<uint8_t> _data;
    
  public:
    static size_t getTypeSize(VARIANT_TYPE type)
    {
      switch (type)
      {
        case VARIANT_FLOAT:
        case VARIANT_CHAR_4:
        case VARIANT_INT:
          return sizeof(uint32_t);
        case VARIANT_FLOAT_2:
        case VARIANT_INT_2:
          return 2*sizeof(uint32_t);
        case VARIANT_FLOAT_3:
        case VARIANT_INT_3:
          return 3*sizeof(uint32_t);
        case VARIANT_FLOAT_4:
        case VARIANT_INT_4:
        case VARIANT_MTX_2X2:
          return 4*sizeof(uint32_t);
        case VARIANT_MTX_3X3:
          return 9*sizeof(uint32_t);
        case VARIANT_MTX_4X4:
          return 16*sizeof(uint32_t);
        case VARIANT_STRING:
          return sizeof(uint8_t);
        case VARIANT_STRUCT:
        case VARIANT_UNKNOWN:
          return 0;
      }
      return 0;
    }
    
    static size_t getTypeComponents(VARIANT_TYPE type)
    {
      switch (type)
      {
        case VARIANT_FLOAT:
        case VARIANT_INT:
        case VARIANT_STRING:
          return 1;
        case VARIANT_FLOAT_2:
        case VARIANT_INT_2:
          return 2;
        case VARIANT_FLOAT_3:
        case VARIANT_INT_3:
          return 3;
        case VARIANT_FLOAT_4:
        case VARIANT_CHAR_4:
        case VARIANT_INT_4:
        case VARIANT_MTX_2X2:
          return 4;
        case VARIANT_MTX_3X3:
          return 9;
        case VARIANT_MTX_4X4:
          return 16;
        case VARIANT_STRUCT:
        case VARIANT_UNKNOWN:
          return 0;
      }
      return 0;
    }
    
    static VARIANT_TYPE getVariantType(const std::string &str)
    {
      if (str == RGBA_STR)
        return VARIANT_CHAR_4;
      if (str == FLOAT_STR)
        return VARIANT_FLOAT;
      if (str == VEC2_STR)
        return VARIANT_FLOAT_2;
      if (str == VEC3_STR)
        return VARIANT_FLOAT_3;
      if (str == VEC4_STR)
        return VARIANT_FLOAT_4;
      if (str == INT_STR)
        return VARIANT_INT;
      if (str == IVEC2_STR)
        return VARIANT_INT_2;
      if (str == IVEC3_STR)
        return VARIANT_INT_3;
      if (str == IVEC4_STR)
        return VARIANT_INT_4;
      if (str == STRING_STR)
        return VARIANT_STRING;
      if (str == STRUCT_STR)
        return VARIANT_STRUCT;
      return VARIANT_UNKNOWN;
    }
    
  public:
    Variant(VARIANT_TYPE type = VARIANT_UNKNOWN, size_t width = 1, size_t height = 1): _type(VARIANT_UNKNOWN) {resize(type, width, height);}
    Variant(const std::string &type, const std::string &str): _type(VARIANT_UNKNOWN) {parse(type, str);}
    
    Variant(const float &value): _type(VARIANT_UNKNOWN) {setValues(&value);}
    Variant(const vec2  &value): _type(VARIANT_UNKNOWN) {setValues(&value);}
    Variant(const vec3  &value): _type(VARIANT_UNKNOWN) {setValues(&value);}
    Variant(const vec4  &value): _type(VARIANT_UNKNOWN) {setValues(&value);}
    
    Variant(const RGBA &value): _type(VARIANT_UNKNOWN) {setValues(&value);}
    
    Variant(const int   &value): _type(VARIANT_UNKNOWN) {setValues(&value);}
    Variant(const ivec2 &value): _type(VARIANT_UNKNOWN) {setValues(&value);}
    Variant(const ivec3 &value): _type(VARIANT_UNKNOWN) {setValues(&value);}
    Variant(const ivec4 &value): _type(VARIANT_UNKNOWN) {setValues(&value);}
    
    Variant(const mat2 &value): _type(VARIANT_UNKNOWN) {setValues(&value);}
    Variant(const mat3 &value): _type(VARIANT_UNKNOWN) {setValues(&value);}
    Variant(const mat4 &value): _type(VARIANT_UNKNOWN) {setValues(&value);}
    
    Variant(const std::string &value): _type(VARIANT_UNKNOWN) {setValues(value.c_str());}
    Variant(const char *value): _type(VARIANT_UNKNOWN) {setValues(value);}
    
    virtual ~Variant() {}
    
    void resize(size_t width, size_t height = 1)
    {
      _width = width;
      size_t size = width * height * _typeSize;
      if (_data.size() < size)
        _data.resize(size);
    }
    void resize(VARIANT_TYPE type, size_t width, size_t height = 1)
    {
      _type = type;
      _typeSize = getTypeSize(_type);
      resize(width, height);
    }
    void clearData() {_data.clear();}
    
    void setValues(VARIANT_TYPE type, const void *ptr, size_t width = 1, size_t height = 1)
    {
      resize(type, width, height);
      memcpy(&_data.at(0), ptr, sizeInBytes());
    }
    
    void setValues(const float *ptr, size_t width = 1, size_t height = 1) {setValues(VARIANT_FLOAT,   ptr, width, height);}
    void setValues(const vec2  *ptr, size_t width = 1, size_t height = 1) {setValues(VARIANT_FLOAT_2, ptr->ptr(), width, height);}
    void setValues(const vec3  *ptr, size_t width = 1, size_t height = 1) {setValues(VARIANT_FLOAT_3, ptr->ptr(), width, height);}
    void setValues(const vec4  *ptr, size_t width = 1, size_t height = 1) {setValues(VARIANT_FLOAT_4, ptr->ptr(), width, height);}
    
    void setValues(const RGBA *ptr, size_t width = 1, size_t height = 1) {setValues(VARIANT_CHAR_4, ptr->ptr(), width, height);}
    
    void setValues(const int   *ptr, size_t width = 1, size_t height = 1) {setValues(VARIANT_INT,   ptr, width, height);}
    void setValues(const ivec2 *ptr, size_t width = 1, size_t height = 1) {setValues(VARIANT_INT_2, ptr->ptr(), width, height);}
    void setValues(const ivec3 *ptr, size_t width = 1, size_t height = 1) {setValues(VARIANT_INT_3, ptr->ptr(), width, height);}
    void setValues(const ivec4 *ptr, size_t width = 1, size_t height = 1) {setValues(VARIANT_INT_4, ptr->ptr(), width, height);}
    
    void setValues(const mat2 *ptr, size_t width = 1, size_t height = 1) {setValues(VARIANT_MTX_2X2, ptr->ptr(), width, height);}
    void setValues(const mat3 *ptr, size_t width = 1, size_t height = 1) {setValues(VARIANT_MTX_3X3, ptr->ptr(), width, height);}
    void setValues(const mat4 *ptr, size_t width = 1, size_t height = 1) {setValues(VARIANT_MTX_4X4, ptr->ptr(), width, height);}
    
    void setValues(const char *ptr) {setValues(VARIANT_STRING, ptr, strlen(ptr)+1);}
    
    template <typename T>
    void setValues(const T *ptr, size_t width = 1, size_t height = 1) {
      _type = VARIANT_STRUCT;
      _typeSize = sizeof(T);
      _width = width;
      _data.resize(_typeSize * width * height);
      memcpy(&_data.at(0), (const void *)ptr, sizeInBytes());
    }
    
    void append(VARIANT_TYPE type, const void *ptr)
    {
      resize(type, size()+1);
      memcpy(&_data.at(_data.size()-_typeSize), ptr, _typeSize);
    }
    void append(const float &value) {append(VARIANT_FLOAT, &value);}
    void append(const vec2  &value) {append(VARIANT_FLOAT_2, value.ptr());}
    void append(const vec3  &value) {append(VARIANT_FLOAT_3, value.ptr());}
    void append(const vec4  &value) {append(VARIANT_FLOAT_4, value.ptr());}
    
    void append(const RGBA  &value) {append(VARIANT_CHAR_4, value.ptr());}
    
    void append(const int   &value) {append(VARIANT_INT, &value);}
    void append(const ivec2 &value) {append(VARIANT_INT_2, value.ptr());}
    void append(const ivec3 &value) {append(VARIANT_INT_3, value.ptr());}
    void append(const ivec4 &value) {append(VARIANT_INT_4, value.ptr());}
    
    void append(const mat2 &value) {append(VARIANT_MTX_2X2, value.ptr());}
    void append(const mat3 &value) {append(VARIANT_MTX_3X3, value.ptr());}
    void append(const mat4 &value) {append(VARIANT_MTX_4X4, value.ptr());}
    
    Variant& operator=(const float &value) {setValues(&value); return *this;}
    Variant& operator=(const vec2  &value) {setValues(&value); return *this;}
    Variant& operator=(const vec3  &value) {setValues(&value); return *this;}
    Variant& operator=(const vec4  &value) {setValues(&value); return *this;}
    
    Variant& operator=(const int   &value) {setValues(&value); return *this;}
    Variant& operator=(const ivec2 &value) {setValues(&value); return *this;}
    Variant& operator=(const ivec3 &value) {setValues(&value); return *this;}
    Variant& operator=(const ivec4 &value) {setValues(&value); return *this;}
    
    Variant& operator=(const mat2 &value) {setValues(&value); return *this;}
    Variant& operator=(const mat3 &value) {setValues(&value); return *this;}
    Variant& operator=(const mat4 &value) {setValues(&value); return *this;}
    
    Variant& operator=(const std::string &value) {setValues(value.c_str()); return *this;}
    Variant& operator=(const char *value) {setValues(value); return *this;}
    
    template <typename T>
    Variant& operator=(const T &value) {setValues(&value); return *this;}
    
    template <typename T>
    Variant& operator=(const std::vector<T> &values) {
      
      setValues(&values.at(0), values.size());
      return *this;
    }
    
    bool operator==(const Variant &other) const
    {
      if (_type != other._type || _data.size() != other._data.size())
        return false;
      return _data.size() == 0 || !memcmp(&_data[0], &other._data[0], _data.size());
    }
    bool operator!=(const Variant &other) const {return !(*this == other);}
    
    VARIANT_TYPE type() const {return _type;}
    bool isIntType() const {return _type != VARIANT_STRING && (_type == VARIANT_CHAR_4 || _type == VARIANT_INT || _type == VARIANT_INT_2 || _type == VARIANT_INT_3 || _type == VARIANT_INT_4);}
    bool isFloatType() const {return _type != VARIANT_STRING && !isIntType();}
    bool isStringType() const {return _type == VARIANT_STRING;}
    
    size_t typeSize() const {return _typeSize;}
    size_t size() const {return _typeSize ? _data.size()/_typeSize : 0;}
    size_t width() const {return _width;}
    size_t height() const {return size()/_width;}
    size_t sizeInBytes() const {return _data.size();}
    size_t components() const {return getTypeComponents(_type);}
    
    void* ptr() {return _data.size() ? &_data.at(0) : nullptr;}
    const void* ptr() const {return _data.size() ? &_data.at(0) : nullptr;}
    
    uint8_t& operator[](int index) {return _data.at(index * _typeSize);}
    const uint8_t& operator[](int index) const {return _data.at(index * _typeSize);}
    
    float& floatAt(int x = 0, int y = 0) {return (float&)_data.at((x+y*_width)*getTypeSize(VARIANT_FLOAT));}
    vec2&  vec2At(int x = 0, int y = 0)  {return (vec2&)_data.at((x+y*_width)*getTypeSize(VARIANT_FLOAT_2));}
    vec3&  vec3At(int x = 0, int y = 0)  {return (vec3&)_data.at((x+y*_width)*getTypeSize(VARIANT_FLOAT_3));}
    vec4&  vec4At(int x = 0, int y = 0)  {return (vec4&)_data.at((x+y*_width)*getTypeSize(VARIANT_FLOAT_4));}
    
    RGBA& RGBAAt(int x = 0, int y = 0) {return (RGBA&)_data.at((x+y*_width)*getTypeSize(VARIANT_CHAR_4));}
    RGBAf& RGBAfAt(int x = 0, int y = 0) {return (RGBAf&)_data.at((x+y*_width)*getTypeSize(VARIANT_FLOAT_4));}
    
    int&   intAt(int x = 0, int y = 0)   {return (int&)_data.at((x+y*_width)*getTypeSize(VARIANT_INT));}
    ivec2& ivec2At(int x = 0, int y = 0) {return (ivec2&)_data.at((x+y*_width)*getTypeSize(VARIANT_INT_2));}
    ivec3& ivec3At(int x = 0, int y = 0) {return (ivec3&)_data.at((x+y*_width)*getTypeSize(VARIANT_INT_3));}
    ivec4& ivec4At(int x = 0, int y = 0) {return (ivec4&)_data.at((x+y*_width)*getTypeSize(VARIANT_INT_4));}
    
    mat2& mat2At(int x = 0, int y = 0) {return (mat2&)_data.at((x+y*_width)*getTypeSize(VARIANT_MTX_2X2));}
    mat3& mat3At(int x = 0, int y = 0) {return (mat3&)_data.at((x+y*_width)*getTypeSize(VARIANT_MTX_3X3));}
    mat4& mat4At(int x = 0, int y = 0) {return (mat4&)_data.at((x+y*_width)*getTypeSize(VARIANT_MTX_4X4));}
    
    char& charAt(int x = 0, int y = 0) {return (char&)_data.at((x+y*_width)*getTypeSize(VARIANT_STRING));}
    
    float floatValue(int x = 0, int y = 0) const {return isIntType() ? (float)intValue(x, y) : (const float&)_data.at((x+y*_width)*getTypeSize(VARIANT_FLOAT));}
    vec2 vec2Value(int x = 0, int y = 0) const {return isIntType() ? vec2(ivec2Value(x, y)) : (const vec2&)_data.at((x+y*_width)*getTypeSize(VARIANT_FLOAT_2));}
    vec3 vec3Value(int x = 0, int y = 0) const {return isIntType() ? vec3(ivec3Value(x, y)) : (const vec3&)_data.at((x+y*_width)*getTypeSize(VARIANT_FLOAT_3));}
    vec4 vec4Value(int x = 0, int y = 0) const {return isIntType() ? vec4(ivec4Value(x, y)) : (const vec4&)_data.at((x+y*_width)*getTypeSize(VARIANT_FLOAT_4));}
    RGBA RGBAValue(int x = 0, int y = 0) const {return (const RGBA&)_data.at((x+y*_width)*getTypeSize(VARIANT_CHAR_4));}
    RGBAf RGBAfValue(int x = 0, int y = 0) const
    {
      if (isIntType())
      {
        vec4 color = vec4(ivec4Value(x, y));
        return RGBAf(color.x, color.y, color.z, color.w);
      }
      return *((const RGBAf*)&_data.at((x+y*_width)*getTypeSize(VARIANT_FLOAT_4)));
    }
    
    int intValue(int x = 0, int y = 0) const {return isIntType() ? (const int&)_data.at((x+y*_width)*getTypeSize(VARIANT_INT)) : (int)floatValue(x, y);}
    ivec2 ivec2Value(int x = 0, int y = 0) const {return isIntType() ? (const ivec2&)_data.at((x+y*_width)*getTypeSize(VARIANT_INT_2)) : ivec2(vec2Value(x, y));}
    ivec3 ivec3Value(int x = 0, int y = 0) const {return isIntType() ? (const ivec3&)_data.at((x+y*_width)*getTypeSize(VARIANT_INT_3)) : ivec3(vec3Value(x, y));}
    ivec4 ivec4Value(int x = 0, int y = 0) const {return isIntType() ? (const ivec4&)_data.at((x+y*_width)*getTypeSize(VARIANT_INT_4)) : ivec4(vec4Value(x, y));}
    
    mat2 mat2Value(int x = 0, int y = 0) const {return (const mat2&)_data.at((x+y*_width)*getTypeSize(VARIANT_MTX_2X2));}
    mat3 mat3Value(int x = 0, int y = 0) const {return (const mat3&)_data.at((x+y*_width)*getTypeSize(VARIANT_MTX_3X3));}
    mat4 mat4Value(int x = 0, int y = 0) const {return (const mat4&)_data.at((x+y*_width)*getTypeSize(VARIANT_MTX_4X4));}
    
    char charValue(int x = 0, int y = 0) const {return (char&)_data.at((x+y*_width)*getTypeSize(VARIANT_STRING));}
    std::string stringValue() const {return (const char*)ptr();}
    
    operator float() const {return floatValue();}
    operator int()   const {return intValue();}
    
    operator vec2() const {return vec2Value();}
    operator vec3() const {return vec3Value();}
    operator vec4() const {return vec4Value();}
    
    operator RGBA() const {return RGBAValue();}
    
    operator ivec2() const {return ivec2Value();}
    operator ivec3() const {return ivec3Value();}
    operator ivec4() const {return ivec4Value();}
    
    operator mat2() const {return mat2Value();}
    operator mat3() const {return mat3Value();}
    operator mat4() const {return mat4Value();}
    
    operator std::string() const {return (const char*)ptr();}
    
    bool parse(const std::string &type, const std::string &str) {return parse(getVariantType(type), str);}
    bool parse(VARIANT_TYPE type, const std::string &str)
    {
      // TODO: Update to handle arrays.
      bool success = false;
      
      if (type == VARIANT_FLOAT)
      {
        float value(0.0f);
        success = sscanf(str.c_str(), " %f", &value) == 1;
        *this = value;
      }
      else if (type == VARIANT_FLOAT_2)
      {
        vec2 value(0.0f, 0.0f);
        success = value.parse(str);
        *this = value;
      }
      else if (type == VARIANT_FLOAT_3)
      {
        vec3 value(0.0f, 0.0f, 0.0f);
        success = value.parse(str);
        *this = value;
      }
      else if (type == VARIANT_FLOAT_4)
      {
        vec4 value(0.0f, 0.0f, 0.0f, 0.0f);
        success = value.parse(str);
        *this = value;
      }
      else if (type == VARIANT_INT)
      {
        int value(0);
        success = sscanf(str.c_str(), " %i", &value) == 1;
        *this = value;
      }
      else if (type == VARIANT_INT_2)
      {
        ivec2 value(0, 0);
        success = value.parse(str);
        *this = value;
      }
      else if (type == VARIANT_INT_3)
      {
        ivec3 value(0, 0, 0);
        success = value.parse(str);
        *this = value;
      }
      else if (type == VARIANT_INT_4)
      {
        ivec4 value(0, 0, 0, 0);
        success = value.parse(str);
        *this = value;
      }
      else
      {
        success = true;
        *this = str;
      }
      return success;
    }
    
    void invert()
    {
      if (_data.size() > 0)
      {
        size_t rowSize = _width * _typeSize;
        std::vector<uint8_t> temp(rowSize);
        
        uint8_t *ptrA = &_data.at(0);
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
  };
  
  typedef std::map<std::string, Variant> VariantMap;
  typedef std::list<std::pair<std::string, Variant> > VariantList;
}

#endif /* Variant_h */
