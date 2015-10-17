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
#include "UniformMap.h"
#include "TextureMap.h"

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
    
    void update()
    {
      if (isLoading())
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
      if (textures)
      {
        int index = 0;
        for (TextureMap::const_iterator itr = textures->begin(); itr != textures->end() && index < mNumTextureLocs; ++itr)
          applyTexture(index++, static_cast<const GLTexture*>(itr->texture()), itr->sampler());
      }
    }
    
    static GLenum GetGLShaderPart(SHADER_PART part);
    
  private:
    bool load();
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
