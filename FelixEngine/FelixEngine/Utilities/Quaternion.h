//
//  Quaternion.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/15/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Quaternion_h
#define Quaternion_h

#include "Matrix.h"


namespace fx
{
  template <typename T>
  struct Quaternion
  {
    Quaternion(): x(0), y(0), z(0), w(1) {}
    Quaternion(T x, T y, T z, T w): x(x), y(y), z(z), w(w) {normalize();}
    Quaternion(const std::string &str): x(0), y(0), z(0), w(1) {parse(str);}
    Quaternion(const Vector4<T> &v): x(v.x), y(v.y), z(v.z), w(v.w) {normalize();}
    Quaternion(const Vector3<T> &v): x(v.x), y(v.y), z(v.z)
    {
      w = 1.0 - (x*x) - (y*y) - (z*z);
      w = w < 0 ? 0 : -std::sqrt(w);
      normalize();
    }
    Quaternion(Vector3<T> axis, T r)
    {
      axis.normalize();
      w = std::cos(r / 2);
      x = y = z = std::sin(r / 2);
      x *= axis.x;
      y *= axis.y;
      z *= axis.z;
      normalize();
    }
    Quaternion(const Vector3<T> &v0, const Vector3<T> &v1): x(0), y(0), z(0), w(1)
    {
      Vector3<T> u = v0.normalized();
      Vector3<T> v = v1.normalized();
      if (u == -v) {
        // 180 degree rotation around any orthogonal vector
        Vector3<T> orth = u.orthogonal().normalized();
        x = orth.x;
        y = orth.y;
        z = orth.z;
        w = 0;
        normalize();
      }
      else if (u != v) {
        Vector3<T> half = (u + v).normalized();
        Vector3<T> cross = u.cross(half);
        x = cross.x;
        y = cross.y;
        z = cross.z;
        w = u.dot(half);
        normalize();
      }
    }
    Quaternion(const mat4 &m) {setTo(m);}
    Quaternion(const mat3 &m) {setTo(m);}
    
    void setTo(const mat4 &m) {setTo(m.toMat3());}
    void setTo(const mat3 &m) {
      w = sqrt(1.0 + m.x.x + m.y.y + m.z.z)/2.0;
      x = (m.z.y - m.y.z)/(4.0*w);
      y = (m.x.z - m.z.x)/(4.0*w);
      z = (m.y.x - m.x.y)/(4.0*w);
      normalize();
    }
    
    Vector3<T> toVec3() const {return Vector3<T>(x, y, z);}
    Vector4<T> toVec4() const {return Vector4<T>(x, y, z, w);}
    
    T Dot(const Quaternion &q) const {return x * q.x + y * q.y + z * q.z + w * q.w;}
    
    Quaternion operator+(const Quaternion &q) const {return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w);}
    Quaternion operator-(const Quaternion &q) const {return Quaternion(x - q.x, y - q.y, z - q.z, w - q.w);}
    Quaternion operator*(const Quaternion &q) const
    {
      Quaternion ret;
      ret.w = w * q.w - x * q.x - y * q.y - z * q.z;
      ret.x = w * q.x + x * q.w + y * q.z - z * q.y;
      ret.y = w * q.y + y * q.w + z * q.x - x * q.z;
      ret.z = w * q.z + z * q.w + x * q.y - y * q.x;
      ret.normalize();
      return ret;
    }
    Quaternion operator*(T s) const {return Quaternion(x*s, y*s, z*s, w*s);}
    Vector3<T> operator*(const Vector3<T> &v) const
    {
      Quaternion<T> ret = *this * Quaternion<T>(v.x, v.y, v.z, 1) * conjugate();
      return vec3(ret.x, ret.y, ret.z);
    }
    
    void operator+=(const Quaternion &q) {*this = *this + q;}
    void operator-=(const Quaternion &q) {*this = *this - q;}
    void operator*=(const Quaternion &q) {*this = *this * q;}
    void operator*=(T s) {x *= s; y *= s; z *= s; w *= s;}
    
