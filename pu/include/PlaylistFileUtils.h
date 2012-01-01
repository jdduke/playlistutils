/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef PLAYLIST_FILE_UTILS_H
#define PLAYLIST_FILE_UTILS_H

#include "PlaylistUtilities.h"

#include <string>

namespace pu {

inline bool beginsWith( const std::string& path, const std::string& beginning ) {
  if (path.length() >= beginning.length()) {
    return (0 == path.compare(0, beginning.length(), beginning));
  } else {
    return false;
  }
}

inline bool endsWith( const std::string& path, const std::string& ending ) {
  if (path.length() >= ending.length()) {
    return (0 == path.compare(path.length() - ending.length(), ending.length(), ending));
  } else {
    return false;
  }
}

inline bool endsWith( const std::string& path, const char* ending ) {
  return endsWith(path, std::string(ending));
}

inline bool beginsWith( const std::string& path, const char* beginning ) {
  return beginsWith(path, std::string(beginning));
}

inline void fileData(const std::string& path, size_t& length, std::string& artist, std::string& title) {
  // TODO: Parse mp3 tag
}

class File {
public:
  File( const char* source, const FileHandler& handler = playlistModule().fileHandler() )
      : mDir( fileDir(source) ),
      mName( fileName(source) ),
      mPath( source ),
      mSize( handler.size(source) ),
      mHandler( handler ) { }

  bool rename( const char* destName ) {
    if( mHandler.copy( mPath.c_str(), path(mPath, destName).c_str() ) ) {
      set( mDir.c_str(), destName );
      return true;
    }
    return false;
  }

  bool copy( const char* destDir ) const {
    return mHandler.copy( mPath.c_str(), path(destDir, mName).c_str() );
  }

  bool remove() {
    if( mHandler.remove( mPath.c_str() ) ) {
      invalidate();
      return true;
    }
    return false;
  }

  bool move( const char* destDir ) {
    if( mHandler.move( mPath.c_str(), path(destDir, mName).c_str() ) ) {
      set(destDir, mName.c_str());
      return true;
    }
    return false;
  }

  const char* getDir()  const { return mDir.c_str();  }
  const char* getName() const { return mName.c_str(); }
  const char* getPath() const { return mPath.c_str(); }
  size_t      getSize() const { return mSize; }

  inline static std::string fileDir( const char* path ) {
    std::string filePath(path);
    return filePath.substr(0, filePath.find_last_of("/\\") + 1);
  }

  inline static std::string fileName( const char* path ) {
    std::string filePath(path);
    auto p0 = filePath.find_last_of("/\\") + 1;
    return filePath.substr(p0);
  }

  inline static std::string fileExtension( const char* path ) {
    std::string file( fileName(path) );
    auto p0 = file.find_last_of(".");
    if ( p0 != std::string::npos ) {
      return file.substr(p0 + 1);
    }
    return "";
  }


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

  void invalidate() {
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

  std::string mDir;
  std::string mName;
  std::string mPath;
  size_t      mSize;
  const FileHandler& mHandler;
};

static inline std::string extensionOf( const char* fileName ) {
  return File::fileExtension( fileName );
}

}

#endif
