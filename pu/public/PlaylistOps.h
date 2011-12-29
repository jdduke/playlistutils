/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef PLAYLIST_OPS_H
#define PLAYLIST_OPS_H

#include <PlaylistUtilities.h>
#include <PlaylistOp.h>

#include <string>

namespace pu {

class File;
class FileTraits;

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

class PU_API CopySongOp : public ConstSongOp {
public:
  CopySongOp( const char* destDir, const FileTraits& traits, const OpListener& listener = OpListener() );

  bool operator()( const Song& song ) const;

private:

  inline std::string opName( const File& file ) const;

  const char* mDestDir;
  const FileTraits& mTraits;
  OpListener mListener;
};

///////////////////////////////////////////////////////////////////////////

class PU_API MoveSongOp : public SongOp {
public:
  MoveSongOp( const char* destDir, const FileTraits& traits, const OpListener& listener = OpListener() );

  bool operator()( Song& song );

private:

  inline std::string opName( const File& file ) const;

  const char* mDestDir;
  const FileTraits& mTraits;
  OpListener mListener;
};

///////////////////////////////////////////////////////////////////////////

class PU_API DeleteSongOp : public SongOp {
public:
  DeleteSongOp( const FileTraits& traits, const OpListener& listener = OpListener() );

  bool operator()( Song& song );

private:

  inline std::string opName( const File& file ) const;

  const FileTraits& mTraits;
  OpListener mListener;
};

///////////////////////////////////////////////////////////////////////////

class PU_API SortSongsOp : public SongsOp {
public:
  SortSongsOp( const SongComparator& compare = SongComparator(),  const OpListener& listener = OpListener() );

  bool operator()( Song* first, Song* last ) const;

private:
  SongComparator mCompare;
  OpListener     mListener;
};

///////////////////////////////////////////////////////////////////////////

}

#endif
