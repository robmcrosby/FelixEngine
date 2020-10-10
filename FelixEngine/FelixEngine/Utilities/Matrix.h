//
//  Matrix.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/15/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Matrix_h
#define Matrix_h

#include <FelixEngine/Vector.h>

#define SWAP_ROWS(a, b) {float *tmp = a; a = b; b = tmp;}


namespace fx
{
  template <typename T>
  struct Matrix2
  {
    Matrix2(): x(1, 0), y(0, 1) {}
    Matrix2(const T* m)
    {
      x.x = m[0]; x.y = m[1];
      y.x = m[2]; y.y = m[3];
    }
    template <typename I>
    Matrix2(const Matrix2<I> &m): x(m.x), y(m.y) {}
    
    T determinant() const {return x.x * y.y - x.y * y.x;}
    Matrix2 inverse() const
    {
      Matrix2 m = *this;
      T det = determinant();
      if (det != static_cast<T>(0))
      {
        det = static_cast<T>(1)/det;
        m.x.x =  y.y * det;  m.x.y = -x.y * det;
        m.y.x = -y.x * det;  m.y.y =  x.x * det;
      }
      else
        std::cerr << "Cannot Invert Matrix: " << std::endl << *this;
      return m;
    }
    Matrix2 transposed() const
    {
      Matrix2 m = *this;
      m.x.y = y.x;
      m.y.x = x.y;
      return m;
    }
    
    Matrix2 scaled(T s) const
    {
      Matrix2 m = *this;
      m.x.x *= s;
      m.y.y *= s;
      return m;
    }
    template <typename I>
    Matrix2 scaled(const Vector2<I> &s) const
    {
      Matrix2 m = *this;
      m.x.x *= s.x;
      m.y.y *= s.y;
      return m;
    }
    Matrix2 operator*(const Matrix2 &m) const
    {
      Matrix2 ret;
      ret.x.x = x.x*m.x.x + y.x*m.x.y;
      ret.y.x = x.x*m.y.x + y.x*m.y.y;
      ret.x.y = x.y*m.x.x + y.y*m.x.y;
      ret.y.y = x.y*m.y.x + y.y*m.y.y;
      return ret;
    }
    template <typename I>
    Vector2<I> operator*(const Vector2<I> &b) const
    {
      Vector2<I> v;
      v.x = x.x*b.x + y.x*b.y;
      v.y = x.y*b.x + y.y*b.y;
      return v;
    }
    bool operator==(const Matrix2 &m) const {return x == m.x && y == m.y;}
    bool operator<(const Matrix2 &m) const {return x == m.x ? y < m.y : x < m.x;}
    void operator*=(const Matrix2 &m) {*this = m * *this;}
    T* ptr() {return x.ptr();}
    const T* ptr() const {return x.ptr();}
    
    Vector2<T>& operator[](int index) {return (&x)[index];}
    const Vector2<T>& operator[](int index) const {return (&x)[index];}
    
    friend std::ostream &operator<<(std::ostream &os, const Matrix2 &m)
    {
      os << "[" << m.x.x << ", " << m.y.x << "]" << std::endl;
      os << "[" << m.x.y << ", " << m.y.y << "]" << std::endl;
      return os;
    }
    
    Vector2<T> x;
    Vector2<T> y;
  };
  
  template <typename T>
  struct Matrix3
  {
    Matrix3(): x(1, 0, 0), y(0, 1, 0), z(0, 0, 1) {}
    Matrix3(const T* m)
    {
      x.x = m[0]; x.y = m[1]; x.z = m[2];
      y.x = m[3]; y.y = m[4]; y.z = m[5];
      z.x = m[6]; z.y = m[7]; z.z = m[8];
    }
    template <typename I>
    Matrix3(const Matrix2<I> &m): x(m.x, 0), y(m.y, 0), z(0, 0, 1) {}
    template <typename I>
    Matrix3(const Matrix3<I> &m): x(m.x), y(m.y), z(m.z) {}
    
