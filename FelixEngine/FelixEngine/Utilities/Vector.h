//
//  Vector.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/15/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Vector_h
#define Vector_h

#include <cmath>
#include <iostream>
#include <string>

#define CHAR_BUFF_SIZE 128

namespace fx
{
  const float Pi = 4 * std::atan(1.0f);
  const float TwoPi = 2 * Pi;
  const float RadToDeg = 180.0f / Pi;
  const float DegToRad = Pi / 180.0f;
  
  template <typename T>
  struct Vector2
  {
    Vector2(): x(0), y(0) {}
    Vector2(T x, T y): x(x), y(y) {}
    Vector2(const std::string &str): x(0), y(0) {parse(str);}
    
    template <typename I>
    Vector2(const Vector2<I> &v): x(v.x), y(v.y) {}
    
    template <typename I>
    void operator=(const Vector2<I> v) {x = v.x; y = v.y;}
    
    Vector2 yx() const {return Vector2(y, x);}
    T dot(const Vector2& v) const {return x * v.x + y * v.y;}
    Vector2 max(const Vector2 &v) const
    {
      Vector2 vec;
      vec.x = x < v.x ? v.x : x;
      vec.y = y < v.y ? v.y : y;
      return vec;
    }
    Vector2 min(const Vector2 &v) const
    {
      Vector2 vec;
      vec.x = x < v.x ? x : v.x;
      vec.y = y < v.y ? y : v.y;
      return vec;
    }
    Vector2 clamp(const Vector2 &v1, const Vector2 &v2) const {return min(v1.max(v2)).max(v1.min(v2));}
    
    static Vector2 Max(const Vector2 &v1, const Vector2 &v2) {return v1.max(v2);}
    static Vector2 Min(const Vector2 &v1, const Vector2 &v2) {return v1.min(v2);}
    static Vector2 Clamp(const Vector2 &v, const Vector2 &v1, const Vector2 &v2) {return v.clamp(v1, v2);}
    
    Vector2 mod() const
    {
      Vector2 vec;
      vec.x = x<0 ? 1+(x-(long)x) : 1<x ? x-(long)x : x;
      vec.y = y<0 ? 1+(y-(long)y) : 1<y ? y-(long)y : y;
      return vec;
    }
    T* ptr() {return &x;}
    const T* ptr() const {return &x;}
    
    T& operator[](int index) {return (&x)[index];}
    const T& operator[](int index) const {return (&x)[index];}
    
    Vector2 operator+(const Vector2& v) const {return Vector2(x + v.x, y + v.y);}
    Vector2 operator-(const Vector2& v) const {return Vector2(x - v.x, y - v.y);}
    Vector2 operator*(const Vector2& v) const {return Vector2(x * v.x, y * v.y);}
    Vector2 operator/(const Vector2& v) const {return Vector2(v.x?x/v.x:v.x, v.y?y/v.y:v.y);}
    
    Vector2 operator*(T s) const {return Vector2(x * s, y * s);}
    Vector2 operator/(T s) const
    {
      if (s)
        return Vector2(x / s, y / s);
      std::cerr << "divide by zero" << std::endl;
      return *this;
    }
    
    void operator+=(const Vector2& v) {*this = *this + v;}
    void operator-=(const Vector2& v) {*this = *this - v;}
    void operator*=(const Vector2& v) {*this = *this * v;}
    void operator/=(const Vector2& v) {*this = *this / v;}
    
    void operator*=(T s) {*this = *this * s;}
    void operator/=(T s) {*this = *this / s;}
    
    T lengthSquared()    const {return x * x + y * y;}
    T length()           const {return sqrt(lengthSquared());}
    Vector2 normalized() const {return *this / length();}
    
    void normalize() {this = *this / length();}
    
    bool operator==(const Vector2& v) const {return x == v.x && y == v.y;}
    bool operator!=(const Vector2& v) const {return x != v.x || y != v.y;}
    bool operator<=(const Vector2& v) const {return x <= v.x && y <= v.y;}
    bool operator>=(const Vector2& v) const {return v <= *this;}
    
    bool operator<(const Vector2& v) const {return x < v.x && y < v.y;}
    bool operator>(const Vector2& v) const {return v < *this;}
    
    Vector2 lerp(T t, const Vector2& v) const {return *this * (1 - t) + v * t;}
    
    T distanceSquared(const Vector2& v) const {return (*this - v).LengthSquared();}
    T distance(const Vector2& v) const {return (*this - v).Length();}
    
    bool parse(const std::string &str)
    {
      float fx = 0.0f, fy = 0.0f;
      int res = sscanf(str.c_str(), " %f , %f", &fx, &fy);
      
      x = (T)fx;
      y = (T)fy;
      return res = 2;
    }
    
