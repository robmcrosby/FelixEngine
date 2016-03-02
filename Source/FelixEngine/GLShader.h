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

#define ERROR_MSG_SIZE 512
#define MAX_TEXTURES 8


namespace fx
{
  /**
   * Resource that manages an OpenGL shader.
   */
  class GLShader: public Shader
  {
  public:
    /**
     * Constructor initalizes the Shader.
     *
     * @param system Pointer to the GLGraphicSystem.
     */
    GLShader(GLGraphicSystem *system): mGLSystem(system), mProgramId(0) {}
    
    /**
     * Destructor that calls unload.
     */
    virtual ~GLShader() {unload();}
    
    /**
     * Calls OpenGL to bind this Shader.
     */
    void bind() const
    {
      glUseProgram(mProgramId);
      for (int i = 0; i < mNumTextureLocs; ++i)
        glUniform1i(mTextureLocs[i], i);
    }
    
    /**
     * Gets the OpenGL Attribute location to the given name.
     *
     * @param name String refrnece of the name.
     * @return OpenGL Attribute location
     */
    GLint getAttributeIndex(const std::string &name) const {return glGetAttribLocation(mProgramId, name.c_str());}
    
    /**
     * Gets the OpenGL uniform location to the given name.
     *
     * @param name String refrnece of the name.
     * @return OpenGL Uniform location
     */
    GLint getUniformLocation(const std::string &name) const {return glGetUniformLocation(mProgramId, name.c_str());}
    
    /**
     * Gets the OpenGL enum for the Shader Part.
     *
     * @param part Either SHADER_VERTEX or GL_FRAGMENT_SHADER.
     * @return OpenGL enum of the Shader type.
     */
    static GLenum GetGLShaderPart(SHADER_PART part)
    {
      if (part == SHADER_VERTEX)
        return GL_VERTEX_SHADER;
      if (part == SHADER_FRAGMENT)
        return GL_FRAGMENT_SHADER;
      return 0;
    }

    /**
     * Loads and Compiles the Shader source from the given BufferMap.
     *
     * @param bufferMap Refrence to the BufferMap containing the shader source.
     */
    void uploadBufferMap(Buffer &bufferMap)
    {
      bool success = false;
      unload();
      if (bufferMap.mapSize() >= 2)
      {
        GLuint shaderParts[SHADER_COUNT];
        for (int i = 0; i < SHADER_COUNT; ++i)
          shaderParts[i] = 0;
        
        // Load the Shader Parts
        success = true;
        for (const Buffer &buffer : bufferMap)
          success &= uploadBuffer(buffer, shaderParts);
        
        if (success)
          createProgram(shaderParts);
        deleteParts(shaderParts);
      }
      setLoaded(success);
    }
    
  private:
    /**
     * Loads and Compiles the source of a Shader Part.
     *
     * @param buffer Refrnece to the Buffer containing the Shader Source.
     * @param parts Pointer to an array of OpenGL Shader handles.
     * @return true if the Shader part was successfully loaded and compiled or false if an error.
     */
    bool uploadBuffer(const Buffer &buffer, GLuint *parts)
    {
      SHADER_PART part = (SHADER_PART)buffer.flags();
      GLenum type = GetGLShaderPart(part);
      std::string source = buffer;
      if (source == "")
        source = mGLSystem->getShaderFunction(buffer.name());
      return preCompile(source) && compilePart(type, source.c_str(), parts+part);
    }
    
    bool preCompile(std::string &src)
    {
      size_t start;
      while ((start = src.find("#include")) != std::string::npos)
      {
        size_t cmt = src.find("\"", start);
        size_t end = src.find("\"", cmt+1);
        if (cmt == std::string::npos || end == std::string::npos)
        {
          std::cerr << "Error parsing include" << std::endl;
          return false;
        }
        
        std::string name = src.substr(cmt+1, end-cmt-1);
        std::string header = mGLSystem->getShaderFunction(name);
        src = src.substr(0, start) + header + src.substr(end+1);
      }
      return true;
    }
    