    T determinant() const
    {
      T det = x.x * y.y * z.z;
      det +=  y.x * z.y * x.z;
      det +=  z.x * x.y * y.z;
      det -=  x.x * z.y * y.z;
      det -=  z.x * y.y * x.z;
      det -=  y.x * x.y * z.z;
      return det;
    }
    Matrix3 inverse() const
    {
      Matrix3 m = *this;
      T det = determinant();
      if (det != static_cast<T>(0))
      {
        det = static_cast<T>(1)/det;
        m.x.x = (y.y * z.z - y.z * z.y) * det;
        m.x.y = (x.z * z.y - x.y * z.z) * det;
        m.x.z = (x.y * y.z - x.z * y.y) * det;
        m.y.x = (y.z * z.x - y.x * z.z) * det;
        m.y.y = (x.x * z.z - x.z * z.x) * det;
        m.y.z = (x.z * y.x - x.x * y.z) * det;
        m.z.x = (y.x * z.y - y.y * z.x) * det;
        m.z.y = (x.y * z.x - x.x * z.y) * det;
        m.z.z = (x.x * y.y - x.y * y.x) * det;
      }
      else
        std::cerr << "Cannot Invert Matrix: " << std::endl << *this;
      return m;
    }
    Matrix3 transposed() const
    {
      Matrix3 m = *this;
      m.x.y = y.x;
      m.x.z = z.x;
      m.y.x = x.y;
      m.y.z = z.y;
      m.z.x = x.z;
      m.z.y = y.z;
      return m;
    }
    Matrix3 scaled(T s) const
    {
      Matrix3 m = *this;
      m.x.x *= s;
      m.y.y *= s;
      m.z.z *= s;
      return m;
    }
    template <typename I>
    Matrix3 scaled(const Vector3<I> &s) const
    {
      Matrix3 m = *this;
      m.x.x *= s.x;
      m.y.y *= s.y;
      m.z.z *= s.z;
      return m;
    }
    Matrix2<T> toMat2() const
    {
      Matrix2<T> m;
      m.x = x.XY();
      m.y = y.XY();
      return m;
    }
    Matrix3 operator*(const Matrix3 &m) const
    {
      Matrix3 ret;
      ret.x.x = x.x*m.x.x + y.x*m.x.y + z.x*m.x.z;
      ret.y.x = x.x*m.y.x + y.x*m.y.y + z.x*m.y.z;
      ret.z.x = x.x*m.z.x + y.x*m.z.y + z.x*m.z.z;
      
      ret.x.y = x.y*m.x.x + y.y*m.x.y + z.y*m.x.z;
      ret.y.y = x.y*m.y.x + y.y*m.y.y + z.y*m.y.z;
      ret.z.y = x.y*m.z.x + y.y*m.z.y + z.y*m.z.z;
      
      ret.x.z = x.z*m.x.x + y.z*m.x.y + z.z*m.x.z;
      ret.y.z = x.z*m.y.x + y.z*m.y.y + z.z*m.y.z;
      ret.z.z = x.z*m.z.x + y.z*m.z.y + z.z*m.z.z;
      return ret;
    }
    template <typename I>
    Vector3<I> operator*(const Vector3<I> &b) const
    {
      Vector3<I> v;
      v.x = x.x*b.x + y.x*b.y + z.x*b.z;
      v.y = x.y*b.x + y.y*b.y + z.y*b.z;
      v.z = x.z*b.x + y.z*b.y + z.z*b.z;
      return v;
    }
    template <typename I>
    Vector2<I> operator*(const Vector2<I> &b) const
    {
      Vector2<I> v;
      v.x = x.x*b.x + y.x*b.y + z.x;
      v.y = x.y*b.x + y.y*b.y + z.y;
      return v;
    }
    bool operator==(const Matrix3 &m) const {return x == m.x && y == m.y && z == m.z;}
    bool operator<(const Matrix3 &m) const {return x == m.x ? y == m.y ? z < m.z : y < m.y : x < m.x;}
    void operator*=(const Matrix3 &m) {*this = m * *this;}
    T* ptr() {return x.ptr();}
    const T* ptr() const {return x.ptr();}
    
    Vector3<T>& operator[](int index) {return (&x)[index];}
    const Vector3<T>& operator[](int index) const {return (&x)[index];}
    
