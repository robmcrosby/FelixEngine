//
//  ObjLoader.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/14/26.
//

#include "ObjLoader.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <map>
#include <tuple>


using namespace std;
using namespace glm;
namespace Felix {

bool ObjLoader::loadStaticMesh(StaticModel& model, const string& file) {
  tinyobj::ObjReaderConfig configuration;
  configuration.mtl_search_path = "./";
  configuration.triangulate = true;
  
  // Parse the Obj File
  tinyobj::ObjReader reader;
  if (!reader.ParseFromFile(file, configuration)) {
    if (!reader.Error().empty())
      cerr << "TinyObjReader: " << reader.Error();
    return false;
  }
  if (!reader.Warning().empty())
    cout << "TinyObjReader: " << reader.Warning();
  
  auto& attrib = reader.GetAttrib();
  auto& shapes = reader.GetShapes();
  auto& materials = reader.GetMaterials();
  
  cout << "Materials: " << materials.size() << endl;
  for (const auto& material : materials) {
    cout << "  Name:" << material.name << endl;
    cout << "  Diffuse Texname:" << material.diffuse_texname << endl;
  }
  
  size_t reserveSize = attrib.vertices.size()/3;
  model.vertices.reserve(reserveSize);
  model.indices.reserve(reserveSize);
  model.materials.reserve(shapes.size());
  
  map<tuple<int, int, int>, uint32_t> vertexMap;
  for (size_t s = 0; s < shapes.size(); ++s) {
    auto& material = model.materials.emplace_back();
    material.offset = (uint32_t)model.indices.size();
    if (s < materials.size()) {
      material.name = materials[s].name;
      material.diffuseColor.r = (float)materials[s].diffuse[0];
      material.diffuseColor.g = (float)materials[s].diffuse[1];
      material.diffuseColor.b = (float)materials[s].diffuse[2];
      material.diffuseTextureName = materials[s].diffuse_texname;
    }
    
    size_t offset = 0;
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f) {
      size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
      
      for (size_t v = 0; v < fv; v++) {
        tinyobj::index_t idx = shapes[s].mesh.indices[offset+v];
        tuple<int, int, int> key(idx.vertex_index, idx.normal_index, idx.texcoord_index);
        if (!vertexMap.contains(key)) {
          vertexMap[key] = (uint32_t)model.vertices.size();
          
          // Add Vertex
          StaticVertex& vertex = model.vertices.emplace_back();
          vertex.position.x = (float)attrib.vertices[3*idx.vertex_index+0];
          vertex.position.y = (float)attrib.vertices[3*idx.vertex_index+1];
          vertex.position.z = (float)attrib.vertices[3*idx.vertex_index+2];
          
          // Set Normal
          if (idx.normal_index >= 0) {
            vertex.normal.x = (float)attrib.normals[3*idx.normal_index+0];
            vertex.normal.y = (float)attrib.normals[3*idx.normal_index+1];
            vertex.normal.z = (float)attrib.normals[3*idx.normal_index+2];
          }
          
          // Set Texture Coord
          if (idx.texcoord_index >= 0) {
            vertex.texcoord.x = (float)attrib.texcoords[2*idx.texcoord_index+0];
            vertex.texcoord.y = (float)attrib.texcoords[2*idx.texcoord_index+1];
          }
        }
        model.indices.emplace_back(vertexMap.at(key));
      }
      offset += fv;
    }
    material.count = (uint32_t)model.indices.size() - material.offset;
  }
  return true;
}

} /* Felix */
