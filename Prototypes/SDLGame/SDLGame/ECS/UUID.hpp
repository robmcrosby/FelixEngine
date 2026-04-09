//
//  UUID.hpp
//  SDLGame
//
//  Created by Robert Crosby on 4/7/26.
//

#ifndef UUID_hpp
#define UUID_hpp

#include <iostream>

class UUID {
private:
  uint64_t mUUID;
  
public:
  UUID();
  UUID(uint64_t uuid);
  UUID(const UUID&) = default;
  
  operator uint64_t() const { return mUUID; }
};

namespace std {
  template<>
  struct hash<UUID> {
    std::size_t operator()(const UUID& uuid) const {
      return hash<uint64_t>()((uint64_t)uuid);
    }
  };
}

#endif /* UUID_hpp */