    static Matrix3 RotX(T r)
    {
      Matrix3 m;
      m.y.y = m.z.z = cos(r);
      m.y.z = sin(r);
      m.z.y = -m.y.z;
      return m;
    }
    static Matrix3 RotY(T r)
    {
      Matrix3 m;
      m.x.x = m.z.z = cos(r);
      m.z.x = sin(r);
      m.x.z = -m.z.x;
      return m;
    }
    static Matrix3 RotZ(T r)
    {
      Matrix3 m;
      m.x.x = m.y.y = cos(r);
      m.x.y = sin(r);
      m.y.x = -m.x.y;
      return m;
    }
    static Matrix3 Rotate(Vector3<T> r) {return RotZ(r.z) * RotY(r.y) * RotX(r.x);}
    static Matrix3 Trans2d(Vector2<T> t)
    {
      Matrix3 m;
      m.z = Vector3<T>(t, 1);
      return m;
    }
    static Matrix3 Scale(Vector2<T> s)
    {
      Matrix3 m;
      m.x.x = s.x;
      m.y.y = s.y;
      return m;
    }
    static Matrix3 Scale(Vector3<T> s)
    {
      Matrix3 m;
      m.x.x = s.x;
      m.y.y = s.y;
      m.z.z = s.z;
      return m;
    }
    static Matrix3<T> Orthoginalize(const Vector3<T> &fwd, const Vector3<T> &up)
    {
      vec3 f = fwd.normalized();
      vec3 s = up.cross(f).normalized();
      vec3 u = f.cross(s);
      
      Matrix3 m;
      m.x.x = s.x; m.y.x = f.x; m.z.x = u.x;
      m.x.y = s.y; m.y.y = f.y; m.z.y = u.y;
      m.x.z = s.z; m.y.z = f.z; m.z.z = u.z;
      return m;
    }
    
    friend std::ostream &operator<<(std::ostream &os, const Matrix3 &m)
    {
      os << "[" << m.x.x << ", " << m.y.x << ", " << m.z.x << "]" << std::endl;
      os << "[" << m.x.y << ", " << m.y.y << ", " << m.z.y << "]" << std::endl;
      os << "[" << m.x.z << ", " << m.y.z << ", " << m.z.z << "]" << std::endl;
      return os;
    }
    
    Vector3<T> x;
    Vector3<T> y;
    Vector3<T> z;
  };
  
  template <typename T>
  struct Matrix4 {
    Matrix4(): x(1, 0, 0, 0), y(0, 1, 0, 0), z(0, 0, 1, 0), w(0, 0, 0, 1) {}
    Matrix4(const T* m)
    {
      x.x = m[0]; x.y = m[1]; x.z = m[2]; x.w = m[3];
      y.x = m[4]; y.y = m[5]; y.z = m[6]; y.w = m[7];
      z.x = m[8]; z.y = m[9]; z.z = m[10]; z.w = m[11];
      w.x = m[12]; w.y = m[13]; w.z = m[14]; w.w = m[15];
    }
    template <typename I>
    Matrix4(const Matrix2<I> &m): x(m.x, 0, 0), y(m.y, 0, 0), z(0, 0, 1, 0), w(0, 0, 0, 1) {}
    template <typename I>
    Matrix4(const Matrix3<I> &m): x(m.x, 0), y(m.y, 0), z(m.z, 0), w(0, 0, 0, 1) {}
    template <typename I>
    Matrix4(const Matrix4<I> &m): x(m.x), y(m.y), z(m.z) {}
    
    Matrix4 transposed() const
    {
      Matrix4 m = *this;
      m.x.y = y.x;
      m.x.z = z.x;
      m.x.w = w.x;
      
      m.y.x = x.y;
      m.y.z = z.y;
      m.y.w = w.y;
      
      m.z.x = x.z;
      m.z.y = y.z;
      m.z.w = w.z;
      
      m.w.x = x.w;
      m.w.y = y.w;
      m.w.z = z.w;
      return m;
    }
    
