//
//  MeshLoader.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "MeshLoader.h"
#include "FileSystem.h"
#include "StringUtils.h"

#include <sstream>

#define NAME_BUFFER_SIZE 32


using namespace fx;
using namespace std;

bool MeshLoader::LoadMeshFromXML(VertexBufferMap &bufferMap, const XMLTree::Node &node)
{
  bool success = false;
  if (node.hasAttribute("file"))
  {
    string filePath = node.tree() ? node.tree()->path() : "";
    success = LoadMeshFromFile(bufferMap, filePath+node.attribute("file"));
  }
  else if (node.numberSubNodes() == 0)
    success = LoadMeshPrimitive(bufferMap, node);
  else
  {
    success = true;
    bufferMap.clear();
    bufferMap.setPrimitiveType(node.attribute("primitive"));
    for (XMLTree::const_iterator itr = node.begin(); itr != node.end(); ++itr)
    {
      if ((*itr)->element() == "Buffer")
        success &= AddBuffer(bufferMap, **itr);
      else if ((*itr)->element() == "Indices")
        StringUtils::ParseInts(bufferMap.indexBuffer(), (*itr)->contents());
      else if ((*itr)->element() == "SubMesh")
        success &= AddSubMesh(bufferMap, **itr);
    }
  }
  return success;
}

bool MeshLoader::LoadMeshFromFile(VertexBufferMap &bufferMap, const std::string &filePath)
{
  bool success = false;
  std::string postfix = StringUtils::GetFilePostfix(filePath);
  
  if (postfix == "xml")
  {
    fx::XMLTree tree;
    if (tree.loadFile(filePath))
    {
      if (tree.begin() != tree.end())
        success = LoadMeshFromXML(bufferMap, **tree.begin());
      else
        cerr << "Mesh XML File is empty" << endl;
    }
    else
      cerr << "Error Reading Mesh XML File: " << filePath << endl;
  }
  else if (postfix == "mesh")
  {
    ifstream meshFile;
    meshFile.open(filePath, ios::in | ios::binary);
    if (meshFile.is_open())
    {
      success = LoadMeshFromStream(bufferMap, meshFile);
      meshFile.close();
    }
    else
      cerr << "Error Opening Mesh File: " << filePath << endl;
  }
  
  return success;
}

bool MeshLoader::LoadMeshPrimitive(VertexBufferMap &bufferMap, const XMLTree::Node &node)
{
  bool success = false;
  string primitive = node.attribute("primitive");
  
  if (primitive == "plane")
  {
    vec2 size(1.0f, 1.0f);
    vec2 offset(0.0f, 0.0f);
    
    if (node.hasAttribute("size"))
      size = node.attribute("size");
    if (node.hasAttribute("offset"))
      offset = node.attribute("offset");
    
    success = LoadMeshPlane(bufferMap, size, offset);
  }
  else
    cerr << "Unknown Mesh Primitive: " << primitive << endl;
  
  return success;
}

bool MeshLoader::LoadMeshPlane(VertexBufferMap &bufferMap, const vec2 &size, const vec2 &offset)
{
  VertexBuffer *buffer;
  
  bufferMap.clear();
  bufferMap.addSubMesh(Range(0, 4));
  bufferMap.setPrimitiveType(VERTEX_TRIANGLE_STRIP);
  
  // Set Positions
  vec2 dim = size/2.0f;
  buffer = &bufferMap["Position"];
  buffer->setComponents(4);
  buffer->append(vec4(-dim.x+offset.x,  dim.y+offset.y, 0.0f, 1.0f));
  buffer->append(vec4(-dim.x+offset.x, -dim.y+offset.y, 0.0f, 1.0f));
  buffer->append(vec4( dim.x+offset.x,  dim.y+offset.y, 0.0f, 1.0f));
  buffer->append(vec4( dim.x+offset.x, -dim.y+offset.y, 0.0f, 1.0f));
  
  // Set Normals
  buffer = &bufferMap["Normal"];
  buffer->setComponents(4);
  for (int i = 0; i < 4; ++i)
    buffer->append(vec4(0.0f, 0.0f, -1.0f, 1.0f));
  
  // Set UVs
  buffer = &bufferMap["UV_0"];
  buffer->setComponents(2);
  buffer->append(vec2(0.0f, 1.0f));
  buffer->append(vec2(0.0f, 0.0f));
  buffer->append(vec2(1.0f, 1.0f));
  buffer->append(vec2(1.0f, 0.0f));
  
  return true;
}

