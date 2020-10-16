//
//  CompressionUtils.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/15/20.
//  Copyright © 2020 Robert Crosby. All rights reserved.
//

#include "CompressionUtils.h"
#include <vector>

#define MIN_MATCH 4


using namespace fx;
using namespace std;


bool CompressionUtils::decompressLZ4(FileData &dst, istream &is, size_t size) {
  bool success = true;
  if (is.peek() == 0) {
    is.seekg((size_t)is.tellg() + 1);
    success = CompressionUtils::decompressLZ4Chunk(dst, is, size-1);
  }
  else {
    size_t ptr = 9, base = is.tellg();
    int chunkSize;
    readL(chunkSize, is);
    chunkSize -= 1;
    
    while (success && chunkSize > 0) {
      is.seekg(base + ptr);
      success = CompressionUtils::decompressLZ4Chunk(dst, is, chunkSize);
      
      ptr += chunkSize;
      if (ptr + 8 < size) {
        ptr += 1;
        is.seekg(base + ptr);
        readL(chunkSize, is);
        ptr += 8;
      }
      else
        chunkSize = 0;
    }
  }
  return success;
}

bool CompressionUtils::decompressLZ4Chunk(FileData &dst, istream &is, size_t size) {
  bool success = true;
  size += is.tellg();
  
  while (is.tellg() < size) {
    // Get Token
    int token = is.get();
    
    // Literal Length
    int litLen = (token >> 4) & 0x0f;
    if (litLen == 15) {
      int value;
      do {
        value = is.get();
        litLen += value;
      } while (value == 255);
    }
    
    // Append the Literal
    FileData literal = readData(litLen, is);
    dst.insert(dst.end(), literal.begin(), literal.end());
    
    // Leave Loop if Last Literal
    if (is.tellg() >= size)
      break;
    
    // Match Offset
    short offset;
    readL(offset, is);
    
    // Match Length
    int matchLen = token & 0x0f;
    if (matchLen == 15) {
      int value;
      do {
        value = is.get();
        matchLen += value;
      } while (value == 255);
    }
    matchLen += MIN_MATCH;
    
    // Copy Matchs
    while (matchLen-- > 0) {
      dst.push_back(dst.at(dst.size()-offset));
    }
  }
  return success;
}
