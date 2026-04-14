//
//  GuiEvent.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/11/26.
//

#ifndef GuiEvent_hpp
#define GuiEvent_hpp

#include <FelixEngine/GuiEventTypes.hpp>
#include <FelixEngine/Scene.hpp>


namespace Felix {

struct GuiEvent {
  GuiEventType type = UndefinedEvent;
  Entity sender;
  
  bool operator==(GuiEventType t) const { return type == t;}
};

} /* Felix */

#endif /* GuiEvent_hpp */
