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
  
  operator Entity() const { return parent; }
};

} /* Felix */

#endif /* Parent_hpp */
