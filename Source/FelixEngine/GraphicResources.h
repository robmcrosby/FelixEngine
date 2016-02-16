//
//  GraphicResources.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GraphicResources_h
#define GraphicResources_h

#include <set>

#include "Resource.h"
#include "VertexBufferMap.h"
#include "XMLTree.h"
#include "Vector.h"
#include "Color.h"
#include "Sampler.h"

#define MAIN_WINDOW "MainWindow"


namespace fx
{
  class GraphicSystem;
  
  enum SHADER_PART {
    SHADER_VERTEX = 0,
    SHADER_FRAGMENT,
    SHADER_COUNT,
  };
  
  enum SHADER_TYPE {
    SHADER_EMPTY,
    SHADER_SOURCE,
    SHADER_FILE,
    SHADER_FUNCTION,
  };
  
  enum WINDOW_MODE {
    WINDOW_FULL_MONO,
    WINDOW_FULL_LEFT,
    WINDOW_FULL_RIGHT,
    WINDOW_LEFT_RIGHT,
    WINDOW_RIGHT_LEFT,
    WINDOW_LEFT_OVER_RIGHT,
    WINDOW_RIGHT_OVER_LEFT,
  };
  
  enum STEREO_FLAGS
  {
    STEREO_MONO   = 0x01,
    STEREO_LEFT   = 0x02,
    STEREO_RIGHT  = 0x04,
    STEREO_BINARY = 0x06,
    STEREO_ALL    = 0x07,
  };
  
  /**
   * Frame Resource Interface
   */
  struct Frame: public Resource
  {
    virtual ~Frame() {}
    
    virtual ivec2 size() const = 0;
    virtual vec2 scale() const = 0;
    
    virtual bool resize(const ivec2 &size) = 0;
  };
  
  /**
   * Window Resource Interface
   */
  struct Window: public Resource
  {
    virtual ~Window() {}
    
    virtual ivec2 position() const = 0;
    virtual ivec2 size()     const = 0;
    virtual vec2  scale()    const = 0;
    
    virtual void setMode(WINDOW_MODE mode) = 0;
    virtual WINDOW_MODE mode() const = 0;
  };
  
  /**
   * Shader Resource Interface
   */
  struct Shader: public Resource
  {
    virtual ~Shader() {}
    
    /**
     * Gets a SHADER_PART enum for the given string.
     *
     * @param str String value to parse.
     * @return SHADER_PART enum.
     */
    static SHADER_PART ParseShaderPart(const std::string &str)
    {
      if (str == "Vertex")
        return SHADER_VERTEX;
      if (str == "Fragment")
        return SHADER_FRAGMENT;
      return SHADER_COUNT;
    }
  };
  
  /**
   * Mesh Resource Interface
   */
  struct Mesh: public Resource
  {
    virtual ~Mesh() {}
  };
  
  /**
   * Texture Resource Interface
   */
  struct Texture: public Resource
  {
    virtual ~Texture() {}
    virtual ivec2 size() const = 0;
  };
}

#endif /* GraphicResources_h */
