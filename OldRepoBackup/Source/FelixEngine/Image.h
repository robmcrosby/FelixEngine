//
//  Image.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/14/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Image_h
#define Image_h

#include <iostream>
#include <vector>
#include "Color.h"


namespace fx
{
  template <typename T>
  class Image
  {
  public:
    Image(int w = 0, int h = 0, T c = T()): mWidth(w), mHeight(h), mData(w*h, c) {}
    
    void resize(int w, int h, T c = T()) {
      mWidth  = w;
      mHeight = h;
      mData.resize(size(), c);
    }
    void clear(T c = T()) {mData.clear(); resize(mWidth*mHeight, c);}
    
    T* operator[](int row) {return &mData[0] + index(row, 0);}
    const T* operator[](int row) const {return &mData[0] + index(row, 0);}
    
    unsigned int width()  const {return mWidth;}
    unsigned int height() const {return mHeight;}
    unsigned int size()   const {return mWidth * mHeight;}
    
    int index(int u, int v) const {return u * mWidth + v;}
    
    std::vector<T>& data() {return mData;}
    T* ptr() {return &mData[0];}
    const T* ptr() const {return &mData[0];}
    
    void invert()
    {
      size_t rowSize = mWidth * sizeof(T);
      std::vector<char> temp(rowSize);
      char *ptrA = (char*)&mData[0];
      char *ptrB = (char*)&mData[(mHeight-1)*mWidth];
      while (ptrA < ptrB)
      {
        memcpy(&temp[0], ptrA, rowSize);
        memcpy(ptrA, ptrB, rowSize);
        memcpy(ptrB, &temp[0], rowSize);
        ptrA += rowSize;
        ptrB -= rowSize;
      }
    }
    
  private:
    unsigned int mWidth;
    unsigned int mHeight;
    std::vector<T> mData;
  };
  
  typedef Image<RGBA> ImageRGBA;
}

#endif /* Image_h */
