/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef PLAYLIST_OP_H
#define PLAYLIST_OP_H

#include <PlaylistCommon.h>
#include <PlaylistUtils.h>

namespace pu {

class Playlist;
class Song;

///////////////////////////////////////////////////////////////////////////

class PU_API OpListener {
public:
  virtual ~OpListener() { }
  virtual bool beginOp(const char* opName, const Song& song) const { return true; }
  virtual bool beginOp(const char* opName) const { return true; }
  virtual bool endOp(bool success) const { return true; }
};

///////////////////////////////////////////////////////////////////////////

class Op {
public:
  const OpListener& listener() const { return playlistModule().opListener(); }
  const FileHandler& handler() const { return playlistModule().fileHandler(); }
};

class PU_API ConstSongOp : public Op {
public:
  virtual ~ConstSongOp() { }
  virtual bool operator()( const Song& ) const = 0;
};

class PU_API SongOp : public Op {
public:
  virtual ~SongOp() { }
  virtual bool operator()( Song& ) = 0;
};

///////////////////////////////////////////////////////////////////////////

class PU_API ConstSongsOp : public Op {
public:
  virtual ~ConstSongsOp() { }
  virtual bool operator()( const Song* first, Song* last ) const = 0;
};

class PU_API SongsOp : public Op {
public:
  virtual ~SongsOp() { }
  virtual bool operator()( Song* first, Song* last ) const = 0;
};

///////////////////////////////////////////////////////////////////////////

class PU_API ConstPlaylistOp : public Op {
public:
  virtual ~ConstPlaylistOp() { }
  virtual void operator()( const Playlist& ) const = 0;
};

class PU_API PlaylistOp : public Op {
public:
  virtual ~PlaylistOp() { }
  virtual void operator()( Playlist& ) const = 0;
};

}

#endif
