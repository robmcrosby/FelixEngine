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







bool MeshLoader::LoadMeshFromXML(BufferMap &bufferMap, const XMLTree::Node &node)
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
    bufferMap.setFlags(VertexBufferMap::GetPrimitiveType(node.attribute("primitive")));
    
    for (XMLTree::const_iterator itr = node.begin(); itr != node.end(); ++itr)
    {
      if ((*itr)->element() == "Buffer")
        success &= AddBuffer(bufferMap, **itr);
      else if ((*itr)->element() == "Indices")
        success &= AddIndices(bufferMap, **itr);
      else if ((*itr)->element() == "SubMesh")
        success &= AddSubMesh(bufferMap.getBuffer("SubMeshes", BUFFER_RANGES), **itr);
    }

    // Check if any Sub Meshes have been added.
    if (!bufferMap.contains("SubMeshes"))
    {
      ivec2 range;
      if (bufferMap.contains(BUFFER_INDICES))
        range.y = (int)bufferMap.getBuffer(BUFFER_INDICES).size();
      else if (bufferMap.contains(BUFFER_VERTEX))
        range.y = (int)bufferMap.getBuffer(BUFFER_VERTEX).size();
      bufferMap.getBuffer("SubMeshes", BUFFER_RANGES) = range;
    }
  }
  return success;
}

bool MeshLoader::LoadMeshFromStream(BufferMap &bufferMap, std::istream &is)
{
  bool success = true;
  int primitiveType = 0;
  int numSubMeshes = 0;
  
  bufferMap.clear();
  
  if (FileSystem::Read(primitiveType, is) == sizeof(int) && FileSystem::Read(numSubMeshes, is) == sizeof(int))
  {
    bufferMap.setFlags(GetVertexPrimitive(primitiveType));
    
    // Load the Sub-Mesh Information
    Buffer &subMeshBuffer = bufferMap.getBuffer("SubMeshes", BUFFER_RANGES);
    subMeshBuffer.resize(VAR_INT_2, numSubMeshes);
    FileSystem::Read((unsigned int*)subMeshBuffer.ptr(), numSubMeshes*2, is);
    
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
        Buffer &indexBuffer = bufferMap.getBuffer("Indices", BUFFER_INDICES);
        indexBuffer.resize(VAR_INT, numIndices);
        FileSystem::Read((unsigned int*)indexBuffer.ptr(), numIndices, is);
      }
      else
        success = false;
    }
  }
  else
    success = false;
  
  return success;
}

bool MeshLoader::LoadMeshFromFile(BufferMap &bufferMap, const std::string &file)
{
  bool success = false;
  std::string postfix = StringUtils::GetFilePostfix(file);
  
  if (postfix == "xml")
  {
    fx::XMLTree tree;
    if (tree.loadFile(file))
    {
      if (tree.begin() != tree.end())
        success = LoadMeshFromXML(bufferMap, **tree.begin());
      else
        cerr << "Mesh XML File is empty" << endl;
    }
    else
      cerr << "Error Reading Mesh XML File: " << file << endl;
  }
  else if (postfix == "mesh")
  {
    ifstream meshFile;
    meshFile.open(file, ios::in | ios::binary);
    if (meshFile.is_open())
    {
      success = LoadMeshFromStream(bufferMap, meshFile);
      meshFile.close();
    }
    else
      cerr << "Error Opening Mesh File: " << file << endl;
  }
  
  return success;
}

bool MeshLoader::LoadMeshPrimitive(BufferMap &bufferMap, const XMLTree::Node &node)
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

bool MeshLoader::LoadMeshPlane(BufferMap &bufferMap, const vec2 &size, const vec2 &offset)
{
  bufferMap.clear();
  bufferMap.getBuffer("SubMeshes", BUFFER_RANGES) = ivec2(0, 4);
  
  // Set Positions
  vec2 dim = size/2.0f;
  Buffer &posBuffer = bufferMap.getBuffer("Position", BUFFER_VERTEX);
  posBuffer.append(vec4(-dim.x+offset.x,  dim.y+offset.y, 0.0f, 1.0f));
  posBuffer.append(vec4(-dim.x+offset.x, -dim.y+offset.y, 0.0f, 1.0f));
  posBuffer.append(vec4( dim.x+offset.x,  dim.y+offset.y, 0.0f, 1.0f));
  posBuffer.append(vec4( dim.x+offset.x, -dim.y+offset.y, 0.0f, 1.0f));
  
  // Set Normals
  Buffer &normBuffer = bufferMap.getBuffer("Normal", BUFFER_VERTEX);
  for (int i = 0; i < 4; ++i)
    normBuffer.append(vec4(0.0f, 0.0f, -1.0f, 1.0f));
  
  // Set UVs
  Buffer &uvBuffer = bufferMap.getBuffer("UV_0", BUFFER_VERTEX);
  uvBuffer.append(vec2(0.0f, 1.0f));
  uvBuffer.append(vec2(0.0f, 0.0f));
  uvBuffer.append(vec2(1.0f, 1.0f));
  uvBuffer.append(vec2(1.0f, 0.0f));
  
  return true;
}



bool MeshLoader::AddBuffer(BufferMap &bufferMap, const XMLTree::Node &node)
{
  bool success = false;
  int components = node.attributeAsInt("components");
  if (node.hasAttribute("attribute") && (components == 2 || components == 4))
  {
    Buffer &buffer = bufferMap.getBuffer(node.attribute("attribute"), BUFFER_VERTEX);
    
    // TODO: implement parsing of values directly with buffer.
    
    // Get the buffer contents in a float vector
    vector<float> data;
    StringUtils::ParseFloats(data, node.contents());
    
    // Resize the buffer for the contents
    if (components == 2)
      buffer.setValues((vec2*)&data[0], data.size()/2);
    else
      buffer.setValues((vec4*)&data[0], data.size()/4);
    success = data.size();
  }
  else
    cerr << "Vertex Buffer Node missing either attribute or components attributes" << endl;
  
  return success;
}

bool MeshLoader::AddIndices(BufferMap &bufferMap, const XMLTree::Node &node)
{
  // Get the index data from the node Contents.
  vector<int> data;
  StringUtils::ParseInts(data, node.contents());
  
  // Create an Index buffer and load with the data.
  Buffer &buffer = bufferMap.getBuffer("Indices", BUFFER_INDICES);
  buffer.setValues(VAR_INT, &data[0]);
  
  return data.size();
}

bool MeshLoader::AddSubMesh(Buffer &buffer, const XMLTree::Node &node)
{
  bool success = false;
  if (node.hasAttribute("start") && node.hasAttribute("end"))
  {
    ivec2 range;
    range.x = node.attributeAsInt("start");
    range.y = node.attributeAsInt("end");
    buffer.append(range);
    success = true;
  }
  return success;
}

bool MeshLoader::ReadBufferFromStream(BufferMap &bufferMap, int numVertices, std::istream &is)
{
  int compSize = 0;
  
  if (FileSystem::Read(compSize, is) == sizeof(int))
  {
    char name[NAME_BUFFER_SIZE];
    
    is.read(name, sizeof(name));
    if (is.gcount() == NAME_BUFFER_SIZE && name[0] != '\0')
    {
      Buffer &buffer = bufferMap.getBuffer(name, BUFFER_VERTEX);
      
      if (compSize == 2)
        buffer.resize(VAR_FLOAT_2, numVertices);
      if (compSize == 4)
        buffer.resize(VAR_FLOAT_4, numVertices);
      
      return FileSystem::Read((float*)buffer.ptr(), buffer.sizeInBytes()/sizeof(float), is);
    }
  }
  return false;
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