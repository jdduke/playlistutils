/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef PLAYLIST_UTILITIES_H
#define PLAYLIST_UTILITIES_H

#include <PlaylistCommon.h>
#include <algorithm>

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

class PU_API SongComparator {
public:
  SongComparator() { }
  virtual ~SongComparator() { }
  virtual bool operator()( const Song& song1, const Song& song2 ) const {
    return std::strcmp( song1.title(), song2.title() ) < 0;
  }
};

PU_API Playlist*        createPlaylist(const char* name);

}

#endif
