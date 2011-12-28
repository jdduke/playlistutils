/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef PLAYLIST_OP_H
#define PLAYLIST_OP_H

#include "PlaylistCommon.h"
#include "Playlist.h"

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

template< class Op >
class PlaylistSongOp : public PlaylistOp {
public:
  PlaylistSongOp( const Op& op = Op() ) : mOp( op ) { }
  void operator()( Playlist& p ) const {
    p.applyOp( mOp );
  }

private:
  DISALLOW_COPY_AND_ASSIGN(PlaylistSongOp);
  const Op& mOp;
};

template< class Op >
class ConstPlaylistSongOp : public ConstPlaylistOp {
public:
  ConstPlaylistSongOp( const Op& op = Op() ) : mOp( op ) { }
  void operator()( const Playlist& p ) const {
    p.applyOp( mOp );
  }

private:
  DISALLOW_COPY_AND_ASSIGN(ConstPlaylistSongOp);
  const SongOp& mOp;
};

///////////////////////////////////////////////////////////////////////////

template< class Op >
class PlaylistSongsOp : public PlaylistOp {
public:
  PlaylistSongsOp( const Op& op = Op() ) : mOp( op ) { }
  void operator()( Playlist& p ) const {
    p.apply( mOp );
  }

private:
  DISALLOW_COPY_AND_ASSIGN(PlaylistSongsOp);
  const Op& mOp;
};

template< class Op >
class ConstPlaylistSongsOp : public ConstPlaylistOp {
public:
  ConstPlaylistSongsOp( const Op& op = Op() ) : mOp( op ) { }
  void operator()( const Playlist& p ) const {
    p.apply( mOp );
  }

private:
  DISALLOW_COPY_AND_ASSIGN(ConstPlaylistSongsOp);
  const Op& mOp;
};

///////////////////////////////////////////////////////////////////////////

}

#endif
