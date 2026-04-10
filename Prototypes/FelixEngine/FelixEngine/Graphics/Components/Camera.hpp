//
//  Camera.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/9/26.
//

#ifndef Camera_hpp
#define Camera_hpp

#include <glm/glm.hpp>


namespace Felix {

struct Camera {
  glm::mat4 projection{1.0f};
  glm::mat4 view{1.0f};
  glm::vec3 position{0.0f};
};

} /* Felix */

#endif /* Camera_hpp */
