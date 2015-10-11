//
//  VertexBufferMap.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef VertexBufferMap_h
#define VertexBufferMap_h

#include <vector>
#include <map>

#include "Vector.h"


namespace fx
{
  /**
   *
   */
  enum VERTEX_PRIMITIVE
  {
    VERTEX_TRIANGLES,
    VERTEX_TRIANGLE_STRIP,
  };
  
  /**
   *
   */
  class VertexBuffer
  {
  public:
    VertexBuffer(): mComponents(1) {}
    
    void setComponents(int size) {mComponents = size;}
    void resize(int count) {mData.resize(mComponents*count);}
    
    void clear()
    {
      mData.clear();
      mComponents = 1;
    }
    
    float* operator[](int index) {return &mData.at(index*mComponents);}
    void append(float value) {mData.push_back(value);}
    void append(const vec2 &v)
    {
      mData.push_back(v.x);
      mData.push_back(v.y);
    }
    void append(const vec4 &v)
    {
      mData.push_back(v.x);
      mData.push_back(v.y);
      mData.push_back(v.z);
      mData.push_back(v.w);
    }
    
    int components() const {return mComponents;}
    int count() const {return (int)mData.size()/mComponents;}
    
    float* ptr() {return mData.size() ? &mData[0] : nullptr;}
    const float* ptr() const {return mData.size() ? &mData[0] : nullptr;}
    
    std::vector<float>& data() {return mData;}
    const std::vector<float>& data() const {return mData;}
    
  private:
    int mComponents;
    std::vector<float> mData;
  };
  
  /**
   *
   */
  struct Range
  {
    Range(int s = 0, int e = 0): start(s), end(e) {}
    int start;
    int end;
  };
  typedef std::vector<Range> Ranges;
  typedef std::vector<int> IndexBuffer;
  
  class VertexBufferMap
  {
  public:
    VertexBufferMap(): mPrimitiveType(VERTEX_TRIANGLES) {}
    
    VertexBuffer& operator[](const std::string &name) {return mMap[name];}
    
    typedef std::map<std::string, VertexBuffer>::iterator iterator;
    iterator begin() {return mMap.begin();}
    iterator end() {return mMap.end();}
    
    typedef std::map<std::string, VertexBuffer>::const_iterator const_iterator;
    const_iterator begin() const {return mMap.begin();}
    const_iterator end() const {return mMap.end();}
    
    void clear() {mMap.clear(); mRanges.clear();}
    
    void addRange(const Range &range) {mRanges.push_back(range);}
    void setPrimitiveType(VERTEX_PRIMITIVE type) {mPrimitiveType = type;}
    void setPrimitiveType(const std::string &type) {mPrimitiveType = GetPrimitiveType(type);}
    
    void setIndexBuffer(const IndexBuffer &buffer) {mIndexBuffer = buffer;}
    
    VERTEX_PRIMITIVE primitiveType() const {return mPrimitiveType;}
    
    IndexBuffer& indexBuffer() {return mIndexBuffer;}
    
    const Ranges& ranges() const {return mRanges;}
    const IndexBuffer& indexBuffer() const {return mIndexBuffer;}
    
    static VERTEX_PRIMITIVE GetPrimitiveType(const std::string &type)
    {
      if (type == "triangles")
        return VERTEX_TRIANGLES;
      if (type == "strip")
        return VERTEX_TRIANGLE_STRIP;
      
      std::cerr << "Unknown Primitive Type: " << type << std::endl;
      std::cerr << "Using Triangles Primitive for now" <<  std::endl;
      return VERTEX_TRIANGLES;
    }
    
  private:
    std::map<std::string, VertexBuffer> mMap;
    VERTEX_PRIMITIVE mPrimitiveType;
    
    Ranges      mRanges;
    IndexBuffer mIndexBuffer;
  };
}


#endif /* VertexBufferMap_h */
