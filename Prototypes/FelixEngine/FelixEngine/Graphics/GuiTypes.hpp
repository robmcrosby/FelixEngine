//
//  GuiTypes.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/10/26.
//

#ifndef GuiTypes_hpp
#define GuiTypes_hpp

#include <iostream>
#include <glm/glm.hpp>


namespace Felix {

enum GuiWidgetType {
  GuiLabel,
  GuiButton,
};

enum GuiAlignment {
  GuiAlignCenter,
  GuiAlignLeft,
  GuiAlignRight,
  GuiAlignTop,
  GuiAlignBottom,
};

} /* Felix */

#endif /* GuiTypes_hpp */