    /**
     * Creates a Shader Program from the given Shader Parts.
     *
     * @param parts Pointer to an array of OpenGL Shader handles.
     * @return true if the Shader Program was successfully created or false otherwise.
     */
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
    
    /**
     * Deletes the Shader Program.
     */
    void unload()
    {
      glDeleteProgram(mProgramId);
      mProgramId = 0;
      setUnloaded();
    }
    
    /**
     * Compiles a Shader Part with the given type and source.
     *
     * @param type OpenGL enum for the type of Shader Part.
     * @param src C string of the Shader's source.
     * @param [out] partId Pointer to the Shader Part's handle.
     * @return true if the Shader is successfully compiled or false otherwise.
     */
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
    
    /**
     * Checks the given Shader Part for Errors where if any are found, Prints
     * the OpenGL error message with the Shader Source.
     *
     * @param partId OpenGL Shader Part Handle.
     * @param src C string of the Shader's source.
     * @return true if no problems or false if errors are found.
     */
    bool checkPart(GLuint partId, const char *src)
    {
      GLint compileSuccess;
      glGetShaderiv(partId, GL_COMPILE_STATUS, &compileSuccess);
      if (compileSuccess == GL_FALSE) {
        GLchar messages[ERROR_MSG_SIZE];
        glGetShaderInfoLog(partId, sizeof(messages), 0, &messages[0]);
        std::cerr << messages << std::endl;
        std::cerr << src << std::endl;
        return false;
      }
      return true;
    }
    
    /**
     * Attaches the given Shader Parts to the Shader Program.
     *
     * @param programId OpenGL Shader Program handle.
     * @param partIds Pointer to OpenGL Shader Part handles.
     */
    void attachParts(GLuint programId, GLuint *partIds)
    {
      for (int i = 0; i < (int)SHADER_COUNT; ++i) {
        if (partIds[i] > 0)
          glAttachShader(programId, partIds[i]);
      }
    }
    
    /**
     * Detaches the Shader Parts from the Shader Program.
     *
     * @param programId OpenGL Shader Program handle.
     * @param partIds Pointer to OpenGL Shader Part handles.
     */
    void detachParts(GLuint programId, GLuint *partIds)
    {
      for (int i = 0; i < (int)SHADER_COUNT; ++i) {
        if (partIds[i] > 0)
          glDetachShader(programId, partIds[i]);
      }
    }
    
    /**
     * Deletes the given Shader Parts.
     *
     * @param partIds Pointer to OpenGL Shader Part handles.
     */
    void deleteParts(GLuint *partIds)
    {
      for (int i = 0; i < (int)SHADER_COUNT; ++i) {
        if (partIds[i] > 0)
          glDeleteShader(partIds[i]);
        partIds[i] = 0;
      }
    }
    
    /**
     * Detects any Textures in the Shader and sets up their indices.
     */
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
    
    /**
     * Checks the Shader Program for any Errors after linking the Shader Parts where
     * if any errors are found, an OpenGL error is printed.
     *
     * @return true if no errors or false otherwise.
     */
    bool checkProgram()
    {
      GLint linkSuccess;
      glGetProgramiv(mProgramId, GL_LINK_STATUS, &linkSuccess);
      if (linkSuccess == GL_FALSE) {
        GLchar messages[ERROR_MSG_SIZE];
        glGetProgramInfoLog(mProgramId, sizeof(messages), 0, &messages[0]);
        std::cerr << messages << std::endl;
        return false;
      }
      return true;
    }
    
  private:
    GLuint mProgramId;      /**< OpenGL Shader Program handle */
    int    mNumTextureLocs; /**< Number of Textures used by the Shader Program */
    GLint  mTextureLocs[MAX_TEXTURES]; /**< Array of Locations to Textures in the Shader */
    
    GLGraphicSystem *mGLSystem; /**< Pointer to the GLGraphicsSystem that owns this Shader */
  };
}

#endif /* GLShader_h */
