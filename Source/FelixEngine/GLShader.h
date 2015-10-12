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
#include "UniformMap.h"

#define MAX_TEXTURES 8


namespace fx
{
  /**
   *
   */
  class GLShader: public Shader
  {
  public:
    GLShader(GLGraphicSystem *system);
    virtual ~GLShader();
    
    virtual void reload();
    
    bool load();
    
    void use() const
    {
      glUseProgram(mProgramId);
      for (int i = 0; i < mNumTextureLocs; ++i)
        glUniform1i(mTextureLocs[i], i);
    }
    
    GLint getAttributeIndex(const std::string &name) const {return glGetAttribLocation(mProgramId, name.c_str());}
    void applyUniform(const std::string &name, const Uniform &uniform) const
    {
      GLint loc = glGetUniformLocation(mProgramId, name.c_str());
      
      if (loc != -1)
      {
        switch (uniform.type())
        {
          case VAR_FLOAT:
            glUniform1fv(loc, (GLsizei)uniform.size(), (const GLfloat*)uniform.ptr());
            break;
          case VAR_INT:
            glUniform1iv(loc, (GLsizei)uniform.size(), (const GLint*)uniform.ptr());
            break;
          case VAR_FLOAT_2:
            glUniform2fv(loc, (GLsizei)uniform.size(), (const GLfloat*)uniform.ptr());
            break;
          case VAR_INT_2:
            glUniform2iv(loc, (GLsizei)uniform.size(), (const GLint*)uniform.ptr());
            break;
          case VAR_FLOAT_3:
            glUniform3fv(loc, (GLsizei)uniform.size(), (const GLfloat*)uniform.ptr());
            break;
          case VAR_INT_3:
            glUniform3iv(loc, (GLsizei)uniform.size(), (const GLint*)uniform.ptr());
            break;
          case VAR_FLOAT_4:
            glUniform4fv(loc, (GLsizei)uniform.size(), (const GLfloat*)uniform.ptr());
            break;
          case VAR_INT_4:
            glUniform4iv(loc, (GLsizei)uniform.size(), (const GLint*)uniform.ptr());
            break;
          case VAR_MTX_2X2:
            glUniformMatrix2fv(loc, (GLsizei)uniform.size(), GL_FALSE, (const GLfloat*)uniform.ptr());
            break;
          case VAR_MTX_3X3:
            glUniformMatrix3fv(loc, (GLsizei)uniform.size(), GL_FALSE, (const GLfloat*)uniform.ptr());
            break;
          case VAR_MTX_4X4:
            glUniformMatrix4fv(loc, (GLsizei)uniform.size(), GL_FALSE, (const GLfloat*)uniform.ptr());
            break;
          case VAR_UNKNOWN:
            break;
        }
      }
    }
    
    void applyUniformMap(const UniformMap *uniforms) const
    {
      if (uniforms)
      {
        for (UniformMap::const_iterator itr = uniforms->begin(); itr != uniforms->end(); ++itr)
          applyUniform(itr->first, itr->second);
      }
    }
    
    void applyTexture(int index, const GLTexture *texture, const Sampler &sampler) const
    {
//      if (texture)
//        texture->use(index, sampler);
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
    
    static GLenum GetGLShaderPart(SHADER_PART part);
    
  private:
    void unload();
    
    bool compilePart(GLenum type, const char *src, GLuint *partId);
    bool checkPart(GLuint partId, const char *src);
    void attachParts(GLuint programId, GLuint *partIds);
    void detachParts(GLuint programId, GLuint *partIds);
    void deleteParts(GLuint *partIds);
    void setupTextures();
    
    bool checkProgram();
    
  private:
    GLGraphicSystem *mGLSystem;
    GLuint mProgramId;
    GLint  mTextureLocs[MAX_TEXTURES];
    int    mNumTextureLocs;
  };
}

#endif /* GLShader_h */
