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