    friend std::ostream &operator<<(std::ostream &os, const Vector2 &v) {return os << "<" << v.x << ", " << v.y << ">";}
    
    static std::string ToString(const Vector2<int> &v)
    {
      char buffer[CHAR_BUFF_SIZE];
      snprintf(buffer, CHAR_BUFF_SIZE, "%i,%i", v.x, v.y);
      return std::string(buffer);
    }
    
    static bool IsPowerOfTwo(T val)
    {
      unsigned int iVal = val;
      return !(val - (T)iVal) && (iVal != 0) && !(iVal & (iVal - 1));
    }
    bool isPowerOfTwo() const {return IsPowerOfTwo(x) && IsPowerOfTwo(y);}
    
    static Vector2 Random() {return Vector2((T)rand(), (T)rand());}
    static Vector2 RandMax() {return Vector2((T)RAND_MAX, (T)RAND_MAX);}
    
    union {T x; T r; T w; T start; T width;};
    union {T y; T g; T h; T end; T height;};
  };
  
  
  template <typename T>
  struct Vector3 {
    Vector3(): x(0), y(0), z(0) {}
    Vector3(T x, T y, T z): x(x), y(y), z(z) {}
    Vector3(const Vector2<T> &v, T z): x(v.x), y(v.y), z(z) {}
    Vector3(T x, const Vector2<T> &v): x(x), y(v.y), z(v.z) {}
    Vector3(const std::string &str): x(0), y(0), z(0) {parse(str);}
    
    template <typename I>
    Vector3(const Vector3<I> &v): x(v.x), y(v.y), z(v.z) {}
    
    template <typename I>
    void operator=(const Vector3<I> v) {x = v.x; y = v.y; z = v.z;}
    
    Vector2<T> xy() const {return Vector2<T>(x, y);}
    Vector2<T> yx() const {return Vector2<T>(y, x);}
    Vector3 zyx() const {return Vector3(z, y, x);}
    
    T dot(const Vector3& v) const {return x * v.x + y * v.y + z * v.z;}
    Vector3 cross(const Vector3& v) const {return Vector3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);}
    Vector3 max(const Vector3 &v) const
    {
      Vector3 vec;
      vec.x = x < v.x ? v.x : x;
      vec.y = y < v.y ? v.y : y;
      vec.z = z < v.z ? v.z : z;
      return vec;
    }
    Vector3 min(const Vector3 &v) const
    {
      Vector3 vec;
      vec.x = x < v.x ? x : v.x;
      vec.y = y < v.y ? y : v.y;
      vec.z = z < v.z ? z : v.z;
      return vec;
    }
    Vector3 clamp(const Vector3 &v1, const Vector3 &v2) const {return min(v1.max(v2)).max(v1.min(v2));}
    Vector3 orthogonal() const {
      if (abs(x) < abs(y))
        return cross(abs(x) < abs(z) ? Vector3(1, 0, 0) : Vector3(0, 0, 1));
      return cross(abs(y) < abs(z) ? Vector3(0, 1, 0) : Vector3(0, 0, 1));
    }
    
    static Vector3 Max(const Vector3 &v1, const Vector3 &v2) {return v1.max(v2);}
    static Vector3 Min(const Vector3 &v1, const Vector3 &v2) {return v1.min(v2);}
    static Vector3 Clamp(const Vector3 &v, const Vector3 &v1, const Vector3 &v2) {return v.clamp(v1, v2);}
    
    T* ptr() {return &x;}
    const T* ptr() const {return &x;}
    
    T& operator[](int index) {return (&x)[index];}
    const T& operator[](int index) const {return (&x)[index];}
    
    Vector3 operator+(const Vector3& v) const {return Vector3(x + v.x, y + v.y, z + v.z);}
    Vector3 operator-(const Vector3& v) const {return Vector3(x - v.x, y - v.y, z - v.z);}
    Vector3 operator*(const Vector3& v) const {return Vector3(x * v.x, y * v.y, z * v.z);}
    Vector3 operator/(const Vector3& v) const {return Vector3(v.x?x/v.x:v.x, v.y?y/v.y:v.y, v.z?y/v.z:v.z);}
    
    Vector3 operator*(T s) const {return Vector3(x * s, y * s, z * s);}
    Vector3 operator/(T s) const
    {
      if (s)
        return Vector3(x / s, y / s, z / s);
      std::cerr << "divide by zero" << std::endl;
      return *this;
    }
    Vector3 operator-() const {return Vector3(-x, -y, -z);}
    
