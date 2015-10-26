//
//  Sampler.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Sampler_h
#define Sampler_h

#include <iostream>
#include <cfloat>
#include "XMLTree.h"

namespace fx
{
  enum SAMPLER_COORD
  {
    SAMPLER_COORD_CLAMP_EDGE  = 0,
    SAMPLER_COORD_REPEAT      = 1,
    SAMPLER_COORD_MIRROR      = 2,
    SAMPLER_COORD_CLAMP_ZERO  = 3,
  };
  
  enum SAMPLER_FILTER
  {
    SAMPLER_FILTER_NEAREST = 0,
    SAMPLER_FILTER_LINEAR  = 1,
  };
  
  enum SAMPLER_FLAGS
  {
    SAMPLER_COORD_S_OFFSET = 0,
    SAMPLER_COORD_T_OFFSET = 2,
    SAMPLER_COORD_R_OFFSET = 4,
    
    SAMPLER_COORD_S_MASK   = 0x0003,
    SAMPLER_COORD_T_MASK   = 0x000C,
    SAMPLER_COORD_R_MASK   = 0x0030,
    
    SAMPLER_FILTER_MIN_OFFSET = 6,
    SAMPLER_FILTER_MAG_OFFSET = 7,
    SAMPLER_FILTER_MIP_OFFSET = 8,
    
    SAMPLER_FILTER_MIN_MASK = 0x0040,
    SAMPLER_FILTER_MAG_MASK = 0x0080,
    SAMPLER_FILTER_MIP_MASK = 0x0100,
    
    SAMPLER_MIP_ENABLED   = 0x0200,
    SAMPLER_UN_NORMALIZED = 0x0400
  };
  
  class Sampler
  {
  public:
    Sampler(): mFlags(0), mSamples(1), mLodMin(0), mLodMax(FLT_MAX) {}
    Sampler(const XMLTree::Node &node): mFlags(0), mSamples(1), mLodMin(0), mLodMax(FLT_MAX) {setToXml(node);}
    
    void setSamples(unsigned int samples) {mSamples = samples;}
    unsigned int samples() const {return mSamples;}
    
    void setSCoord(SAMPLER_COORD coord) {mFlags = (mFlags & ~SAMPLER_COORD_S_MASK) | (coord << SAMPLER_COORD_S_OFFSET);}
    SAMPLER_COORD sCoord() const {return (SAMPLER_COORD)((mFlags & SAMPLER_COORD_S_MASK) >> SAMPLER_COORD_S_OFFSET);}
    
    void setTCoord(SAMPLER_COORD coord) {mFlags = (mFlags & ~SAMPLER_COORD_T_MASK) | (coord << SAMPLER_COORD_T_OFFSET);}
    SAMPLER_COORD tCoord() const {return (SAMPLER_COORD)((mFlags & SAMPLER_COORD_T_MASK) >> SAMPLER_COORD_T_OFFSET);}
    
    void setRCoord(SAMPLER_COORD coord) {mFlags = (mFlags & ~SAMPLER_COORD_R_MASK) | (coord << SAMPLER_COORD_R_OFFSET);}
    SAMPLER_COORD rCoord() const {return (SAMPLER_COORD)((mFlags & SAMPLER_COORD_R_MASK) >> SAMPLER_COORD_R_OFFSET);}
    
    void setMinFilter(SAMPLER_FILTER filter) {mFlags = (mFlags & ~SAMPLER_FILTER_MIN_MASK ) | (filter << SAMPLER_FILTER_MIN_OFFSET);}
    SAMPLER_FILTER minFilter() const {return (SAMPLER_FILTER)((mFlags & SAMPLER_FILTER_MIN_MASK) >> SAMPLER_FILTER_MIN_OFFSET);}
    
    void setMagFilter(SAMPLER_FILTER filter) {mFlags = (mFlags & ~SAMPLER_FILTER_MAG_MASK ) | (filter << SAMPLER_FILTER_MAG_OFFSET);}
    SAMPLER_FILTER magFilter() const {return (SAMPLER_FILTER)((mFlags & SAMPLER_FILTER_MAG_MASK) >> SAMPLER_FILTER_MAG_OFFSET);}
    
    void setMipFilter(SAMPLER_FILTER filter) {mFlags = (mFlags & ~SAMPLER_FILTER_MIP_MASK ) | (filter << SAMPLER_FILTER_MIP_OFFSET);}
    SAMPLER_FILTER mipFilter() const {return (SAMPLER_FILTER)((mFlags & SAMPLER_FILTER_MIP_MASK) >> SAMPLER_FILTER_MIP_OFFSET);}
    
    void enableMipMapping() {mFlags |= SAMPLER_MIP_ENABLED;}
    void disableMipMapping() {mFlags &= ~SAMPLER_MIP_ENABLED;}
    bool mipMappingEnabled() const {return mFlags & SAMPLER_MIP_ENABLED;}
    
    void setNormalized() {mFlags &= ~SAMPLER_UN_NORMALIZED;}
    void setUnNormalized() {mFlags |= SAMPLER_UN_NORMALIZED;}
    bool isNormalized() const {return !(mFlags & SAMPLER_UN_NORMALIZED);}
    
    void setLodMin(float lod) {mLodMin = lod;}
    float lodMin() const {return mLodMin;}
    
    void setLodMax(float lod) {mLodMax = lod;}
    float lodMax() const {return mLodMax;}
    
    bool operator<(const Sampler &other) const
    {
      if (mFlags != other.mFlags)
        return mFlags < other.mFlags;
      if (mSamples != other.mSamples)
        return mSamples < other.mSamples;
      if (mLodMin != other.mLodMin)
        return mLodMin < other.mLodMin;
      return mLodMax < other.mLodMax;
    }
    bool operator==(const Sampler &other) const
    {
      return mFlags == other.mFlags && mSamples == other.mSamples && mLodMin == other.mLodMin && mLodMax == other.mLodMax;
    }
    
    bool setToXml(const XMLTree::Node &node)
    {
      if (node.hasAttribute("min"))
        setMinFilter(node.attribute("min") == "linear" ? SAMPLER_FILTER_LINEAR : SAMPLER_FILTER_NEAREST);
      if (node.hasAttribute("mag"))
        setMagFilter(node.attribute("mag") == "linear" ? SAMPLER_FILTER_LINEAR : SAMPLER_FILTER_NEAREST);
      return true;
    }
    
  private:
    int mFlags;
    int mSamples;
    float mLodMin;
    float mLodMax;
  };
}

#endif /* Sampler_h */
