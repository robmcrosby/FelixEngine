//
//  GLShader.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GLShader_h
#define GLShader_h

#include "GLGraphicSystem.h"
#include "GLTexture.h"
#include "TextureMap.h"
#include "FileSystem.h"

#define BUFF_SIZE 512
#define MAX_TEXTURES 8


namespace fx
{
  /**
   *
   */
  class GLShader: public Shader
  {
  public:
    GLShader(GLGraphicSystem *system): mGLSystem(system), mProgramId(0) {}
    virtual ~GLShader() {}
    
    void update()
    {
      if (loading())
      {
        if (load())
          setLoaded();
        else
          setNotLoading();
      }
    }
    
    void use() const
    {
      glUseProgram(mProgramId);
      for (int i = 0; i < mNumTextureLocs; ++i)
        glUniform1i(mTextureLocs[i], i);
    }
    GLint getAttributeIndex(const std::string &name) const {return glGetAttribLocation(mProgramId, name.c_str());}
    GLint getUniformLocation(const std::string &name) const {return glGetUniformLocation(mProgramId, name.c_str());}
    
    void applyTexture(int index, const GLTexture *texture, const Sampler &sampler) const
    {
      if (texture)
        texture->use(index, sampler);
    }
    void applyTextureMap(const TextureMap *textures) const
    {
//      if (textures)
//      {
//        int index = 0;
//        for (TextureMap::const_iterator itr = textures->begin(); itr != textures->end() && index < mNumTextureLocs; ++itr)
//          applyTexture(index++, static_cast<const GLTexture*>(itr->texture()), itr->sampler());
//      }
    }
    
    static GLenum GetGLShaderPart(SHADER_PART part)
    {
      if (part == SHADER_VERTEX)
        return GL_VERTEX_SHADER;
      if (part == SHADER_FRAGMENT)
        return GL_FRAGMENT_SHADER;
      return 0;
    }

    void uploadBufferMap(const BufferMap &bufferMap)
    {
      bool success = false;
      unload();
      if (bufferMap.size() >= 2)
      {
        GLuint shaderParts[SHADER_COUNT];
        for (int i = 0; i < SHADER_COUNT; ++i)
          shaderParts[i] = 0;
        
        // Load the Shader Parts
        success = true;
        for (BufferMap::const_iterator itr = bufferMap.begin(); itr != bufferMap.end(); ++itr)
          success &= loadBuffer(*itr, shaderParts);
        
        if (success)
          createProgram(shaderParts);
        deleteParts(shaderParts);
      }
      
      if (success)
        setLoaded();
      else
        setUnloaded();
    }
    
  private:
    bool loadBuffer(const Buffer &buffer, GLuint *parts)
    {
      SHADER_PART part = (SHADER_PART)buffer.flags();
      GLenum type = GetGLShaderPart(part);
      std::string source = buffer;
      if (source == "")
        source = mGLSystem->getShaderFunction(buffer.name());
      return compilePart(type, source.c_str(), parts+part);
    }
    
    bool createProgram(GLuint *parts)
    {
      mProgramId = glCreateProgram();
      if (!mProgramId)
      {
        std::cerr << "Error: Unable to create OpenGL Program" << std::endl;
        return false;
      }
      attachParts(mProgramId, parts);
      glLinkProgram(mProgramId);
      if (!checkProgram())
      {
        glDeleteProgram(mProgramId);
        mProgramId = 0;
        return false;
      }
      setupTextures();
      detachParts(mProgramId, parts);
      return true;
    }
    
    
    bool load()
    {
      bool success = false;
      if (!loaded())
      {
        GLuint shaderParts[SHADER_COUNT];
        success = true;
        for (int i = 0; success && i < (int)SHADER_COUNT; ++i)
        {
          GLenum glPart = GetGLShaderPart((SHADER_PART)i);
          if (mPartTypes[i] == SHADER_FILE)
            success &= compilePart(glPart, FileSystem::LoadText(mParts[i]).c_str(), shaderParts+i);
          else if (mPartTypes[i] == SHADER_SOURCE)
            success &= compilePart(glPart, mParts[i].c_str(), shaderParts+i);
          else if (mPartTypes[i] == SHADER_FUNCTION)
            success &= compilePart(glPart, mGLSystem->getShaderFunction(mParts[i]).c_str(), shaderParts+i);
          else
            shaderParts[i] = 0;
        }
        
        if (success)
        {
          mProgramId = glCreateProgram();
          if (mProgramId) {
            attachParts(mProgramId, shaderParts);
            glLinkProgram(mProgramId);
            if (!checkProgram())
            {
              glDeleteProgram(mProgramId);
              mProgramId = 0;
              success = false;
            }
            else
              setupTextures();
            detachParts(mProgramId, shaderParts);
          }
        }
        deleteParts(shaderParts);
      }
      return success;
    }
    
