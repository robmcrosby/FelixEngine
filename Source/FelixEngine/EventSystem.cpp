//
//  EventSystem.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 11/1/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "EventSystem.h"
#include "EventHandler.h"

using namespace fx;
using namespace std;


Pool<EventHandler::Observer> EventHandler::ObserverPool;
List<EventHandler::Observer*> EventHandler::UsedObservers;
