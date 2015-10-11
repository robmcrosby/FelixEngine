//
//  GLShader.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "GLShader.h"
#include "GLGraphicSystem.h"
#include <SDL2/SDL.h>


using namespace fx;
using namespace std;


GLShader::GLShader(GLGraphicSystem *system): mGLSystem(system)
{
}

GLShader::~GLShader()
{
}

bool GLShader::load()
{
  return true;
}

void GLShader::setVertexShaderSrc(const std::string &src)
{
}

void GLShader::setFragmentShaderSrc(const std::string &src)
{
}
