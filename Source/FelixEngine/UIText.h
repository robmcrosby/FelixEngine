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
#include "Scene.h"
#include "FileSystem.h"
#include "RenderSlots.h"
#include "MeshLoader.h"


#define GLYPH_TABLE_SIZE 128

namespace fx
{
  /**
   *
   */
  class UIText: public UIWidget
  {
  public:
    UIText(Scene *scene): UIWidget(scene), mAtlas(0), mRenderSlots(0) {initGlyphTable();}
    virtual ~UIText() {}
    
    virtual void setToXml(const XMLTree::Node &node)
    {
      UIWidget::setToXml(node);
      
      if (node.hasAttribute("font"))
        setFontFile(mScene->directory()+node.attribute("font"));
      
      mRenderSlots = getChild<RenderSlots>();
    }
    
    virtual bool init()
    {
      bool success = false;
      if (UIWidget::init())
      {
        if (mFontFile.extension() == "font")
          success = loadBitmapFont();
      }
      return success;
    }
    
    void setFontFile(const File &file) {mFontFile = file;}
    void setAtlas(Texture *atlas) {mAtlas = atlas;}
    
    
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
    
    void initGlyphTable()
    {
      for (int i = 0; i < GLYPH_TABLE_SIZE; ++i)
        mGlyphTable[i] = Glyph();
    }
    
    bool loadBitmapFont()
    {
      bool success = false;
      if (mFontFile.exists())
      {
        std::fstream fileIn;
        fileIn.open(mFontFile.path().c_str(), std::ios::in);
        if (fileIn.is_open())
        {
          std::string line;
          getline(fileIn, line);
          
          size_t pos = line.find(',');
          if (pos != std::string::npos)
          {
            File atlasFile = mScene->directory() + line.substr(0, pos);
            vec2 atlasSize = line.substr(pos+1);
            
            // Load the Texture Atlas of the Font
            Texture *atlas = FelixEngine::GetGraphicSystem()->getTexture(atlasFile.name());
            atlas->setImageFile(atlasFile.path());
            atlas->setToLoad();
            setAtlas(atlas);
            
            // Get the Glyph Information
            initGlyphTable();
            while (getline(fileIn, line))
            {
              //32,18,125,309,0,0,0,0
              int code;
              Glyph glyph;
              sscanf(line.c_str(), "%i , %f , %f , %f , %f , %f , %f , %f" ,
                     &code, &glyph.advance, &glyph.uvLoc.x, &glyph.uvLoc.y,
                     &glyph.loc.x, &glyph.loc.y, &glyph.size.x, &glyph.size.y);
              glyph.uvLoc /= atlasSize;
              glyph.uvSize = glyph.size / atlasSize;
              
              if (code > 0 && code < GLYPH_TABLE_SIZE)
                mGlyphTable[code] = glyph;
            }
            
            fileIn.close();
            success = setupRenderSlots();
          }
        }
      }
      return success;
    }
    
    bool setupRenderSlots()
    {
      static Shader *fontShader = nullptr;
      static Mesh *planeMesh = nullptr;
      
      bool success = false;
      if (mRenderSlots)
      {
        if (!fontShader)
        {
          fontShader = FelixEngine::GetGraphicSystem()->getShader("FontShader");
          fontShader->setFunctionToPart("VertexUV", SHADER_VERTEX);
          fontShader->setFunctionToPart("SimpleTexture", SHADER_FRAGMENT);
        }
        if (!planeMesh)
        {
          planeMesh = FelixEngine::GetGraphicSystem()->getMesh("PlaneMesh");
          MeshLoader::LoadMeshPlane(planeMesh->getVertexBufferMap(), vec2(1.0f, 1.0f), vec2(0.0f, 0.0f));
          planeMesh->setToLoad();
        }
        
        mRenderSlots->addSlot();
        mRenderSlot = mRenderSlots->back();
        mRenderSlot->setMaterial(new Material());
        mRenderSlot->material()->setShader(fontShader);
        mRenderSlot->material()->textureMap().push();
        mRenderSlot->material()->textureMap().back().setTexture(mAtlas);
        mRenderSlot->setMesh(planeMesh);
        mRenderSlot->setPass("UIPass");
        mRenderSlot->setLayer(1);
        mRenderSlot->blendState().setEquation(BLEND_EQ_ADD);
        mRenderSlot->blendState().setSrc(BLEND_INPUT_SRC_ALPHA);
        mRenderSlot->blendState().setDst(BLEND_INPUT_ONE_MINUS | BLEND_INPUT_SRC_ALPHA);
        
        success = true;
      }
      return success;
    }
    
    File mFontFile;
    
    Glyph mGlyphTable[GLYPH_TABLE_SIZE];
    
    Texture *mAtlas;
    RenderSlots *mRenderSlots;
    RenderSlot *mRenderSlot;
  };
}

#endif /* UIText_h */
