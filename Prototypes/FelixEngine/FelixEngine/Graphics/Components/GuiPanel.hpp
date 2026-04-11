//
//  GuiPanel.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/10/26.
//

#ifndef GuiPanel_hpp
#define GuiPanel_hpp

#include <glm/glm.hpp>
#include <FelixEngine/GuiTypes.hpp>


namespace Felix {

struct GuiPanel {
  int widgetCount  = 0;
  bool visible     = true;
  bool collapsed   = false;
  bool collapsable = false;
  bool moveable    = false;
  bool sizeable    = false;
  bool background  = false;
  GuiAlignment alignX = GuiAlignLeft;
  GuiAlignment alignY = GuiAlignTop;
  glm::vec2 offset{0.0f, 0.0f};
};

} /* Felix */

#endif /* GuiPanel_hpp */
