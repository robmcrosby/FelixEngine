//
//  FileSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 12/1/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef FileSystem_h
#define FileSystem_h

#include "System.h"
#include "XMLTree.h"
#include "Image.h"
#include <iomanip>

namespace fx
{
  class File;
  typedef std::list<File> Directory;
  
  enum FILE_TYPE
  {
    FILE_DIR,
    FILE_TEXT,
    FILE_XML,
    FILE_MESH,
    FILE_SHADER,
    FILE_IMAGE,
    FILE_OTHER,
  };
  
  /**
   *
   */
  class File
  {
  public:
    File(const std::string &url = "");
    
    std::string name() const;
    std::string path() const;
    std::string extension() const;
    
    bool exists() const;
    
    bool load(Directory &dir) const;
    bool load(std::string &text) const;
    bool load(XMLTree &tree) const;
    
    bool createDirectory() const;
    bool write(const std::string &text) const;
    bool append(const std::string &text) const;
    
    bool remove() const;
    
    bool addDirectory(const std::string &name) const;
    
    FILE_TYPE type() const {return mType;}
    std::string url() const {return mUrl;}
    
    File operator+(const std::string &path) const;
    File upperDirectory() const;
    
  public:
    friend std::ostream &operator<<(std::ostream &os, const File &f)
    {
      os << std::left;
      os << std::setw(8) << TypeString(f.type()) << f.name();
      return os << std::right;
    }

  public:
    static std::string TypeString(FILE_TYPE type);
    static FILE_TYPE TypeFromExtension(const std::string &ext);
    
  private:
    void setType();
    
  private:
    FILE_TYPE mType;
    std::string mUrl;
  };
  
  /**
   *
   */
  class FileSystem: public System
  {
  public:
    virtual ~FileSystem() {}
    virtual File getDocuments() = 0;
    
  public:
    static bool LoadText(std::string &buffer, const std::string &filePath);
    static std::string LoadText(const std::string &filePath);
    
    static long Read(unsigned int *buffer, long size, std::istream &is);
    static long Read(unsigned int &i, std::istream &is) {return Read(&i, 1, is);}
    
    static long Read(int *buffer, long size, std::istream &is) {return Read((unsigned int*)buffer, size, is);}
    static long Read(int &i, std::istream &is) {return Read((unsigned int*)&i, 1, is);}
    
    static long Read(float *buffer, long size, std::istream &is) {return Read((unsigned int*)buffer, size, is);}
    static long Read(float &f, std::istream &is) {return Read((unsigned int*)&f, 1, is);}
    
    static File GetResources();
    static File GetDocuments();
    
  protected:
    static FileSystem *sInstance;
  };
}

#endif /* FileSystem_h */