    T determinant() const
    {
      T det = x.x * y.y * z.z * w.w + x.x * y.z * z.w * w.y + x.x * y.w * z.y * w.z;
      det  += x.y * y.x * z.w * w.z + x.y * y.z * z.x * w.w + x.y * y.w * z.z * w.x;
      det  += x.z * y.x * z.y * w.w + x.z * y.y * z.w * w.x + x.z * y.w * z.x * w.y;
      det  += x.w * y.x * z.z * w.y + x.w * y.y * z.x * w.z + x.w * y.z * z.y * w.x;
      
      det -= x.x * y.y * z.w * w.z + x.x * y.z * z.y * w.w + x.x * y.w * z.z * w.y;
      det -= x.y * y.x * z.z * w.w + x.y * y.z * z.w * w.x + x.y * y.w * z.x * w.z;
      det -= x.z * y.x * z.w * w.y + x.z * y.y * z.x * w.w + x.z * y.w * z.y * w.x;
      det -= x.w * y.x * z.y * w.z + x.w * y.y * z.z * w.x + x.w * y.z * z.x * w.y;
      return det;
    }
    Matrix4 inverse() const
    {
      Matrix4 m = *this;
      T det = determinant();
      if (det != static_cast<T>(0))
      {
        det = static_cast<T>(1)/det;
        m.x.x = (y.y * z.z * w.w + y.z * z.w * w.y + y.w * z.y * w.z - y.y * z.w * w.z - y.z * z.y * w.w - y.w * z.z * w.y) * det;
        m.x.y = (x.y * z.w * w.z + x.z * z.y * w.w + x.w * z.z * w.y - x.y * z.z * w.w - x.z * z.w * w.y - x.w * z.y * w.z) * det;
        m.x.z = (x.y * y.z * w.w + x.z * y.w * w.y + x.w * y.y * w.z - x.y * y.w * w.z - x.z * y.y * w.w - x.w * y.z * w.y) * det;
        m.x.w = (x.y * y.w * z.z + x.z * y.y * z.w + x.w * y.z * z.y - x.y * y.z * z.w - x.z * y.w * z.y - x.w * y.y * z.z) * det;
        
        m.y.x = (y.x * z.w * w.z + y.z * z.x * w.w + y.w * z.z * w.x - y.x * z.z * w.w - y.z * z.w * w.x - y.w * z.x * w.z) * det;
        m.y.y = (x.x * z.z * w.w + x.z * z.w * w.x + x.w * z.x * w.z - x.x * z.w * w.z - x.z * z.x * w.w - x.w * z.z * w.x) * det;
        m.y.z = (x.x * y.w * w.z + x.z * y.x * w.w + x.w * y.z * w.x - x.x * y.z * w.w - x.z * y.w * w.x - x.w * y.x * w.z) * det;
        m.y.w = (x.x * y.z * z.w + x.z * y.w * z.x + x.w * y.x * z.z - x.x * y.w * z.z - x.z * y.x * z.w - x.w * y.z * z.x) * det;
        
        m.z.x = (y.x * z.y * w.w + y.y * z.w * w.x + y.w * z.x * w.y - y.x * z.w * w.y - y.y * z.x * w.w - y.w * z.y * w.x) * det;
        m.z.y = (x.x * z.w * w.y + x.y * z.x * w.w + x.w * z.y * w.x - x.x * z.y * w.w - x.y * z.w * w.x - x.w * z.x * w.y) * det;
        m.z.z = (x.x * y.y * w.w + x.y * y.w * w.x + x.w * y.x * w.y - x.x * y.w * w.y - x.y * y.x * w.w - x.w * y.y * w.x) * det;
        m.z.w = (x.x * y.w * z.y + x.y * y.x * z.w + x.w * y.y * z.x - x.x * y.y * z.w - x.y * y.w * z.x - x.w * y.x * z.y) * det;
        
        m.w.x = (y.x * z.z * w.y + y.y * z.x * w.z + y.z * z.y * w.x - y.x * z.y * w.z - y.y * z.z * w.x - y.z * z.x * w.y) * det;
        m.w.y = (x.x * z.y * w.z + x.y * z.z * w.x + x.z * z.x * w.y - x.x * z.z * w.y - x.y * z.x * w.z - x.z * z.y * w.x) * det;
        m.w.z = (x.x * y.z * w.y + x.y * y.x * w.z + x.z * y.y * w.x - x.x * y.y * w.z - x.y * y.z * w.x - x.z * y.x * w.y) * det;
        m.w.w = (x.x * y.y * z.z + x.y * y.z * z.x + x.z * y.x * z.y - x.x * y.z * z.y - x.y * y.x * z.z - x.z * y.y * z.x) * det;
      }
      else
        std::cerr << "Cannot Invert Matrix: " << std::endl << *this;
      return m;
    }
    Matrix4 scaled(T s) const
    {
      Matrix4 m = *this;
      m.x.x *= s;
      m.y.y *= s;
      m.z.z *= s;
      return m;
    }
    template <typename I>
    Matrix4 scaled(const Vector3<I> &s) const
    {
      Matrix4 m = *this;
      m.x.x *= s.x;
      m.y.y *= s.y;
      m.z.z *= s.z;
      return m;
    }
    Matrix2<T> toMat2() const
    {
      Matrix2<T> m;
      m.x = x.XY();
      m.y = y.XY();
      return m;
    }
    Matrix3<T> toMat3() const
    {
      Matrix3<T> m;
      m.x = x.xyz();
      m.y = y.xyz();
      m.z = z.xyz();
      return m;
    }
    Matrix4 operator*(T s) const
    {
      Matrix4 ret = *this;
      ret.x *= s;
      ret.y *= s;
      ret.z *= s;
      ret.w *= s;
      return ret;
    }
    Matrix4 operator*(const Matrix4 &m) const
    {
      Matrix4 ret;
      ret.x.x = x.x*m.x.x + y.x*m.x.y + z.x*m.x.z + w.x*m.x.w;
      ret.y.x = x.x*m.y.x + y.x*m.y.y + z.x*m.y.z + w.x*m.y.w;
      ret.z.x = x.x*m.z.x + y.x*m.z.y + z.x*m.z.z + w.x*m.z.w;
      ret.w.x = x.x*m.w.x + y.x*m.w.y + z.x*m.w.z + w.x*m.w.w;
      
      ret.x.y = x.y*m.x.x + y.y*m.x.y + z.y*m.x.z + w.y*m.x.w;
      ret.y.y = x.y*m.y.x + y.y*m.y.y + z.y*m.y.z + w.y*m.y.w;
      ret.z.y = x.y*m.z.x + y.y*m.z.y + z.y*m.z.z + w.y*m.z.w;
      ret.w.y = x.y*m.w.x + y.y*m.w.y + z.y*m.w.z + w.y*m.w.w;
      
      ret.x.z = x.z*m.x.x + y.z*m.x.y + z.z*m.x.z + w.z*m.x.w;
      ret.y.z = x.z*m.y.x + y.z*m.y.y + z.z*m.y.z + w.z*m.y.w;
      ret.z.z = x.z*m.z.x + y.z*m.z.y + z.z*m.z.z + w.z*m.z.w;
      ret.w.z = x.z*m.w.x + y.z*m.w.y + z.z*m.w.z + w.z*m.w.w;
      
      ret.x.w = x.w*m.x.x + y.w*m.x.y + z.w*m.x.z + w.w*m.x.w;
      ret.y.w = x.w*m.y.x + y.w*m.y.y + z.w*m.y.z + w.w*m.y.w;
      ret.z.w = x.w*m.z.x + y.w*m.z.y + z.w*m.z.z + w.w*m.z.w;
      ret.w.w = x.w*m.w.x + y.w*m.w.y + z.w*m.w.z + w.w*m.w.w;
      return ret;
    }
    template <typename I>
    Vector4<I> operator*(const Vector4<I> &b) const
    {
      Vector4<I> v;
      v.x = x.x*b.x + y.x*b.y + z.x*b.z + w.x*b.w;
      v.y = x.y*b.x + y.y*b.y + z.y*b.z + w.y*b.w;
      v.z = x.z*b.x + y.z*b.y + z.z*b.z + w.z*b.w;
      v.w = x.w*b.x + y.w*b.y + z.w*b.z + w.w*b.w;
      return v;
    }
    template <typename I>
    Vector3<I> operator*(const Vector3<I> &b) const
    {
      Vector4<I> v = *this * Vector4<I>(b, 1);
      return v.xyz()/v.w;
    }
    bool operator==(const Matrix4 &m) const {return x == m.x && y == m.y && z == m.z && w == m.w;}
    bool operator<(const Matrix4 &m) const {return x == m.x ? y == m.y ? z == m.z ? w < m.w : z < m.z : y < m.y : x < m.x;}
    void operator*=(const Matrix4 &m) {*this = m * *this;}
    T* ptr() {return x.ptr();}
    const T* ptr() const {return x.ptr();}
    
