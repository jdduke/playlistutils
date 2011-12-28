/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef PLAYLIST_UTILITIES_H
#define PLAYLIST_UTILITIES_H

#include <PlaylistCommon.h>
#include <algorithm>
#include <memory>

namespace pu {

class PU_API Song {
public:
  virtual ~Song() { }
  virtual const char* artist() const { return ""; }
  virtual const char* path()   const { return ""; }
  virtual const char* title()  const { return ""; }
};

class PU_API Playlist {
public:
  ///////////////////////////////////////////////////////////////////////////

  virtual ~Playlist() { }

  virtual void release() = 0;

  virtual void        addSong(Song*) = 0;
  virtual size_t      songCount() const = 0;
  virtual const Song& song(size_t index) const = 0;

  ///////////////////////////////////////////////////////////////////////////

  template<class Op> inline void apply( const Op& op ) const {
    op( first(), last() );
  }

  template<class Op> inline void apply( const Op& op ) {
    op( first(), last() );
  }

  template<class Op> inline void applyOp( const Op& op ) const {
    std::for_each( first(), last(), op);
  }

  template<class Op> inline void applyOp( const Op& op ) {
    std::for_each( first(), last(), op);
  }

protected:
  virtual Song* first()             = 0;
  virtual const Song* first() const = 0;

  virtual Song* last()              = 0;
  virtual const Song* last() const  = 0;

};

typedef std::unique_ptr<Playlist,Releaser> PlaylistPtr;

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

class PU_API SongComparator {
public:
  SongComparator() { }
  virtual ~SongComparator() { }
  virtual bool operator()( const Song& song1, const Song& song2 ) const {
    return std::strcmp( song1.title(), song2.title() ) < 0;
  }
};

PU_API PlaylistManager& playlistManager();

}

#endif