    void operator+=(const Vector3& v) {*this = *this + v;}
    void operator-=(const Vector3& v) {*this = *this - v;}
    void operator*=(const Vector3& v) {*this = *this * v;}
    void operator/=(const Vector3& v) {*this = *this / v;}
    
    void operator*=(T s) {*this = *this * s;}
    void operator/=(T s) {*this = *this / s;}
    
    T lengthSquared()    const {return x * x + y * y + z * z;}
    T length()           const {return sqrt(lengthSquared());}
    Vector3 normalized() const {return *this / length();}
    
    void normalize() {*this /= length();}
    
    bool operator==(const Vector3& v) const {return x == v.x && y == v.y && z == v.z;}
    bool operator!=(const Vector3& v) const {return x != v.x || y != v.y || z != v.z;}
    bool operator<=(const Vector3& v) const {return x <= v.x && y <= v.y && z <= v.z;}
    bool operator>=(const Vector3& v) const {return v <= *this;}
    
    bool operator<(const Vector3& v) const {return x < v.x && y < v.y && z < v.z;}
    bool operator>(const Vector3& v) const {return v < *this;}
    
    Vector3 lerp(T t, const Vector3& v) const {return *this * (1 - t) + v * t;}
    
    bool parse(const std::string &str)
    {
      float fx = 0.0f, fy = 0.0f, fz = 0.0f;
      int res = sscanf(str.c_str(), " %f , %f , %f", &fx, &fy, &fz);
      
      x = (T)fx;
      y = (T)fy;
      z = (T)fz;
      return res = 3;
    }
    
    static Vector3 Random() {return Vector3((T)rand(), (T)rand(), (T)rand());}
    static Vector3 RandMax() {return Vector3((T)RAND_MAX, (T)RAND_MAX, (T)RAND_MAX);}
    
    friend std::ostream &operator<<(std::ostream &os, const Vector3 &v)
    {
      return os << "<" << v.x << ", " << v.y << ", " << v.z << ">";
    }
    
