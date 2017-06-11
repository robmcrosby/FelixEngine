//
//  Tasking.h
//  FelixEngine
//
//  Created by Robert Crosby on 4/19/16.
//  Copyright © 2016 Robert Crosby. All rights reserved.
//

#ifndef Tasking_h
#define Tasking_h

#include "Delegate.h"


namespace fx
{
  typedef void(TaskFunction)(void*);
  typedef Delegate<void, void*> TaskDelegate;
}


#endif /* Tasking_h */
