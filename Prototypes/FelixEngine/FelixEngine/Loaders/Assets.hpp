//
//  Assets.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/14/26.
//

#ifndef Assets_hpp
#define Assets_hpp

#include <iostream>
#include <vector>
#include <glm/glm.hpp>


namespace Felix {

struct StaticVertex {
  glm::vec3 position{0.0f, 0.0f, 0.0f};
  glm::vec3 normal{0.0f, 0.0f, 1.0f};
  glm::vec2 texcoord{0.0f, 0.0f};
};
typedef std::vector<StaticVertex> StaticVertices;
typedef std::vector<uint32_t> Indices;

struct PhongMaterial {
  std::string name;
  uint32_t offset = 0;
  uint32_t count = 0;
  glm::vec3 diffuseColor{0.4f, 0.4f, 0.4f};
  std::string diffuseTextureName;
};
typedef std::vector<PhongMaterial> PhongMaterials;

struct StaticModel {
  glm::mat4 model{1.0f};
  StaticVertices vertices;
  Indices indices;
  PhongMaterials materials;
};

}  /* Felix */

#endif /* Assets_hpp */