    void rotate(const Vector3<T> &v)
    {
      if (v.x != 0)
        *this *= Quaternion(vec3(1, 0, 0), v.x);
      if (v.y != 0)
        *this *= Quaternion(vec3(0, 1, 0), v.y);
      if (v.z != 0)
        *this *= Quaternion(vec3(0, 0, 1), v.z);
    }
    
    T lengthSquared() const {return x * x + y * y + z * z + w * w;}
    T length()        const {return sqrt(lengthSquared());}
    
    Quaternion normalized() const {return *this * (1.0/length());}
    Quaternion conjugate()  const
    {
      Quaternion q(-x, -y, -z, w);
      return q.normalized();
    }
    T norm() const {return x*x + y*y + z*z + w*w;}
    Quaternion scale(T s) const {return Quaternion(x*s, y*s, z*s, w*s);}
    Quaternion inverse() const {return conjugate().scale(1.0/norm());}
    
    void normalize() {*this *= (1.0/length());}
    
    bool operator==(const Quaternion &q) const {return x == q.x && y == q.y && z == q.z && w == q.w;}
    bool operator<(const Quaternion &q) const
    {
      return x == q.x ? y == q.y ? z == q.z ? w < q.w : z < q.z : y < q.y : x < q.x;
    }
    Quaternion lerp(T t, const Quaternion &q) const
    {
      Quaternion result = q + (*this - q) * t;
      return result.Normalized();
    }
    Quaternion slerp(T t, const Quaternion &q) const
    {
      const T epsilon = 0.0005f;
      Quaternion result, q2;
      T theta, dot = Dot(q);
      
      if (dot > 1 - epsilon) {
        result = q + (*this - q) * t;
        return result.Normalized();
      }
      
      dot = dot < 0 ? 0 : dot > 1 ? 1 : dot;
      
      theta = std::acos(dot);
      theta *= t;
      
      q2 = (q - *this * dot);
      q2.Normalize();
      
      result = *this * std::cos(theta) + q2 * std::sin(theta);
      return result.Normalized();
    }
    bool parse(const std::string &str)
    {
      float fx = 0.0f, fy = 0.0f, fz = 0.0f, fw = 1.0f;
      int res = sscanf(str.c_str(), " %f , %f , %f , %f", &fx, &fy, &fz, &fw);
      x = (T)fx;
      y = (T)fy;
      z = (T)fz;
      w = (T)fw;
      normalize();
      return res = 4;
    }
    Matrix3<T> toMat3() const
    {
      T s = 2;
      Matrix3<T> m;
      
      T xs, ys, zs;
      T wx, wy, wz;
      T xx, xy, xz;
      T yy, yz, zz;
      xs = x * s;  ys = y * s;  zs = z * s;
      wx = w * xs; wy = w * ys; wz = w * zs;
      xx = x * xs; xy = x * ys; xz = x * zs;
      yy = y * ys; yz = y * zs; zz = z * zs;
      
      m.x = Vector3<T>(1 - (yy + zz), xy + wz, xz - wy);
      m.y = Vector3<T>(xy - wz, 1 - (xx + zz), yz + wx);
      m.z = Vector3<T>(xz + wy, yz - wx, 1 - (xx + yy));
      return m;
    }
    Matrix4<T> toMat4() const {return Matrix4<T>(toMat3());}
    
    friend std::ostream &operator<<(std::ostream &os, const Quaternion &q)
    {
      return os << "(" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ")";
    }
    
    static Quaternion RotX(T r) {
      return Quaternion(vec3(1, 0, 0), r);
    }
    static Quaternion RotY(T r) {
      return Quaternion(vec3(0, 1, 0), r);
    }
    static Quaternion RotZ(T r) {
      return Quaternion(vec3(0, 0, 1), r);
    }
    
    static Quaternion ParseFloat(const std::string &str)
    {
      Quaternion q;
      sscanf(str.c_str(), " %f , %f , %f , %f", &q.x, &q.y, &q.z, &q.w);
      return q;
    }
    
    T x;
    T y;
    T z;
    T w;
  };
  
  typedef Quaternion<float> quat;
}

#endif /* Quaternion_h */
