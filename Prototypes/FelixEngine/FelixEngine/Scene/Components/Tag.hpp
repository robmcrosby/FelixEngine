//
//  Tag.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/9/26.
//

#ifndef Tag_hpp
#define Tag_hpp

#include <iostream>


namespace Felix {

struct Tag {
  std::string tag;
  
  operator std::string&() { return tag; }
  operator const std::string&() const { return tag; }
};

} /* Felix */


#endif /* Tag_hpp */
