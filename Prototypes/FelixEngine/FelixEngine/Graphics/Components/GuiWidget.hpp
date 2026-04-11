//
//  GuiWidget.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/10/26.
//

#ifndef GuiWidget_hpp
#define GuiWidget_hpp

#include <FelixEngine/GuiTypes.hpp>


namespace Felix {

struct GuiWidget {
  int order = 0;
  GuiWidgetType type = GuiLabel;
  std::string text = "";
  
  bool operator<(const GuiWidget& other) const {
    return order < other.order;
  }
};

} /* Felix */

#endif /* GuiWidget_hpp */
