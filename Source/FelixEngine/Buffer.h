//
//  Buffer.h
//  FelixEngine
//
//  Created by Robert Crosby on 1/6/16.
//  Copyright © 2016 Robert Crosby. All rights reserved.
//

#ifndef Buffer_h
#define Buffer_h

#include "Resource.h"
#include "Variant.h"
#include "IndexedMap.h"
#include "Color.h"


#define UNIFORMS_STR  "Uniforms"
#define UNIFORM_STR   "Uniform"
#define STRUCT_STR    "Struct"
#define INSTANCED_STR "Instanced"

#define MESH_STR      "Mesh"
#define VERTEX_STR    "Vertex"
#define INDICES_STR   "Indices"
#define RANGES_STR    "Ranges"

#define FRAME_STR     "Frame"
#define TARGET_STR    "Target"

#define PROGRAM_STR  "Program"
#define SHADER_STR   "Shader"

#define TEXTURE_STR   "Texture"


namespace fx
{
  class Buffer;
  
  /**
   * Defines what the Buffer would be used for.
   */
  enum BUFFER_TYPE
  {
    BUFFER_UNIFORM,   /**< Constant Uniform Value or Values */
    BUFFER_STRUCT,    /**< Constant Uniform Struct */
    BUFFER_INSTANCED, /**< Constant Uniform Values for multiple Instances */
    
    BUFFER_MESH,      /**< Collection of Buffers for a Mesh */
    BUFFER_VERTEX,    /**< Vertex Buffer */
    BUFFER_INDICES,   /**< Index Buffer */
    BUFFER_RANGES,    /**< Ranges for Sub-Meshes in a Mesh */
    
    BUFFER_FRAME,     /**< Frame Buffer with Render Targets */
    BUFFER_TARGET,    /**< Render Target */
    
    BUFFER_PROGRAM,   /**< Shader Program */
    BUFFER_SHADER,    /**< Shader Part */
    
    BUFFER_TEXTURES,  /**< Multiple Textures */
    BUFFER_TEXTURE,   /**< Texture */
  };
  
  
  static BUFFER_TYPE ParseBufferType(const std::string &str)
  {
    if (str == UNIFORM_STR)
      return BUFFER_UNIFORM;
    if (str == STRUCT_STR)
      return BUFFER_STRUCT;
    if (str == INSTANCED_STR)
      return BUFFER_INSTANCED;
    
    if (str == MESH_STR)
      return BUFFER_MESH;
    if (str == VERTEX_STR)
      return BUFFER_VERTEX;
    if (str == INDICES_STR)
      return BUFFER_INDICES;
    if (str == RANGES_STR)
      return BUFFER_RANGES;
    
    if (str == FRAME_STR)
      return BUFFER_FRAME;
    if (str == TARGET_STR)
      return BUFFER_TARGET;
    
    if (str == SHADER_STR)
      return BUFFER_PROGRAM;
    
    if (str == TEXTURE_STR)
      return BUFFER_TEXTURE;
    return BUFFER_UNIFORM;
  }
  
  
  class Buffer: public Variant
  {
  public:
    /**
     * Constructor
     */
    Buffer(BUFFER_TYPE bufType = BUFFER_UNIFORM, VAR_TYPE varType = VAR_UNKNOWN, size_t size = 1): Variant(varType, size),
      mBufferType(bufType),
      mResource(0),
      mFlags(0),
      mUpdated(true),
      mMap(0)
    {
    }
    
    /**
     * Constructor for a Resource
     */
    Buffer(Resource *resource, BUFFER_TYPE bufType, const std::string &name = ""):
      mName(name),
      mBufferType(bufType),
      mResource(0),
      mFlags(0),
      mUpdated(true),
      mMap(0)
    {
      setResource(resource);
    }
    
    /**
     * Constructor
     */
    Buffer(const Variant &var): Variant(var),
      mBufferType(BUFFER_UNIFORM),
      mResource(0),
      mFlags(0),
      mUpdated(true),
      mMap(0)
    {
    }
    
    /**
     * Constructor
     */
    Buffer(const std::string &type, const std::string &str): Variant(type, str),
      mBufferType(BUFFER_UNIFORM),
      mResource(0),
      mFlags(0),
      mUpdated(true),
      mMap(0)
    {
    }
    
    /**
     * XML Constructor
     */
    Buffer(const XMLTree::Node *node): Variant(node),
      mBufferType(BUFFER_UNIFORM),
      mResource(0),
      mFlags(0),
      mUpdated(true),
      mMap(0)
    {
      setToXml(node);
    }
    
    /**
     * XML Constructor
     */
    Buffer(const XMLTree::Node &node): Variant(node),
      mBufferType(BUFFER_UNIFORM),
      mResource(0),
      mFlags(0),
      mUpdated(true),
      mMap(0)
    {
      setToXml(node);
    }
    
    template <typename T>
    Buffer(const T &value): Variant(value),
      mBufferType(BUFFER_UNIFORM),
      mResource(0),
      mFlags(0),
      mUpdated(true),
      mMap(0)
    {
    }
    
    /**
     * Copy Constructor
     */
    Buffer(const Buffer &other):
      mResource(0),
      mUpdated(true),
      mMap(0)
    {
      *this = other;
    }
    
