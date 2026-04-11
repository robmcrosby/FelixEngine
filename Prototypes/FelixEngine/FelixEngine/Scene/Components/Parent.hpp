//
//  Parent.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/9/26.
//

#ifndef Parent_hpp
#define Parent_hpp

#include <FelixEngine/Scene.hpp>

namespace Felix {

struct Parent {
  Entity parent;
  
  Parent() = default;
  Parent(const Parent&) = default;
  Parent(const Entity& entity): parent(entity) {}
  
  bool operator==(const Entity& other) const {
    return parent == other;
  }
  bool operator==(const entt::entity& other) const {
    return parent.handle() == other;
  }
  operator Entity() const { return parent; }
};

} /* Felix */

#endif /* Parent_hpp */
