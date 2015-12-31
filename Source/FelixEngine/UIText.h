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
#include "FileSystem.h"
#include "RenderSlots.h"
#include "MeshLoader.h"

namespace fx
{
  /**
   *
   */
  class UIText: public UIWidget
  {
  public:
    UIText(Scene *scene): UIWidget(scene), mAtlas(0), mRenderSlots(0) {}
    virtual ~UIText() {}
    
    virtual void setToXml(const XMLTree::Node &node)
    {
      UIWidget::setToXml(node);
      addChildren(node);
      
      if (node.hasAttribute("font"))
        setFontFile(mDirectory+node.attribute("font"));
      
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
            File atlasFile = mDirectory + line.substr(0, pos);
            //ivec2 atlasSize = line.substr(pos+1);
            Texture *atlas = FelixEngine::GetGraphicSystem()->getTexture(atlasFile.name());
            atlas->setImageFile(atlasFile.path());
            atlas->setToLoad();
            setAtlas(atlas);
            
            // TODO: get and process the glyths
            
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
        RenderSlot *slot = mRenderSlots->back();
        slot->setMaterial(new Material());
        slot->material()->setShader(fontShader);
        slot->material()->textureMap().push();
        slot->material()->textureMap().back().setTexture(mAtlas);
        slot->setMesh(planeMesh);
        slot->setPass("UIPass");
        slot->setLayer(1);
        slot->blendState().setEquation(BLEND_EQ_ADD);
        slot->blendState().setSrc(BLEND_INPUT_SRC_ALPHA);
        slot->blendState().setDst(BLEND_INPUT_ONE_MINUS | BLEND_INPUT_SRC_ALPHA);
        
        success = true;
      }
      return success;
    }
    
    File mFontFile;
    
    Texture *mAtlas;
    RenderSlots *mRenderSlots;
  };
}

#endif /* UIText_h */
