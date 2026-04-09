//
//  UUID.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/9/26.
//

#ifndef UUID_hpp
#define UUID_hpp

#include <iostream>


namespace Felix {

class UUID {
private:
  uint64_t mUUID;
  
public:
  UUID();
  UUID(uint64_t uuid);
  UUID(const UUID&) = default;
  
  operator uint64_t() const { return mUUID; }
};

} /* Felix */

namespace std {
  template<>
  struct hash<Felix::UUID> {
    std::size_t operator()(const Felix::UUID& uuid) const {
      return hash<uint64_t>()((uint64_t)uuid);
    }
  };
} /* std */



#endif /* UUID_hpp */
