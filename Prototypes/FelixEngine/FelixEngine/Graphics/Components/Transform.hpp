//
//  Transform.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/9/26.
//

#ifndef Transform_hpp
#define Transform_hpp

#include <glm/glm.hpp>


namespace Felix {

struct Transform {
  glm::mat4 model{1.0f};
  glm::mat4 atlas{1.0f};
};

typedef std::vector<Transform> Transforms;

} /* Felix */

#endif /* Transform_hpp */
