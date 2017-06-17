//
//  Macros.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Macros_h
#define Macros_h

#ifdef __OBJC__
#define OBJC_CLASS(name) @class name;
#else
#define OBJC_CLASS(name) typedef struct objc_object name;
#endif

#endif /* Macros_h */
