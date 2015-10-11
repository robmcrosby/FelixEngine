//
//  GLShader.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "GLShader.h"
#include "Platform.h"

#define BUFF_SIZE 512


using namespace fx;
using namespace std;


GLShader::GLShader(GLGraphicSystem *system): mGLSystem(system), mProgramId(0)
{
}

GLShader::~GLShader()
{
}

void GLShader::reload()
{
  GraphicResource::reload();
  if (mGLSystem)
    mGLSystem->loadOnNextUpdate();
}

bool GLShader::load()
{
  if (!mLoaded)
  {
    GLuint shaderParts[SHADER_COUNT];
    mLoaded = true;
    for (int i = 0; mLoaded && i < (int)SHADER_COUNT; ++i)
    {
      GLenum glPart = GetGLShaderPart((SHADER_PART)i);
      if (mPartTypes[i] == SHADER_FILE)
        mLoaded &= compilePart(glPart, Platform::LoadTextFile(mParts[i]).c_str(), shaderParts+i);
      else if (mPartTypes[i] == SHADER_SOURCE)
        mLoaded &= compilePart(glPart, mParts[i].c_str(), shaderParts+i);
      else
        shaderParts[i] = 0;
    }
    
    if (mLoaded)
    {
      mProgramId = glCreateProgram();
      if (mProgramId) {
        attachParts(mProgramId, shaderParts);
        glLinkProgram(mProgramId);
        if (!checkProgram())
        {
          glDeleteProgram(mProgramId);
          mProgramId = 0;
          mLoaded = false;
        }
        else
          setupTextures();
        detachParts(mProgramId, shaderParts);
      }
    }
    deleteParts(shaderParts);
  }
  return mLoaded;
}

GLenum GLShader::GetGLShaderPart(SHADER_PART part)
{
  if (part == SHADER_VERTEX)
    return GL_VERTEX_SHADER;
  if (part == SHADER_FRAGMENT)
    return GL_FRAGMENT_SHADER;
  return 0;
}

void GLShader::unload()
{
  glDeleteProgram(mProgramId);
  mProgramId = 0;
}

bool GLShader::compilePart(GLenum type, const char *src, GLuint *partId)
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
      cerr << "Error creating OpenGL Shader" << endl;
      compiled = false;
    }
  }
  return compiled;
}

bool GLShader::checkPart(GLuint partId, const char *src)
{
  GLint compileSuccess;
  glGetShaderiv(partId, GL_COMPILE_STATUS, &compileSuccess);
  if (compileSuccess == GL_FALSE) {
    GLchar messages[BUFF_SIZE];
    glGetShaderInfoLog(partId, sizeof(messages), 0, &messages[0]);
    cerr << messages << endl;
    cerr << src << endl;
    return false;
  }
  return true;
}

void GLShader::attachParts(GLuint programId, GLuint *partIds)
{
  for (int i = 0; i < (int)SHADER_COUNT; ++i) {
    if (partIds[i] > 0)
      glAttachShader(programId, partIds[i]);
  }
}

void GLShader::detachParts(GLuint programId, GLuint *partIds)
{
  for (int i = 0; i < (int)SHADER_COUNT; ++i) {
    if (partIds[i] > 0)
      glDetachShader(programId, partIds[i]);
  }
}

void GLShader::deleteParts(GLuint *partIds)
{
  for (int i = 0; i < (int)SHADER_COUNT; ++i) {
    if (partIds[i] > 0)
      glDeleteShader(partIds[i]);
    partIds[i] = 0;
  }
}

void GLShader::setupTextures()
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

bool GLShader::checkProgram()
{
  GLint linkSuccess;
  glGetProgramiv(mProgramId, GL_LINK_STATUS, &linkSuccess);
  if (linkSuccess == GL_FALSE) {
    GLchar messages[BUFF_SIZE];
    glGetProgramInfoLog(mProgramId, sizeof(messages), 0, &messages[0]);
    cerr << messages << endl;
    return false;
  }
  return true;
}
