#ifndef PLAYLIST_OP_H
#define PLAYLIST_OP_H

#include "PlaylistCommon.h"
#include "Playlist.h"

namespace pu {

class Playlist;
class Song;

class PU_API SongOpListener {
public:
  virtual ~SongOpListener() { }
  virtual void beginOp(const char* opName) const { }
  virtual void endOp(bool success) const { }
};

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

class PU_API ConstPlaylistOp {
public:
  virtual ~ConstPlaylistOp() { }
  virtual bool operator()( const Playlist& ) const = 0;
};

class PU_API PlaylistOp {
public:
  virtual ~PlaylistOp() { }
  virtual bool operator()( Playlist& ) = 0;
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
  SongOp mOp;
  SongOpListener mOpListener;
};

template< class SongOp >
class PU_API ConstPlaylistSongOp : public ConstPlaylistOp {
public:
  ConstPlaylistSongOp( ConstSongOp& op ) : mOp( op ) { }
  void operator()( const Playlist& p ) {
    p.apply( op );
  }

private:
  DISALLOW_COPY_AND_ASSIGN(ConstPlaylistSongOp);
  const SongOp mOp;
  const SongOpListener mOpListener;
};

}

#endif
