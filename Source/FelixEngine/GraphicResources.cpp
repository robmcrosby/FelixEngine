//
//  GraphicResources.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "GraphicResources.h"
#include "GraphicSystem.h"
#include "FelixEngine.h"
#include "MeshLoader.h"
#include "FileSystem.h"


using namespace fx;
using namespace std;



bool Window::setToXml(const XMLTree::Node &node)
{
  bool success = true;
  if (node.hasAttribute("title"))
    setTitle(node.attribute("title"));
  if (node.hasSubNode("position"))
    setPosition(node.subContents("position"));
  if (node.hasSubNode("size"))
    setSize(node.subContents("size"));
  return success;
}



bool Frame::setToXml(const XMLTree::Node &node)
{
  bool success = true;
  
  // Set the size
  setSize(ivec2(0, 0));
  setScale(vec2(1.0f, 1.0f));
  if (node.hasAttribute("width") && node.hasAttribute("height"))
  {
    int width  = node.attributeAsInt("width");
    int height = node.attributeAsInt("height");
    setSize(ivec2(width, height));
  }
  else
  {
    setRefrenceFrame(node.hasAttribute("frame") ? node.attribute("frame") : MAIN_WINDOW);
    if (node.hasAttribute("scale"))
      setScale(node.attribute("scale"));
  }
  return success;
}

void Frame::setRefrenceFrame(const std::string &name)
{
  GraphicSystem *sys = FelixEngine::GetGraphicSystem();
  if (sys && name != "")
    setRefrenceFrame(sys->getFrame(name));
  else
    setRefrenceFrame(nullptr);
}




bool Shader::setToXml(const XMLTree::Node &node)
{
  bool success = true;
  clearParts();
  for (XMLTree::const_iterator itr = node.begin(); itr != node.end(); ++itr)
  {
    SHADER_PART part = ParseShaderPart((*itr)->element());
    if ((*itr)->hasAttribute("file"))
    {
      //setFileToPart(FileSystem::GetResourcesPath()+(*itr)->attribute("file"), part);
      string filePath = (*itr)->tree() ? (*itr)->tree()->path() : "";
      setFileToPart(filePath+(*itr)->attribute("file"), part);
    }
    else if ((*itr)->hasAttribute("function"))
      setFunctionToPart((*itr)->attribute("function"), part);
    else
      setSourceToPart((*itr)->contents(), part);
  }
  return success;
}

void Shader::clearParts()
{
  for (int i = 0; i < (int)SHADER_COUNT; ++i)
  {
    mParts[i] = "";
    mPartTypes[i] = SHADER_EMPTY;
  }
}

void Shader::setSourceToPart(const std::string &src, SHADER_PART part)
{
  int i = (int)part;
  if (i < (int)SHADER_COUNT)
  {
    mParts[i] = src;
    mPartTypes[i] = SHADER_SOURCE;
  }
}

void Shader::setFileToPart(const std::string &file, SHADER_PART part)
{
  int i = (int)part;
  if (i < (int)SHADER_COUNT)
  {
    mParts[i] = file;
    mPartTypes[i] = SHADER_FILE;
  }
}

void Shader::setFunctionToPart(const std::string &func, SHADER_PART part)
{
  int i = (int)part;
  if (i < (int)SHADER_COUNT)
  {
    mParts[i] = func;
    mPartTypes[i] = SHADER_FUNCTION;
  }
}

SHADER_PART Shader::ParseShaderPart(const std::string &partStr)
{
  if (partStr == "Vertex")
    return SHADER_VERTEX;
  if (partStr == "Fragment")
    return SHADER_FRAGMENT;
  return SHADER_COUNT;
}



bool Mesh::setToXml(const XMLTree::Node &node)
{
  return MeshLoader::LoadMeshFromXML(mBufferMap, node);
}

void Mesh::addVertexBuffer(const std::string &name, int components, int count, const float *data)
{
  VertexBuffer &buffer = mBufferMap[name];
  buffer.setComponents(components);
  buffer.resize(count);
  memcpy(buffer.ptr(), data, sizeof(float)*components*count);
}

void Mesh::setIndexBuffer(int count, const int *data)
{
  IndexBuffer &buffer = mBufferMap.indexBuffer();
  buffer.resize(count);
  memcpy(&buffer.at(0), data, sizeof(int)*count);
}

void Mesh::setPrimitiveType(VERTEX_PRIMITIVE type)
{
  mBufferMap.setPrimitiveType(type);
}



bool Texture::setToXml(const XMLTree::Node &node)
{
  bool success = false;
  if (node.hasAttribute("file"))
  {
    string filePath = node.tree() ? node.tree()->path() : "";
    setImageFile(filePath+node.attribute("file"));
    if (mSampler.setToXml(node))
      success = true;
  }
  return success;
}