    union {T x; T r;};
    union {T y; T g;};
    union {T z; T b;};
  };
  
  template <typename T>
  struct Vector4 {
    Vector4(): x(0), y(0), z(0), w(0) {}
    Vector4(T x, T y, T z, T w): x(x), y(y), z(z), w(w) {}
    Vector4(const std::string &str): x(0), y(0), z(0), w(0) {parse(str);}
    
    Vector4(const Vector2<T> &v, T z, T w): x(v.x), y(v.y), z(z), w(w) {}
    Vector4(T x, const Vector2<T> &v, T w): x(x), y(v.y), z(v.z), w(w) {}
    Vector4(T x, T y, const Vector2<T> &v): x(x), y(y), z(v.z), w(v.w) {}
    Vector4(const Vector2<T> &v1, const Vector2<T> &v2): x(v1.x), y(v1.y), z(v2.x), w(v2.y) {}
    
    Vector4(const Vector3<T> &v, T w): x(v.x), y(v.y), z(v.z), w(w) {}
    Vector4(T x, const Vector3<T> &v): x(x), y(v.x), z(v.y), w(v.z) {}
    
    template <typename I>
    Vector4(const Vector4<I> &v): x(v.x), y(v.y), z(v.z), w(v.w) {}
    
    template <typename I>
    void operator=(const Vector4<I> v) {x = v.x; y = v.y; z = v.z; w = v.w;}
    
    Vector2<T> xy() const {return Vector2<T>(x, y);}
    Vector2<T> yx() const {return Vector2<T>(y, x);}
    Vector3<T> xyz() const {return Vector3<T>(x, y, z);}
    Vector3<T> zyx() const {return Vector3<T>(z, y, x);}
    Vector3<T> yzw() const {return Vector3<T>(y, z, w);}
    Vector4 wzyx() const {return Vector4(w, z, y, x);}
    T dot(const Vector4& v) const {return x * v.x + y * v.y + z * v.z + w * v.w;}
    
    Vector4 max(const Vector4 &v) const
    {
      Vector4 vec;
      vec.x = x < v.x ? v.x : x;
      vec.y = y < v.y ? v.y : y;
      vec.z = z < v.z ? v.z : z;
      vec.w = w < v.w ? v.w : w;
      return vec;
    }
    Vector4 min(const Vector4 &v) const
    {
      Vector4 vec;
      vec.x = x < v.x ? x : v.x;
      vec.y = y < v.y ? y : v.y;
      vec.z = z < v.z ? y : v.z;
      vec.w = w < v.w ? w : v.w;
      return vec;
    }
    Vector4 clamp(const Vector4 &v1, const Vector4 &v2) const {return min(v1.max(v2)).max(v1.min(v2));}
    
    static Vector4 Max(const Vector4 &v1, const Vector4 &v2) {return v1.max(v2);}
    static Vector4 Min(const Vector4 &v1, const Vector4 &v2) {return v1.min(v2);}
    static Vector4 Clamp(const Vector4 &v, const Vector4 &v1, const Vector4 &v2) {return v.clamp(v1, v2);}
    
    T* ptr() {return &x;}
    const T* ptr() const {return &x;}
    
    T& operator[](int index) {return (&x)[index];}
    const T& operator[](int index) const {return (&x)[index];}
    
    Vector4 operator+(const Vector4& v) const {return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);}
    Vector4 operator-(const Vector4& v) const {return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);}
    Vector4 operator*(const Vector4& v) const {return Vector4(x * v.x, y * v.y, z * v.z, w * v.w);}
    Vector4 operator/(const Vector4& v) const {return Vector4(v.x?x/v.x:v.x, v.y?y/v.y:v.y, v.z?z/v.z:v.z, v.w?w/v.w:v.w);}
    
    Vector4 operator*(T s) const {return Vector4(x * s, y * s, z * s, w * s);}
    Vector4 operator/(T s) const
    {
      if (s)
        return Vector4(x / s, y / s, z / s, w / s);
      std::cerr << "divide by zero" << std::endl;
      return *this;
    }
    
    void operator+=(const Vector4& v) {*this = *this + v;}
    void operator-=(const Vector4& v) {*this = *this - v;}
    void operator*=(const Vector4& v) {*this = *this * v;}
    void operator/=(const Vector4& v) {*this = *this / v;}
    
    void operator*=(T s) {*this = *this * s;}
    void operator/=(T s) {*this = *this / s;}
    
    T lengthSquared()    const {return x * x + y * y + z * z + w * w;}
    T length()           const {return sqrt(lengthSquared());}
    Vector4 normalized() const {return *this / length();}
    
    void normalize() {this = *this / length();}
    void swap(Vector4 &other)
    {
      Vector4 tmp = *this;
      *this = other;
      other = tmp;
    }
    
    bool operator==(const Vector4& v) const {return x == v.x && y == v.y && z == v.z && w == v.w;}
    bool operator!=(const Vector4& v) const {return x != v.x || y != v.y || z != v.z || w != v.w;}
    bool operator<=(const Vector4& v) const {return x <= v.x && y <= v.y && z <= v.z && w <= v.w;}
    bool operator>=(const Vector4& v) const {return v <= *this;}
    
    bool operator<(const Vector4& v) const {return x < v.x && y < v.y && z < v.z && w < v.w;}
    bool operator>(const Vector4& v) const {return v < *this;}
    
    Vector4 lerp(T t, const Vector4& v) const {return *this * (1 - t) + v * t;}
    
    bool parse(const std::string &str)
    {
      float fx = 0.0f, fy = 0.0f, fz = 0.0f, fw = 0.0f;
      int res = sscanf(str.c_str(), " %f , %f , %f , %f", &fx, &fy, &fz, &fw);
      
      x = (T)fx;
      y = (T)fy;
      z = (T)fz;
      w = (T)fw;
      return res = 4;
    }
    
    friend std::ostream &operator<<(std::ostream &os, const Vector4 &v)
    {
      return os << "<" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ">";
    }
    
    static std::string ToString(const Vector4<float> &v)
    {
      char buffer[CHAR_BUFF_SIZE];
      snprintf(buffer, CHAR_BUFF_SIZE, "%.4f,%.4f,%.4f,%.4f", v.x, v.y, v.z, v.w);
      return std::string(buffer);
    }
    
    static Vector4 Random() {return Vector4((T)rand(), (T)rand(), (T)rand(), (T)rand());}
    static Vector4 RandMax() {return Vector4((T)RAND_MAX, (T)RAND_MAX, (T)RAND_MAX, (T)RAND_MAX);}
    
    union {T x; T r;};
    union {T y; T g;};
    union {T z; T b;};
    union {T w; T a;};
  };
  
  
  typedef Vector2<int> ivec2;
  typedef Vector3<int> ivec3;
  typedef Vector4<int> ivec4;
  
  typedef Vector2<float> vec2;
  typedef Vector3<float> vec3;
  typedef Vector4<float> vec4;
  
  typedef Vector2<float> float2;
  typedef Vector3<float> float3;
  typedef Vector4<float> float4;
  
  typedef Vector4<unsigned char> RGBA;
  typedef Vector4<float> RGBAf;
  
  typedef Vector2<unsigned int> Range;
}

#endif /* Vector_h */
