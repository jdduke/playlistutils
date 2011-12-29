/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef PLAYLIST_UTILITIES_H
#define PLAYLIST_UTILITIES_H

#include <PlaylistCommon.h>
#include <algorithm>
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
      op( *mSongs.begin(), songCount() );
    }
  }

  template<class Op> inline void apply( const Op& op ) {
    if (songCount() > 0 ) {
      op( *mSongs.begin(), songCount() );
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

typedef std::unique_ptr<Playlist,Releaser> PlaylistPtr;

///////////////////////////////////////////////////////////////////////////

class PU_API PlaylistImporter {
public:
  virtual ~PlaylistImporter() { }
  virtual PlaylistPtr operator()(const char* fileName) const = 0;
};

class PU_API PlaylistExporter {
public:
  virtual ~PlaylistExporter() { }
  virtual bool operator()(const Playlist& playlist, const char* fileName) const = 0;
};

///////////////////////////////////////////////////////////////////////////

class PU_API PlaylistManager {
public:
  virtual ~PlaylistManager() { }

  virtual PlaylistPtr importFromFile(const char* fileName) const = 0;
  virtual bool        exportToFile(const Playlist& playlist, const char* fileName) const = 0;

  virtual bool registerImporter(PlaylistImporter* importer, const char* extension) = 0;
  virtual bool supportsImport(const char* extension) const = 0;

  virtual bool registerExporter(PlaylistExporter* exporter, const char* extension) = 0;
  virtual bool supportsExport(const char* extension) const = 0;
};

///////////////////////////////////////////////////////////////////////////

class PU_API SongComparator {
public:
  SongComparator() { }
  virtual ~SongComparator() { }
  virtual bool operator()( const Song& song1, const Song& song2 ) const {
    return std::strcmp( song1.title(), song2.title() ) < 0;
  }
};

///////////////////////////////////////////////////////////////////////////

PU_API PlaylistManager& playlistManager();

}

#endif
