//
//  ObjLoader.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/14/26.
//

#ifndef ObjLoader_hpp
#define ObjLoader_hpp

#include <FelixEngine/Assets.hpp>


namespace Felix {

class ObjLoader {
public:
  static bool loadStaticMesh(StaticModel& model, const std::string& file);
};

} /* Felix */

#endif /* ObjLoader_hpp */
