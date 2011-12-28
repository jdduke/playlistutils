#ifndef PLAYLIST_FILE_UTILS_H
#define PLAYLIST_FILE_UTILS_H

#include "PlaylistCommon.h"

#include <string>

namespace pu {

class PU_API FileTraits {
public:
  virtual ~FileTraits() { }
  virtual bool copy(const char* sourcePath, const char* destPath) const   = 0;
  virtual bool rename(const char* sourcePath, const char* destName) const = 0;
  virtual bool remove(const char* sourcePath) const                       = 0;
  virtual bool move(const char* sourcePath, const char* destPath) const   = 0;
};

class PU_API File {
public:
  File( const char* source, const FileTraits& traits )
      : mDir( fileDir(source) ),
      mName( fileName(source) ),
      mPath( source ),
      mTraits( traits ) { }

  bool rename( const char* destName ) {
    if( mTraits.copy( mPath.c_str(), path(mPath, destName).c_str() ) ) {
      set( mDir.c_str(), destName );
      return true;
    }
    return false;
  }

  bool copy( const char* destDir ) const {
    return mTraits.copy( mPath.c_str(), path(destDir, mName).c_str() );
  }

  bool remove() {
    if( mTraits.remove( mPath.c_str() ) ) {
      invalidate();
      return true;
    }
    return false;
  }

  bool move( const char* destDir ) {
    if( mTraits.move( mPath.c_str(), path(destDir, mName).c_str() ) ) {
      set(destDir, mName.c_str());
      return true;
    }
    return false;
  }

  const char* getDir() const  { return mDir.c_str();  }
  const char* getName() const { return mName.c_str(); }
  const char* getPath() const { return mPath.c_str(); }

private:
  DISALLOW_COPY_AND_ASSIGN(File);

  bool isValid() {
    return !(mDir.empty() || mName.empty() || mPath.empty());
  }

  void set( const char* dir, const char* name )
  {
    mDir = dir;
    mName = name;
    mPath = path(dir, name);
  }

  bool invalidate() {
    set("", "");
  }

  inline static std::string path( const std::string& dir, const std::string& name ) {
    return path(dir.c_str(), name.c_str());
  }

  inline static std::string path( const std::string& dir, const char* name ) {
    return path(dir.c_str(), name);
  }

  inline static std::string path( const char* dir, const std::string& name ) {
    return path(dir, name.c_str());
  }

  inline static std::string path( const char* dir, const char* name ) {
    std::string filePath(dir);
    filePath.append("/");
    filePath.append(name);
    return filePath;
  }

  inline static std::string fileDir( const char* path ) {
    std::string filePath(path);
    return filePath.substr(0, filePath.find_last_of("/\\") + 1);
  }

  inline static std::string fileName( const char* path ) {
    std::string filePath(path);
    size_t p0 = filePath.find_last_of("/\\") + 1;
    return filePath.substr(p0);
  }

  std::string mDir;
  std::string mName;
  std::string mPath;
  const FileTraits& mTraits;
};

}

#endif
