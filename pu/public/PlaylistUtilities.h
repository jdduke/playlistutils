/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef PLAYLIST_UTILITIES_H
#define PLAYLIST_UTILITIES_H

#include <PlaylistCommon.h>

#include <iosfwd>
#include <algorithm>
#include <string>
#include <vector>

namespace pu {

///////////////////////////////////////////////////////////////////////////

class Song {
public:
  enum {
    INVALID_LENGTH = ((size_t)-1)
  };

  Song() : mLength(INVALID_LENGTH) { }
  Song(const char* path);

  size_t      length() const { return mLength; }
  const char* path()   const { return mPath.c_str(); }
  const char* artist() const { return mArtist.c_str(); }
  const char* title()  const { return mTitle.c_str(); }

protected:
  size_t mLength;
  std::string mPath, mArtist, mTitle;
};

///////////////////////////////////////////////////////////////////////////

class Playlist {
public:

  ///////////////////////////////////////////////////////////////////////////

  void release() {
    delete this;
  }

  void addSong(const char* path) {
    mSongs.emplace_back(path);
  }
  void addSong(const Song& song) {
    mSongs.push_back(song);
  }
  void addSong(const Song&& song) {
    mSongs.push_back(std::move(song));
  }
  size_t songCount() const {
    return mSongs.size();
  }
  const Song& song(size_t index) const {
    return mSongs[index];
  }

  ///////////////////////////////////////////////////////////////////////////

  template<class Op> inline void apply( const Op& op ) const {
    if (songCount() > 0 ) {
      op( mSongs.data(), mSongs.data() + songCount() );
    }
  }

  template<class Op> inline void apply( const Op& op ) {
    if (songCount() > 0 ) {
      op( mSongs.data(), mSongs.data() + songCount() );
    }
  }

  template<class Op> inline void applyOp( const Op& op ) const {
    std::for_each( mSongs.begin(), mSongs.end(), op);
  }

  template<class Op> inline void applyOp( const Op& op ) {
    std::for_each( mSongs.begin(), mSongs.end(), op);
  }

  ///////////////////////////////////////////////////////////////////////////

protected:
  Playlist() { }
  DISALLOW_COPY_AND_ASSIGN(Playlist);

  typedef std::vector< Song > Songs;
  Songs mSongs;
};

class Releaser {
public:
  template< typename T>
  void operator()(T* ptr) {
    if( ptr ) { ptr->release(); }
  }
};

typedef std::unique_ptr<Playlist,Releaser> PlaylistPtr;

///////////////////////////////////////////////////////////////////////////

class PU_API PlaylistImporter {
public:
  virtual ~PlaylistImporter() { }
  virtual void release() = 0;
  virtual PlaylistPtr operator()(const char* fileName) const = 0;
};

class PU_API PlaylistExporter {
public:
  virtual ~PlaylistExporter() { }
  virtual void release() = 0;
  virtual bool operator()(const Playlist& playlist, const char* fileName) const = 0;
};

///////////////////////////////////////////////////////////////////////////

class PU_API FileHandler {
public:
  virtual ~FileHandler() { }
  virtual bool copy(  const char* sourcePath, const char* destPath) const = 0;
  virtual bool rename(const char* sourcePath, const char* destName) const = 0;
  virtual bool move(  const char* sourcePath, const char* destPath) const = 0;
  virtual bool remove(const char* sourcePath) const                       = 0;
  virtual bool exists(const char* sourcePath) const                       = 0;
};

class PU_API XmlHandler {
public:
  virtual ~XmlHandler() { }
  virtual void load( std::ifstream& ifs ) = 0;
  virtual void nextElement( )             = 0;
  virtual void hasNextElement( ) const    = 0;
};

class PU_API LogHandler {
public:
  enum LogLevel {
    INFO = 0,
    WARNING,
    ERROR,
    CRITICAL,
    NUM_LEVELS
  };
  virtual ~LogHandler() { }
  virtual void operator()(const char* msg, LogLevel level = INFO) const = 0;
};

///////////////////////////////////////////////////////////////////////////

class PU_API PlaylistModule {
public:
  virtual ~PlaylistModule() { }

  virtual PlaylistPtr importFromFile(const char* fileName) const = 0;
  virtual bool        exportToFile(const Playlist& playlist, const char* fileName) const = 0;

  virtual bool registerImporter(PlaylistImporter* importer, const char* extension) = 0;
  virtual bool supportsImport(const char* extension) const = 0;

  virtual bool registerExporter(PlaylistExporter* exporter, const char* extension) = 0;
  virtual bool supportsExport(const char* extension) const = 0;

  virtual void setFileHandler(FileHandler*)      = 0;
  virtual const FileHandler& fileHandler() const = 0;

  virtual void setXmlHandler(XmlHandler*)        = 0;
  virtual const XmlHandler& xmlHandler() const   = 0;

  virtual void setLogHandler(LogHandler*)        = 0;
  virtual const LogHandler& logHandler() const   = 0;
};

////////////////////////////////////////////////////////f///////////////////

class PU_API SongComparator {
public:
  SongComparator() { }
  virtual ~SongComparator() { }
  virtual bool operator()( const Song& song1, const Song& song2 ) const {
    return std::strcmp( song1.title(), song2.title() ) < 0;
  }
};

///////////////////////////////////////////////////////////////////////////

PU_API PlaylistModule& playlistModule();

}

#endif
