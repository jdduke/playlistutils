/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef PLAYLIST_OP_H
#define PLAYLIST_OP_H

#include <PlaylistCommon.h>

namespace pu {

class Playlist;
class Song;

///////////////////////////////////////////////////////////////////////////

class PU_API OpListener {
public:
  virtual ~OpListener() { }
  virtual void beginOp(const char* opName) const { }
  virtual void endOp(bool success) const { }
};

///////////////////////////////////////////////////////////////////////////

class PU_API ConstSongOp {
public:
  virtual ~ConstSongOp() { }
  virtual bool operator()( const Song& ) const = 0;
};

class PU_API SongOp {
public:
  virtual ~SongOp() { }
  virtual bool operator()( Song& ) = 0;
};

///////////////////////////////////////////////////////////////////////////

class PU_API ConstSongsOp {
public:
  virtual ~ConstSongsOp() { }
  virtual bool operator()( const Song* first, const Song* last ) const = 0;
};

class PU_API SongsOp {
public:
  virtual ~SongsOp() { }
  virtual bool operator()( Song* first, Song* last ) const = 0;
};

///////////////////////////////////////////////////////////////////////////

class PU_API ConstPlaylistOp {
public:
  virtual ~ConstPlaylistOp() { }
  virtual void operator()( const Playlist& ) const = 0;
};

class PU_API PlaylistOp {
public:
  virtual ~PlaylistOp() { }
  virtual void operator()( Playlist& ) const = 0;
};

///////////////////////////////////////////////////////////////////////////

PU_API ConstSongOp*     createConstSongOp(const char* name);
PU_API SongOp*          createSongOp(const char* name);
PU_API ConstSongsOp*    createConstSongsOp(const char* name);
PU_API SongsOp*         createSongsOp(const char* name);
PU_API ConstPlaylistOp* createConstPlaylistOp(const char* name);
PU_API PlaylistOp*      createPlaylistOp(const char* name);

}

#endif