bool MeshLoader::AddSubMesh(VertexBufferMap &bufferMap, const XMLTree::Node &node)
{
  bool success = false;
  if (node.hasAttribute("start") && node.hasAttribute("end"))
  {
    Range range;
    range.start = node.attributeAsInt("start");
    range.end   = node.attributeAsInt("end");
    bufferMap.addSubMesh(range);
    success = true;
  }
  return success;
}

bool MeshLoader::AddBuffer(VertexBufferMap &bufferMap, const XMLTree::Node &node)
{
  bool success = false;
  if (node.hasAttribute("attribute") && node.hasAttribute("components"))
  {
    VertexBuffer &buffer = bufferMap[node.attribute("attribute")];
    buffer.clear();
    buffer.setComponents(node.attributeAsInt("components"));
    StringUtils::ParseFloats(buffer.data(), node.contents());
    success = buffer.data().size();
  }
  else
    cerr << "Vertex Buffer Node missing either attribute or components attributes" << endl;
  
  return success;
}

bool MeshLoader::LoadMeshFromStream(VertexBufferMap &bufferMap, std::istream &is)
{
  bool success = true;
  int primitiveType = 0;
  int numSubMeshes = 0;
  
  bufferMap.clear();
  
  if (FileSystem::Read(primitiveType, is) == sizeof(int) && FileSystem::Read(numSubMeshes, is) == sizeof(int))
  {
    bufferMap.setPrimitiveType(GetVertexPrimitive(primitiveType));
    
    // Read the Sub-Mesh Information.
    for (int i = 0; i < numSubMeshes && success; ++i)
    {
      Range range;
      success = FileSystem::Read(range.start, is) == sizeof(int) && FileSystem::Read(range.end, is) == sizeof(int);
      bufferMap.addSubMesh(range);
    }
    
    int numVertices, numBuffers;
    if (success && FileSystem::Read(numVertices, is) == sizeof(int) && FileSystem::Read(numBuffers, is) == sizeof(int))
    {
      for (int i = 0; i < numBuffers && success; ++i)
        success |= ReadBufferFromStream(bufferMap, numVertices, is);
    }
    else
      success = false;
    
    if (primitiveType == 8 || primitiveType == 9)
    {
      int numIndices;
      if (success && FileSystem::Read(numIndices, is) == sizeof(int))
      {
        bufferMap.indexBuffer().resize(numIndices);
        FileSystem::Read(&bufferMap.indexBuffer()[0], numIndices, is);
      }
      else
        success = false;
    }
  }
  else
    success = false;
  
  return success;
}

VERTEX_PRIMITIVE MeshLoader::GetVertexPrimitive(int value)
{
  if (value == 0 || value == 8)
    return VERTEX_TRIANGLES;
  if (value == 1 || value == 9)
    return VERTEX_TRIANGLE_STRIP;
  
  cerr << "Unknown Vertex Primitive Value: " << value << endl;
  cerr << "Using Primitive Triangles for now" << endl;
  return VERTEX_TRIANGLES;
}

bool MeshLoader::ReadBufferFromStream(VertexBufferMap &bufferMap, int numVertices, std::istream &is)
{
  int compSize = 0;
  
  if (FileSystem::Read(compSize, is) == sizeof(int))
  {
    char name[NAME_BUFFER_SIZE];
    
    is.read(name, sizeof(name));
    if (is.gcount() == NAME_BUFFER_SIZE && name[0] != '\0')
    {
      string bufferName(name);
      VertexBuffer &buffer = bufferMap[bufferName];
      
      buffer.setComponents(compSize);
      buffer.resize(numVertices);
      
      return FileSystem::Read(buffer.ptr(), numVertices*compSize, is) == numVertices*compSize*sizeof(float);
    }
  }
  return false;
}