    Vector4<T>& operator[](int index) {return (&x)[index];}
    const Vector4<T>& operator[](int index) const {return (&x)[index];}
    
    static Matrix4 RotX(T r)
    {
      Matrix4 m;
      m.y.y = m.z.z = cos(r);
      m.y.z = sin(r);
      m.z.y = -m.y.z;
      return m;
    }
    static Matrix4 RotY(T r)
    {
      Matrix4 m;
      m.x.x = m.z.z = cos(r);
      m.z.x = sin(r);
      m.x.z = -m.z.x;
      return m;
    }
    static Matrix4 RotZ(T r)
    {
      Matrix4 m;
      m.x.x = m.y.y = cos(r);
      m.x.y = sin(r);
      m.y.x = -m.x.y;
      return m;
    }
    static Matrix4 Rotate(Vector3<T> r) {return RotZ(r.z) * RotY(r.y) * RotX(r.x);}
    
    static Matrix4 Trans3d(Vector3<T> t)
    {
      Matrix4 m;
      m.w = Vector4<T>(t, 1);
      return m;
    }
    static Matrix4 Scale(Vector2<T> s)
    {
      Matrix4 m;
      m.x.x = s.x;
      m.y.y = s.y;
      return m;
    }
    static Matrix4 Scale(Vector3<T> s)
    {
      Matrix4 m;
      m.x.x = s.x;
      m.y.y = s.y;
      m.z.z = s.z;
      return m;
    }
    static Matrix4 Scale(Vector4<T> s)
    {
      Matrix4 m;
      m.x.x = s.x;
      m.y.y = s.y;
      m.z.z = s.z;
      m.w.w = s.w;
      return m;
    }
    static Matrix4 Frustum(T left, T right, T bottom, T top, T near, T far)
    {
      T a = (2 * near) / (right - left);
      T b = (2 * near) / (top - bottom);
      T c = (right + left) / (right - left);
      T d = (top + bottom) / (top - bottom);
      T e = -(far + near) / (far - near);
      T f = (-2 * far * near) / (far - near);
      
      Matrix4 m;
      m.x = Vector4<T>(a, 0, 0, 0);
      m.y = Vector4<T>(0, b, 0, 0);
      m.z = Vector4<T>(c, d, e, -1);
      m.w = Vector4<T>(0, 0, f, 1);
      return m;
    }
    static Matrix4 Ortho(T left, T right, T bottom, T top, T near, T far)
    {
      T a = static_cast<T>(2) / (right - left);
      T b = static_cast<T>(2) / (top - bottom);
      T c = static_cast<T>(-2) / (far - near);
      T d = -(right + left) / (right - left);
      T e = -(top + bottom) / (top - bottom);
      T f = -(far + near) / (far - near);
      
      Matrix4 m;
      m.x = Vector4<T>(a, 0, 0, 0);
      m.y = Vector4<T>(0, b, 0, 0);
      m.z = Vector4<T>(0, 0, c, 0);
      m.w = Vector4<T>(d, e, f, 1);
      return m;
    }
    static Matrix4<T> Orthoginalize(const Vector3<T> &fwd, const Vector3<T> &up)
    {
      vec3 f = fwd.normalized();
      vec3 s = fwd.cross(up).normalized();
      vec3 u = s.cross(f);
      
      Matrix4 m;
      m.x.x = s.x; m.y.x = u.x; m.z.x = f.x;
      m.x.y = s.y; m.y.y = u.y; m.z.y = f.y;
      m.x.z = s.z; m.y.z = u.z; m.z.z = f.z;
      return m;
    }
    static Matrix4<T> Magic(const Vector3<T> &fwd, const Vector3<T> &up, const Vector3<T> &pos)
    {
      return Trans3d(pos) * Orthoginalize(fwd, up);
    }
    static Matrix4<T> LookAt(const Vector3<T> &eye, const Vector3<T> &center, const Vector3<T> &up)
    {
      vec3 f = (center - eye).normalized();
      vec3 s = f.cross(up).normalized();
      vec3 u = s.cross(f);
      
      Matrix4 m;
      m.x.x = s.x; m.y.x = s.y; m.z.x = s.z;
      m.x.y = u.x; m.y.y = u.y; m.z.y = u.z;
      m.x.z = -f.x; m.y.z = -f.y; m.z.z = -f.z;
      
      return m * Trans3d(-eye);
    }
    
    friend std::ostream &operator<<(std::ostream &os, const Matrix4 &m)
    {
      os << "[" << m.x.x << ", " << m.y.x << ", " << m.z.x << ", " << m.w.x << "]" << std::endl;
      os << "[" << m.x.y << ", " << m.y.y << ", " << m.z.y << ", " << m.w.y << "]" << std::endl;
      os << "[" << m.x.z << ", " << m.y.z << ", " << m.z.z << ", " << m.w.z << "]" << std::endl;
      os << "[" << m.x.w << ", " << m.y.w << ", " << m.z.w << ", " << m.w.w << "]" << std::endl;
      return os;
    }
    
    Vector4<T> x;
    Vector4<T> y;
    Vector4<T> z;
    Vector4<T> w;
  };
  
  typedef Matrix2<float> mat2;
  typedef Matrix3<float> mat3;
  typedef Matrix4<float> mat4;
  
  typedef Matrix2<float> float2x2;
  typedef Matrix3<float> float3x3;
  typedef Matrix4<float> float4x4;
}

#endif /* Matrix_h */
