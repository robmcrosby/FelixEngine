//
//  GuiBounds.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/10/26.
//

#ifndef GuiBounds_hpp
#define GuiBounds_hpp

#include <glm/glm.hpp>


namespace Felix {

struct GuiBounds {
  glm::vec2 position{0.0f, 0.0f};
  glm::vec2 size{0.0f, 0.0f};
};

} /* Felix */

#endif /* GuiBounds_hpp */
