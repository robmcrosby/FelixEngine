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
      mRenderSlots(0),
      mGlyphSlot(0)
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
    }
    
    virtual bool init() {return UIWidget::init();}
    
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
      
      mGlyphSlot->setShader("Texture");
      mGlyphSlot->setTexture(mAtlasTexture, mAtlasSampler);
      mGlyphSlot->drawState().blendState.setDefaults();
      mGlyphSlot->setPass(UI_PASS);
      mGlyphSlot->setLayer(1);
      
      return true;
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
    
    Glyph mGlyphTable[GLYPH_TABLE_SIZE];
    
    Buffer  mAtlasTexture;
    Sampler mAtlasSampler;
  };
}











//#include "UIWidget.h"
//#include "Scene.h"
//#include "FileSystem.h"
//#include "RenderSlots.h"
//#include "MeshLoader.h"
//
//
//#define GLYPH_TABLE_SIZE 128
//#define TEXT_VERT_SHADER "TextUV"
//#define TEXT_FRAG_SHADER "SimpleTexture"
//
//namespace fx
//{
//  /**
//   *
//   */
//  class UIText: public UIWidget
//  {
//  public:
//    UIText(Scene *scene): UIWidget(scene), mAtlas(0), mRenderSlots(0), mUpdateText(0)
//    {
//      initGlyphTable();
//    }
//    virtual ~UIText() {}
//    
//    virtual void setToXml(const XMLTree::Node &node)
//    {
//      UIWidget::setToXml(node);
//      
//      if (node.hasAttribute("font"))
//        setFontFile(mScene->directory()+node.attribute("font"));
//      if (node.hasAttribute("text"))
//        setText(node.attribute("text"));
//      
//      mRenderSlots = getChild<RenderSlots>();
//    }
//    
//    virtual bool init()
//    {
//      bool success = false;
//      if (UIWidget::init())
//      {
//        if (mFontFile.extension() == "font")
//          success = loadBitmapFont();
//      }
//      return success;
//    }
//    
//    void setFontFile(const File &file) {mFontFile = file;}
//    void setAtlas(Texture *atlas) {mAtlas = atlas;}
//    
//    void setText(const std::string &text)
//    {
//      mText = text;
//      mUpdateText = true;
//    }
//    std::string text() const {return mText;}
//    
//    virtual void update()
//    {
//      UIWidget::update();
//      if (mUpdateText)
//        updateText();
//    }
//    
//  private:
//    struct Glyph
//    {
//      Glyph(): advance(0) {}
//      
//      vec2 size;
//      vec2 loc;
//      float advance;
//      
//      vec2 uvLoc;
//      vec2 uvSize;
//    };
//
//    void updateText()
//    {
////      if (mRenderSlot)
////      {
////        mUpdateText = false;
////        float advance = 0.0f;
////        std::vector<vec4> locs;
////        std::vector<vec4> uvs;
////        
////        for (std::string::const_iterator c = mText.begin(); c != mText.end(); ++c)
////        {
////          unsigned int i = (unsigned int)*c;
////          if (i < GLYPH_TABLE_SIZE)
////          {
////            Glyph &glyph = mGlyphTable[i];
////            if (glyph.size.w != 0.0f && glyph.size.h != 0.0f)
////            {
////              vec4 loc;
////              loc.x = advance - glyph.loc.x;
////              loc.y = glyph.loc.y;
////              loc.z = glyph.size.w;
////              loc.w = glyph.size.h;
////              locs.push_back(loc);
////              
////              vec4 uv;
////              uv.x = glyph.uvLoc.x;
////              uv.y = 1.0f - glyph.uvLoc.y;
////              uv.z = uv.x + glyph.uvSize.w;
////              uv.w = uv.y - glyph.uvSize.h;
////              uvs.push_back(uv);
////            }
////            std::cout << i << ":" << glyph.loc << std::endl;
////            advance += glyph.advance;
////          }
////        }
////        
////        if (locs.size() > 0)
////        {
////          vec2 s = size();
////          for (std::vector<vec4>::iterator itr = locs.begin(); itr != locs.end(); ++itr)
////            *itr /= vec4(s, s);
////          
////          mRenderSlot->lock();
////          mRenderSlot->uniforms()["Locs"].setValues(&locs[0], (int)locs.size());
////          mRenderSlot->uniforms()["Locs"].setInstanceDivisor(1);
////          
////          mRenderSlot->uniforms()["UVs"].setValues(&uvs[0], (int)uvs.size());
////          mRenderSlot->uniforms()["UVs"].setInstanceDivisor(1);
////          mRenderSlot->unlock();
////        }
////        mRenderSlot->setInstances((int)locs.size());
////        
////        //std::cout << "Updated Text Locations:" << std::endl;
////        //for (std::vector<vec4>::iterator itr = locs.begin(); itr != locs.end(); ++itr)
////        //  std::cout << *itr << std::endl;
////      }
//    }
//    
//    void initGlyphTable()
//    {
//      for (int i = 0; i < GLYPH_TABLE_SIZE; ++i)
//        mGlyphTable[i] = Glyph();
//    }
//
//    bool loadBitmapFont()
//    {
//      bool success = false;
//      if (mFontFile.exists())
//      {
//        std::fstream fileIn;
//        fileIn.open(mFontFile.path().c_str(), std::ios::in);
//        if (fileIn.is_open())
//        {
//          std::string line;
//          getline(fileIn, line);
//          
//          size_t pos = line.find(',');
//          if (pos != std::string::npos)
//          {
//            File atlasFile = mScene->directory() + line.substr(0, pos);
//            vec2 atlasSize = line.substr(pos+1);
//            
//            // Load the Texture Atlas of the Font
//            Texture *atlas = FelixEngine::GetGraphicSystem()->getTexture(atlasFile.name());
//            //atlas->setImageFile(atlasFile.path());
//            setAtlas(atlas);
//            
//            // Get the Glyph Information
//            initGlyphTable();
//            while (getline(fileIn, line))
//            {
//              int code;
//              Glyph glyph;
//              sscanf(line.c_str(), "%i , %f , %f , %f , %f , %f , %f , %f" ,
//                     &code, &glyph.advance, &glyph.uvLoc.x, &glyph.uvLoc.y,
//                     &glyph.loc.x, &glyph.loc.y, &glyph.size.x, &glyph.size.y);
//              
//              glyph.uvLoc /= atlasSize;
//              glyph.uvSize = glyph.size / atlasSize;
//              
//              if (code > 0 && code < GLYPH_TABLE_SIZE)
//                mGlyphTable[code] = glyph;
//            }
//            
//            fileIn.close();
//            success = setupRenderSlots();
//          }
//        }
//      }
//      return success;
//    }
//    
//    bool setupRenderSlots()
//    {
//      static Shader *fontShader = nullptr;
//      static Mesh *planeMesh = nullptr;
//      
//      bool success = false;
////      if (mRenderSlots)
////      {
////        if (!fontShader)
////        {
////          fontShader = FelixEngine::GetGraphicSystem()->getShader("FontShader");
////          fontShader->setFunctionToPart(TEXT_VERT_SHADER, SHADER_VERTEX);
////          fontShader->setFunctionToPart(TEXT_FRAG_SHADER, SHADER_FRAGMENT);
////        }
////        if (!planeMesh)
////        {
////          planeMesh = FelixEngine::GetGraphicSystem()->getMesh("PlaneMesh");
////          MeshLoader::LoadMeshPlane(planeMesh->getVertexBufferMap(), vec2(1.0f, 1.0f), vec2(0.0f, 0.0f));
////          planeMesh->setToLoad();
////        }
////        
////        mRenderSlots->addSlot();
////        mRenderSlot = mRenderSlots->back();
////        mRenderSlot->setMaterial(new Material());
////        mRenderSlot->material()->setShader(fontShader);
////        mRenderSlot->material()->textureMap().push();
////        mRenderSlot->material()->textureMap().back().setTexture(mAtlas);
////        mRenderSlot->setMesh(planeMesh);
////        mRenderSlot->setPass("UIPass");
////        mRenderSlot->setLayer(1);
////        mRenderSlot->blendState().setEquation(BLEND_EQ_ADD);
////        mRenderSlot->blendState().setSrc(BLEND_INPUT_SRC_ALPHA);
////        mRenderSlot->blendState().setDst(BLEND_INPUT_ONE_MINUS | BLEND_INPUT_SRC_ALPHA);
////        mRenderSlot->setInstances(0);
////        
////        success = true;
////      }
//      return success;
//    }
//    
//    File mFontFile;
//    
//    Glyph mGlyphTable[GLYPH_TABLE_SIZE];
//    
//    Texture *mAtlas;
//    RenderSlots *mRenderSlots;
////    RenderSlot *mRenderSlot;
//    
//    std::string mText;
//    bool mUpdateText;
//  };
//}

#endif /* UIText_h */
