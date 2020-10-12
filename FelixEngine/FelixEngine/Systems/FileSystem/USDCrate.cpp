//
//  USDCrate.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/11/20.
//  Copyright © 2020 Robert Crosby. All rights reserved.
//

#include "USDCrate.h"


using namespace std;
using namespace fx;


USDCrate::USDCrate() {
  
}

USDCrate::~USDCrate() {
  
}

bool USDCrate::open(const string &filePath, size_t offset, size_t length) {
  _filePath = filePath;
  _fileOffset = offset;
  _fileLength = length;
  cout << "Open USDCrate: " << filePath << endl;
  cout << "Offset: " << offset << endl;
  cout << "Length: " << length << endl;
  return false;
}
