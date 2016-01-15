//
//  BufferMap.h
//  FelixEngine
//
//  Created by Robert Crosby on 1/6/16.
//  Copyright © 2016 Robert Crosby. All rights reserved.
//

#ifndef BufferMap_h
#define BufferMap_h

#include "Variant.h"
#include "Resource.h"


namespace fx
{
  /**
   * Defines what the Buffer would be used for.
   */
  enum BUFFER_TYPE
  {
    BUFFER_UNIFORM,   /**< Constant Uniform Value or Values */
    BUFFER_INSTANCED, /**< Constant Uniform Values for multiple Instances */
    BUFFER_VERTEX,    /**< Values for a Vertex Attribute in a Mesh */
    BUFFER_INDICES,   /**< Indices for a Mesh */
    BUFFER_RANGES,    /**< Ranges for Sub-Meshes in a Mesh */
    BUFFER_TEXTURE,   /**< Two dimensional image */
  };
  
  /**
   * Defines what a BufferMap would be used for.
   */
  enum BUFFER_MAP_TYPE
  {
    BUFFER_MAP_UNIFORMS, /**< Individual Uniforms */
    BUFFER_MAP_STRUCT,   /**< Values of a Uniform Struct */
    BUFFER_MAP_MESH,     /**< Parts for a Mesh */
    BUFFER_MAP_TEXTURES, /**< Collection of Textures */
  };
  
  class Buffer: public Variant
  {
  public:
    Buffer(BUFFER_TYPE bufType = BUFFER_UNIFORM, VAR_TYPE varType = VAR_UNKNOWN, size_t size = 1): Variant(varType, size), mBufferType(bufType), mResource(0) {}
    Buffer(const Variant &var): Variant(var), mBufferType(BUFFER_UNIFORM), mResource(0) {}
    Buffer(const Buffer &buf): Variant(buf), mName(buf.mName), mBufferType(buf.mBufferType), mResource(0) {setResource(buf.mResource);}
    Buffer(const std::string &type, const std::string &str): Variant(type, str), mBufferType(BUFFER_UNIFORM), mResource(0) {}
    
    Buffer(const XMLTree::Node *node): Variant(node), mBufferType(BUFFER_UNIFORM), mResource(0) {setToXml(node);}
    Buffer(const XMLTree::Node &node): Variant(node), mBufferType(BUFFER_UNIFORM), mResource(0) {setToXml(node);}
    
    Buffer(const float &value): Variant(value), mBufferType(BUFFER_UNIFORM) {}
    Buffer(const vec2  &value): Variant(value), mBufferType(BUFFER_UNIFORM) {}
    Buffer(const vec3  &value): Variant(value), mBufferType(BUFFER_UNIFORM) {}
    Buffer(const vec4  &value): Variant(value), mBufferType(BUFFER_UNIFORM) {}
    Buffer(const RGBAf &value): Variant(value), mBufferType(BUFFER_UNIFORM) {}
    
    Buffer(const int   &value): Variant(value), mBufferType(BUFFER_UNIFORM) {}
    Buffer(const ivec2 &value): Variant(value), mBufferType(BUFFER_UNIFORM) {}
    Buffer(const ivec3 &value): Variant(value), mBufferType(BUFFER_UNIFORM) {}
    Buffer(const ivec4 &value): Variant(value), mBufferType(BUFFER_UNIFORM) {}
    
    Buffer(const mat2 &value): Variant(value), mBufferType(BUFFER_UNIFORM) {}
    Buffer(const mat3 &value): Variant(value), mBufferType(BUFFER_UNIFORM) {}
    Buffer(const mat4 &value): Variant(value), mBufferType(BUFFER_UNIFORM) {}
    virtual ~Buffer() {setResource(nullptr);}
    
    Buffer& operator=(const Buffer &buffer)
    {
      Variant::operator=(buffer);
      mName = buffer.mName;
      mBufferType = buffer.mBufferType;
      setResource(buffer.mResource);
      return *this;
    }
    Buffer& operator=(const Variant &variant) {Variant::operator=(variant); return *this;}
    
    Buffer& operator=(const float &value) {Variant::operator=(value); return *this;}
    Buffer& operator=(const vec2  &value) {Variant::operator=(value); return *this;}
    Buffer& operator=(const vec3  &value) {Variant::operator=(value); return *this;}
    Buffer& operator=(const vec4  &value) {Variant::operator=(value); return *this;}
    Buffer& operator=(const RGBAf &value) {Variant::operator=(value); return *this;}
    
    Buffer& operator=(const int   &value) {Variant::operator=(value); return *this;}
    Buffer& operator=(const ivec2 &value) {Variant::operator=(value); return *this;}
    Buffer& operator=(const ivec3 &value) {Variant::operator=(value); return *this;}
    Buffer& operator=(const ivec4 &value) {Variant::operator=(value); return *this;}
    
    Buffer& operator=(const mat2 &value) {Variant::operator=(value); return *this;}
    Buffer& operator=(const mat3 &value) {Variant::operator=(value); return *this;}
    Buffer& operator=(const mat4 &value) {Variant::operator=(value); return *this;}
    
    Buffer& operator=(const XMLTree::Node *node) {setToXml(node); return *this;}
    Buffer& operator=(const XMLTree::Node &node) {setToXml(node); return *this;}
    
