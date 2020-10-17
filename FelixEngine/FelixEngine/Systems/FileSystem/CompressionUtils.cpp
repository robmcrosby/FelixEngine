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


bool CompressionUtils::decodeLongsL(LongVector &dst, FileData &src) {
  bool success = true;
  
  memstream is((uint8_t*)&src[0], src.size());
  for (size_t i = src.size()/sizeof(long); i > 0; --i) {
    dst.push_back(readLongL(is));
  }
  return success;
}

bool CompressionUtils::decompressUSD32(IntVector &dst, FileData &src, size_t count) {
  bool success = true;
  
  size_t numCodes = (count*2 + 7)/8;
  memstream is((uint8_t*)&src[0], src.size());
  int commonValue = readIntL(is);
  int preValue = 0;
  
  memstream codes((uint8_t*)&src[4], numCodes);
  memstream vints((uint8_t*)&src[numCodes+4], src.size()-numCodes-4);
  
  size_t cp = 0;
  while (cp < count) {
    int token = codes.get();
    for (int i = 0; cp < count && i < 4; ++i) {
      int code = (token >> i*2) & 0x3;
      if (code == 0)
        preValue += commonValue;
      else if (code == 1)
        preValue += (char)vints.get();
      else if (code == 2)
        preValue += readShortL(vints);
      else
        preValue += readIntL(vints);
      dst.push_back(preValue);
      ++cp;
    }
  }
  return success;
}

bool CompressionUtils::decompressLZ4(FileData &dst, istream &is, size_t size) {
  bool success = true;
  if (is.peek() == 0) {
    is.seekg((size_t)is.tellg() + 1);
    success = CompressionUtils::decompressLZ4Chunk(dst, is, size-1);
  }
  else {
    size_t ptr = 9, base = is.tellg();
    int chunkSize = readIntL(is) - 1;
    
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
