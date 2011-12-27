#ifndef PLAYLIST_OP_H
#define PLAYLIST_OP_H

#include "Playlist.h"

namespace pu {

class Playlist;
class Song;

class ConstSongOp {
public:
  virtual ~SongOp() { }
  virtual void operator()( const Song& ) const = 0;
};

class SongOp {
public:
  virtual ~SongOp() { }
  virtual void operator()( Song& ) = 0;
};

class Const PlaylistOp {
public:
  virtual ~PlaylistOp() { }
  virtual void operator()( const Playlist& ) const = 0;
};

class PlaylistOp {
public:
  virtual ~PlaylistOp() { }
  virtual void operator()( Playlist& ) = 0;
};

template< class SongOp >
class PlaylistSongOp : public PlaylistOp {
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
class ConstPlaylistSongOp : public ConstPlaylistOp {
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
