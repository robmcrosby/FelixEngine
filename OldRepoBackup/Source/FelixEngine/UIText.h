//
//  UIText.h
//  FelixEngine
//
//  Created by Robert Crosby on 12/30/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef UIText_h
#define UIText_h

#include "UIWidget.h"
#include "UIWidget.h"
#include "RenderSlots.h"
#include "MeshLoader.h"
#include "FileSystem.h"
#include "Projection.h"
#include "View.h"

#define GLYPH_TABLE_SIZE 128


namespace fx
{
  /**
   *
   */
  class UIText: public UIWidget
  {
  public:
    UIText(Scene *scene): UIWidget(scene),
      mFontScale(1.0f),
      mRenderSlots(0),
      mGlyphSlot(0),
      mUpdateText(true)
    {
      mAtlasSampler.setMinFilter(SAMPLER_FILTER_LINEAR);
      mAtlasSampler.setMagFilter(SAMPLER_FILTER_LINEAR);
      setEventFlags(EVENT_WINDOW);
    }
    virtual ~UIText() {}
    
    virtual void setToXml(const XMLTree::Node &node)
    {
      UIWidget::setToXml(node);
      mRenderSlots = getChild<RenderSlots>();
      
      if (node.hasAttribute("font"))
        setFont(node.attribute("font"));
      if (node.hasAttribute("text"))
        setText(node.attribute("text"));
      if (node.hasAttribute("scale"))
        setFontScale(node.attributeAsFloat("scale"));
    }
    
    virtual bool init() {return UIWidget::init();}
    
    virtual void update()
    {
      UIWidget::update();
      if (mUpdateText)
        updateText();
    }
    
    bool setFont(const std::string &file)
    {
      return setFont(FileSystem::GetResources() + "Fonts/" + file);
    }
    
    bool setFont(const File &file)
    {
      if (file.extension() == "font")
        return loadBitmapFont(file);
      return false;
    }
    
    void setAtlasTexture(const File &file)
    {
      Buffer texture = mScene->createTexture(file.name(), file.path());
      setAtlasTexture(texture);
    }
    void setAtlasTexture(const Buffer &texture)
    {
      mAtlasTexture = texture;
    }
    
    void setAtlasSampler(const Sampler &sampler)
    {
      mAtlasSampler = sampler;
    }
    
    void setFontScale(float scale) {mFontScale = scale;}
    float fontScale() const {return mFontScale;}
    
    void setText(const std::string &text)
    {
      if (text != mText)
      {
        mText = text;
        mUpdateText = true;
      }
    }
    std::string text() const {return mText;}
    
  private:
    struct Glyph
    {
      Glyph(): advance(0) {}
      
      vec2 size;
      vec2 loc;
      float advance;
      
      vec2 uvLoc;
      vec2 uvSize;
    };
    
  private:
    bool setupGlyphSlot()
    {
      if (!mRenderSlots)
        mRenderSlots = getChild<RenderSlots>();
      if (!mGlyphSlot)
        mGlyphSlot = mRenderSlots->addSlot();
      
      mGlyphSlot->setMesh("GlyphPlane");
      if (!mGlyphSlot->mesh().loaded())
      {
        MeshLoader::LoadMeshPlane(mGlyphSlot->mesh(), vec2(1.0f, 1.0f), vec2(0.0f, 0.0f));
        mGlyphSlot->uploadMesh();
      }
      
      mGlyphSlot->setShader("Atlas");
      mGlyphSlot->setTexture(mAtlasTexture, mAtlasSampler);
      mGlyphSlot->drawState().blendState.setDefaults();
      mGlyphSlot->setPass(UI_PASS);
      mGlyphSlot->setLayer(1);
      
      return true;
    }
    
    void updateText()
    {
      if (mGlyphSlot)
      {
        mUpdateText = false;
        float advance = 0.0f;
        Variant locs, uvs;
        
        for (const auto &c : mText)
        {
          unsigned int i = (unsigned int)c;
          if (i < GLYPH_TABLE_SIZE)
          {
            Glyph &glyph = mGlyphTable[i];
            if (glyph.size.w != 0.0f && glyph.size.h != 0.0f)
            {
              vec4 loc;
              loc.x = advance + (glyph.size.x/2.0f + glyph.loc.x) * mFontScale;
              loc.y = (glyph.loc.y - glyph.size.y/2.0f) * mFontScale;
              loc.z = glyph.size.w * mFontScale;
              loc.w = glyph.size.h * mFontScale;
              locs.append(loc);
              
              vec4 uv;
              uv.x = glyph.uvLoc.x;
              uv.y = 1.0f - glyph.uvLoc.y;
              uv.z = uv.x + glyph.uvSize.w;
              uv.w = uv.y - glyph.uvSize.h;
              uvs.append(uv);
            }
            advance += glyph.advance * mFontScale;
          }
        }
        
        if (locs.size() > 0)
        {
          vec4 offset(-advance/2.0f, 0.0f, 0.0f, 0.0f);
          vec4 div(size(), size());
          for (int i = 0; i < locs.size(); ++i)
            locs.vec4At(i) = (locs.vec4At(i) + offset)/div;
          mGlyphSlot->setInstanced("AtlasLocs", locs);
          mGlyphSlot->setInstanced("AtlasUVs", uvs);
        }
        mGlyphSlot->drawState().instances = (int)locs.size();
      }
    }
    
    void initGlyphTable()
    {
      for (int i = 0; i < GLYPH_TABLE_SIZE; ++i)
        mGlyphTable[i] = Glyph();
    }
    
    void setGlyph(const std::string &str, const vec2 &atlasSize)
    {
      int code;
      Glyph glyph;
      sscanf(str.c_str(), "%i , %f , %f , %f , %f , %f , %f , %f" ,
             &code, &glyph.advance, &glyph.uvLoc.x, &glyph.uvLoc.y,
             &glyph.loc.x, &glyph.loc.y, &glyph.size.x, &glyph.size.y);
      
      glyph.uvLoc /= atlasSize;
      glyph.uvSize = glyph.size / atlasSize;
      
      if (code > 0 && code < GLYPH_TABLE_SIZE)
        mGlyphTable[code] = glyph;
    }
    
    bool loadBitmapFont(const File &fontFile)
    {
      bool success = false;
      if (fontFile.exists())
      {
        std::fstream fileIn;
        fileIn.open(fontFile.path().c_str(), std::ios::in);
        if (fileIn.is_open())
        {
          std::string line;
          getline(fileIn, line);
          
          size_t pos = line.find(',');
          if (pos != std::string::npos)
          {
            File atlasFile = fontFile.upperDirectory() + line.substr(0, pos);
            vec2 atlasSize = line.substr(pos+1);
            
            setAtlasTexture(atlasFile);
            
            // Get the Glyph Information
            initGlyphTable();
            while (getline(fileIn, line))
              setGlyph(line, atlasSize);
            
            success = setupGlyphSlot();
          }
          fileIn.close();
        }
      }
      return success;
    }
    
    RenderSlots *mRenderSlots;
    RenderSlot  *mGlyphSlot;
    
    float mFontScale;
    bool mUpdateText;
    std::string mText;
    
    Glyph mGlyphTable[GLYPH_TABLE_SIZE];
    
    Buffer  mAtlasTexture;
    Sampler mAtlasSampler;
  };
}

#endif /* UIText_h */
