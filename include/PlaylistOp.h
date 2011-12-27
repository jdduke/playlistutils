#ifndef PLAYLIST_OP_H
#define PLAYLIST_OP_H

#include "PlaylistCommon.h"
#include "Playlist.h"

namespace pu {

class Playlist;
class Song;

class PU_API ConstSongOp {
public:
  virtual ~SongOp() { }
  virtual void operator()( const Song& ) const = 0;
};

class PU_API SongOp {
public:
  virtual ~SongOp() { }
  virtual void operator()( Song& ) = 0;
};

class PU_API ConstPlaylistOp {
public:
  virtual ~PlaylistOp() { }
  virtual void operator()( const Playlist& ) const = 0;
};

class PU_API PlaylistOp {
public:
  virtual ~PlaylistOp() { }
  virtual void operator()( Playlist& ) = 0;
};

template< class SongOp >
class PU_API PlaylistSongOp : public PlaylistOp {
public:
  PlaylistSongOp( SongOp& op ) : mOp( op ) { }
  void operator()( Playlist& p ) {
    p.apply( op );
  }

private:
  DISALLOW_COPY_AND_ASSIGN(PlaylistSongOp);
  SongOp& mOp;
}

template< class SongOp >
class PU_API ConstPlaylistSongOp : public ConstPlaylistOp {
public:
  ConstPlaylistSongOp( ConstSongOp& op ) : mOp( op ) { }
  void operator()( const Playlist& p ) {
    p.apply( op );
  }

private:
  DISALLOW_COPY_AND_ASSIGN(ConstPlaylistSongOp);
  const SongOp& mOp;
}

}

#endif
