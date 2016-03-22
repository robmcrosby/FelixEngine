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
    SAMPLER_SAMPLES_MASK = 0x00ff,
    SAMPLER_MAX_SAMPLES  = 32,
    
    SAMPLER_COORD_S_OFFSET = 8,
    SAMPLER_COORD_T_OFFSET = 10,
    SAMPLER_COORD_R_OFFSET = 12,
    
    SAMPLER_COORD_S_MASK   = 0x0300,
    SAMPLER_COORD_T_MASK   = 0x0C00,
    SAMPLER_COORD_R_MASK   = 0x3000,
    
    SAMPLER_FILTER_MIN_OFFSET = 14,
    SAMPLER_FILTER_MAG_OFFSET = 15,
    SAMPLER_FILTER_MIP_OFFSET = 16,
    
    SAMPLER_FILTER_MIN_MASK = 0x004000,
    SAMPLER_FILTER_MAG_MASK = 0x008000,
    SAMPLER_FILTER_MIP_MASK = 0x010000,
    
    SAMPLER_MIP_ENABLED   = 0x020000,
    SAMPLER_UN_NORMALIZED = 0x040000
  };
  
  class Sampler
  {
  public:
    Sampler(int flags = 0, float min = 0.0f, float max = FLT_MAX): mFlags(flags), mLodMin(min), mLodMax(max) {}
    Sampler(const XMLTree::Node &node): mFlags(0), mLodMin(0), mLodMax(FLT_MAX) {setToXml(node);}
    
    void setSamples(int samples)
    {
      samples = samples < 1 ? 1 : samples > SAMPLER_MAX_SAMPLES ? SAMPLER_MAX_SAMPLES : samples;
      mFlags = (mFlags & ~SAMPLER_SAMPLES_MASK) | ((samples - 1) & SAMPLER_SAMPLES_MASK);
    }
    int samples() const {return (mFlags & ~SAMPLER_SAMPLES_MASK) + 1;}
    
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
    
    int flags() const {return mFlags;}
    
    bool operator<(const Sampler &other) const
    {
      if (mFlags != other.mFlags)
        return mFlags < other.mFlags;
      if (mLodMin != other.mLodMin)
        return mLodMin < other.mLodMin;
      return mLodMax < other.mLodMax;
    }
    bool operator==(const Sampler &other) const
    {
      return mFlags == other.mFlags && mLodMin == other.mLodMin && mLodMax == other.mLodMax;
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
    float mLodMin;
    float mLodMax;
  };
}

#endif /* Sampler_h */