    virtual ~Buffer()
    {
      setResource(nullptr);
      delete mMap;
    }
    
    Buffer& operator=(const Buffer &buffer)
    {
      Variant::operator=(buffer);
      mName = buffer.mName;
      mBufferType = buffer.mBufferType;
      setResource(buffer.mResource);
      mFlags = buffer.mFlags;
      
      if (!mMap)
        mMap = buffer.mMap ? new IndexedMap<Buffer>(*buffer.mMap) : nullptr;
      else
        *mMap = *buffer.mMap;
      
      return *this;
    }
    
    Buffer& operator=(const XMLTree::Node *node)
    {
      setToXml(node);
      return *this;
    }
    Buffer& operator=(const XMLTree::Node &node)
    {
      setToXml(node);
      return *this;
    }
    
    Buffer& operator=(const Variant &variant) {Variant::operator=(variant); return *this;}
    
    template <typename T>
    Buffer& operator=(const T &value) {Variant::operator=(value); return *this;}
    
    
    bool setToXml(const XMLTree::Node *node) {return node && setToXml(*node);}
    bool setToXml(const XMLTree::Node &node)
    {
      setBufferType(node.element());
      setName(node.attribute("name"));
      
      if (mBufferType == BUFFER_UNIFORM)
      {
        Variant::setToXml(node);
        for (const auto &subNode : node)
          addBuffer(*subNode);
      }
      else
      {
        for (const auto &subNode : node)
          addBuffer(*subNode);
      }
      return true;
    }

    
    void setName(const std::string &name) {mName = name;}
    std::string name() const {return mName;}
    
    void setBufferType(const std::string &str) {setBufferType(ParseBufferType(str));}
    void setBufferType(BUFFER_TYPE type) {mBufferType = type;}
    BUFFER_TYPE bufferType() const {return mBufferType;}
    
    void setResource(Resource *r) {Resource::Replace(&mResource, r);}
    Resource* resource() const {return mResource;}
    
    void setFlags(int f) {mFlags = f;}
    int flags() const {return mFlags;}
    
    void deleteMap()
    {
      if (mMap)
        delete mMap;
      mMap = nullptr;
    }
    void setMap(const IndexedMap<Buffer> &map)
    {
      if (!mMap)
        mMap = new IndexedMap<Buffer>();
      *mMap = map;
    }
    IndexedMap<Buffer>& map()
    {
      if (!mMap)
        mMap = new IndexedMap<Buffer>();
      return *mMap;
    }
    IndexedMap<Buffer>* mapPtr() const {return mMap;}
    
    size_t mapSize() const {return mMap ? mMap->size() : 0;}
    
    typedef IndexedMap<Buffer>::iterator iterator;
    iterator begin() {return map().begin();}
    iterator end() {return map().end();}
    
    
    Buffer& operator[](int index) {return map()[index];}
    Buffer& operator[](const std::string &name) {return map()[name];}
    Buffer& operator[](const char *str) {return (*this)[std::string(str)];}
    
    Buffer& addBuffer(const XMLTree::Node &node)
    {
      int index = map().push(node.attribute("name"), node);
      map()[index].setName(node.attribute("name"));
      return map()[index];
    }
    Buffer& addBuffer(const Buffer &buffer = Buffer())
    {
      int index = map().push(buffer.name(), buffer);
      return map()[index];
    }
    Buffer& addBuffer(const std::string &name, const Buffer &buffer = Buffer())
    {
      int index = map().push(name, buffer);
      map()[index].setName(name);
      return map()[index];
    }
    Buffer& back() {return map().back();}
    Buffer& front() {return map().front();}
    
    void clearMap() {map().clear();}
    
    void set(const std::string &name, const Variant &var)
    {
      getBuffer(name) = var;
      setToUpdate();
    }
    void set(const XMLTree::Node &node)
    {
      getBuffer(node.attribute("name")) = node;
      setToUpdate();
    }
    
    bool loaded() const {return mResource && mResource->loaded();}
    
    bool contains(BUFFER_TYPE type) const
    {
      if (mMap)
      {
        for (Buffer& buffer : *mMap)
        {
          if (buffer.bufferType() == type)
            return true;
        }
      }
      return false;
    }
    bool contains(const std::string &name) const {return mMap && mMap->contains(name);}
    
    Buffer& getBuffer(const std::string &name, BUFFER_TYPE type = BUFFER_UNIFORM)
    {
      if (!map().contains(name))
        return addBuffer(name, type);
      return map()[name];
    }
    Buffer& getBuffer(BUFFER_TYPE type)
    {
      for (Buffer& buffer : map())
      {
        if (buffer.bufferType() == type)
          return buffer;
      }
      return addBuffer(type);
    }
    
    void setToUpdate() {mUpdated = false;}
    void setUpdated() {mUpdated = true;}
    bool updated() const {return mUpdated;}


  private:
    std::string mName;
    BUFFER_TYPE mBufferType;
    IndexedMap<Buffer> *mMap;
    Resource *mResource;
    int mFlags;
    bool mUpdated;
  };
  
  typedef std::list<Buffer*> BufferMapList;
  
  template <typename T>
  static T* GetResource(Buffer *buffer) {return buffer ? static_cast<T*>(buffer->resource()) : nullptr;}
}


#endif /* Buffer_h */