    bool setToXml(const XMLTree::Node *node) {return node && setToXml(*node);}
    bool setToXml(const XMLTree::Node &node)
    {
      Variant::setToXml(node);
      return true;
    }
    
    void setName(const std::string &name) {mName = name;}
    std::string name() const {return mName;}
    
    void setBufferType(BUFFER_TYPE type) {mBufferType = type;}
    BUFFER_TYPE bufferType() const {return mBufferType;}
    
    void setResource(Resource *r) {Resource::Replace(&mResource, r);}
    Resource* resource() const {return mResource;}
    
  private:
    std::string mName;
    BUFFER_TYPE mBufferType;
    Resource *mResource;
  };
  
  class BufferMap
  {
  public:
    BufferMap(BUFFER_MAP_TYPE type = BUFFER_MAP_UNIFORMS): mType(type), mResource(0), mFlags(0) {}
    BufferMap(const BufferMap &map): mType(BUFFER_MAP_UNIFORMS), mResource(0), mFlags(0) {*this = map;}
    ~BufferMap() {setResource(nullptr);}
    
    BufferMap& operator=(const BufferMap &map)
    {
      mName = map.mName;
      mType = map.mType;
      mFlags = map.mFlags;
      mBuffers = map.mBuffers;
      mNameMap = map.mNameMap;
      setResource(map.mResource);
      return *this;
    }
    
    typedef std::vector<Buffer>::iterator iterator;
    iterator begin() {return mBuffers.begin();}
    iterator end() {return mBuffers.end();}
    
    typedef std::vector<Buffer>::const_iterator const_iterator;
    const_iterator begin() const {return mBuffers.begin();}
    const_iterator end() const {return mBuffers.end();}
    
    void addBuffer(const Buffer &buffer = Buffer())
    {
      if (buffer.name() != "")
        mNameMap[buffer.name()] = (int)mBuffers.size();
      mBuffers.push_back(buffer);
    }
    void addBuffer(const std::string &name, const Buffer &buffer = Buffer())
    {
      mNameMap[name] = (int)mBuffers.size();
      mBuffers.push_back(buffer);
      mBuffers.back().setName(name);
    }
    
    void clear()
    {
      mBuffers.clear();
      mNameMap.clear();
    }
    
    Buffer& operator[](int index) {return mBuffers.at(index);}
    const Buffer& operator[](int index) const {return mBuffers.at(index);}
    
    Buffer& operator[](const std::string &name)
    {
      if (!mNameMap.count(name))
        addBuffer(name);
      return mBuffers.at(mNameMap.at(name));
    }
    const Buffer& operator[](const std::string &name) const {return mBuffers.at(mNameMap.at(name));}
    
    Buffer& getBuffer(const std::string &name, BUFFER_TYPE type)
    {
      if (!mNameMap.count(name))
        addBuffer(name, type);
      return mBuffers.at(mNameMap.at(name));
    }
    Buffer& getBuffer(BUFFER_TYPE type)
    {
      for (iterator itr = begin(); itr != end(); ++itr)
      {
        if (itr->bufferType() == type)
          return *itr;
      }
      addBuffer(type);
      return mBuffers.back();
    }
    
    bool contains(const std::string &name) const {return mNameMap.count(name);}
    bool contains(BUFFER_TYPE type) const
    {
      for (const_iterator itr = begin(); itr != end(); ++itr)
      {
        if (itr->bufferType() == type)
          return true;
      }
      return false;
    }
    
    void setName(const std::string &name) {mName = name;}
    std::string name() const {return mName;}
    
    void setType(BUFFER_MAP_TYPE type) {mType = type;}
    BUFFER_MAP_TYPE type() const {return mType;}
    
    void setResource(Resource *r) {Resource::Replace(&mResource, r);}
    void* resource() const {return mResource;}
    
    /**
     * Gets the combined size of all the buffers.
     */
    size_t sizeInBytes() const
    {
      size_t totalSize = 0;
      for (const_iterator itr = begin(); itr != end(); ++itr)
        totalSize += itr->sizeInBytes();
      return totalSize;
    }
    
    /**
     * Collects all of the data in the buffers to the provided data vector.
     */
    void combineBuffers(std::vector<uint8_t> &data) const
    {
      data.resize(sizeInBytes());
      if (data.size() > 0)
      {
        uint8_t *ptr = &data.at(0);
        for (const_iterator itr = begin(); itr != end(); ++itr)
        {
          memcpy(ptr, itr->ptr(), itr->sizeInBytes());
          ptr += itr->sizeInBytes();
        }
      }
    }
    
    /**
     * Clears the data from the buffers.
     */
    void clearBuffers()
    {
      for (iterator itr = begin(); itr != end(); ++itr)
        itr->clearData();
    }
    
    void setFlags(int f) {mFlags = f;}
    int flags() const {return mFlags;}
    
  private:
    std::vector<Buffer> mBuffers;
    std::map<std::string, int> mNameMap;
    
    std::string mName;
    BUFFER_MAP_TYPE mType;
    int mFlags;
    Resource *mResource;
  };
  typedef std::list<BufferMap*> BufferMapList;
  typedef std::map<std::string, BufferMap*> BufferMapDirectory;
}


#endif /* BufferMap_h */
