//
//  MultiVector.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/26/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef MultiVector_h
#define MultiVector_h

#include "Semaphore.h"
#include <list>
#include <vector>

namespace fx
{
  template <typename T>
  class MultiVector
  {
  public:
    MultiVector(int size = 0): mSemephore(size) {setup();}
    MultiVector(const MultiVector &other): mSemephore(other.mSemephore.size()) {setup();}
    ~MultiVector() {delete [] mVectors;}
    
    MultiVector& operator=(const MultiVector &other) {return *this;}
    
    void append(const T &value)
    {
      int i = mSemephore.waitForIndex();
      mVectors[i].vector.push_back(value);
      mSemephore.postIndex(i);
    }
    void clear()
    {
      mSemephore.lock();
      for (int i = 0; i < mSemephore.size(); ++i)
        mVectors[i].vector.clear();
      mSemephore.unlock();
    }
    void dump(std::list<T> &data)
    {
      mSemephore.lock();
      for (int i = 0; i < mSemephore.size(); ++i)
      {
        data.insert(data.end(), mVectors[i].vector.begin(), mVectors[i].vector.end());
        mVectors[i].vector.clear();
      }
      mSemephore.unlock();
    }
    void dump(std::vector<T> &data)
    {
      mSemephore.lock();
      for (int i = 0; i < mSemephore.size(); ++i)
      {
        data.insert(data.end(), mVectors[i].vector.begin(), mVectors[i].vector.end());
        mVectors[i].vector.clear();
      }
      mSemephore.unlock();
    }
    
  private:
    struct Vector
    {
      std::vector<T> vector;
      char buffer[SDL_CACHELINE_SIZE-sizeof(std::vector<T>)];
    };
    
  private:
    void setup() {mVectors = new Vector[mSemephore.size()];}
    
    Semaphore mSemephore;
    Vector *mVectors;
  };
}

#endif /* MultiList_h */