    void unload()
    {
      glDeleteProgram(mProgramId);
      mProgramId = 0;
      setUnloaded();
    }
    
    bool compilePart(GLenum type, const char *src, GLuint *partId)
    {
      bool compiled = true;
      if (strlen(src) > 0) {
        *partId = glCreateShader(type);
        if (*partId > 0) {
          glShaderSource(*partId, 1, &src, 0);
          glCompileShader(*partId);
          compiled = checkPart(*partId, src);
        }
        else {
          std::cerr << "Error creating OpenGL Shader" << std::endl;
          compiled = false;
        }
      }
      return compiled;
    }
    
    bool checkPart(GLuint partId, const char *src)
    {
      GLint compileSuccess;
      glGetShaderiv(partId, GL_COMPILE_STATUS, &compileSuccess);
      if (compileSuccess == GL_FALSE) {
        GLchar messages[BUFF_SIZE];
        glGetShaderInfoLog(partId, sizeof(messages), 0, &messages[0]);
        std::cerr << messages << std::endl;
        std::cerr << src << std::endl;
        return false;
      }
      return true;
    }
    
    void attachParts(GLuint programId, GLuint *partIds)
    {
      for (int i = 0; i < (int)SHADER_COUNT; ++i) {
        if (partIds[i] > 0)
          glAttachShader(programId, partIds[i]);
      }
    }
    
    void detachParts(GLuint programId, GLuint *partIds)
    {
      for (int i = 0; i < (int)SHADER_COUNT; ++i) {
        if (partIds[i] > 0)
          glDetachShader(programId, partIds[i]);
      }
    }
    
    void deleteParts(GLuint *partIds)
    {
      for (int i = 0; i < (int)SHADER_COUNT; ++i) {
        if (partIds[i] > 0)
          glDeleteShader(partIds[i]);
        partIds[i] = 0;
      }
    }
    
    void setupTextures()
    {
      int texLoc = glGetUniformLocation(mProgramId, "tex2D");
      if (texLoc != -1)
      {
        mNumTextureLocs = 1;
        mTextureLocs[0] = 0;
      }
      else
      {
        char texStr[32];
        mNumTextureLocs = 0;
        for (int i = 0; i < MAX_TEXTURES; ++i)
        {
          sprintf(texStr, "tex2D_%i", i);
          texLoc = glGetUniformLocation(mProgramId, texStr);
          if (texLoc != -1)
          {
            mTextureLocs[i] = texLoc;
            ++mNumTextureLocs;
          }
          else
            break;
        }
      }
    }
    
    bool checkProgram()
    {
      GLint linkSuccess;
      glGetProgramiv(mProgramId, GL_LINK_STATUS, &linkSuccess);
      if (linkSuccess == GL_FALSE) {
        GLchar messages[BUFF_SIZE];
        glGetProgramInfoLog(mProgramId, sizeof(messages), 0, &messages[0]);
        std::cerr << messages << std::endl;
        return false;
      }
      return true;
    }
    
  private:
    GLGraphicSystem *mGLSystem;
    GLuint mProgramId;
    GLint  mTextureLocs[MAX_TEXTURES];
    int    mNumTextureLocs;
  };
}

#endif /* GLShader_h */
