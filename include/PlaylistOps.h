#ifndef PLAYLIST_OPS_H
#define PLAYLIST_OPS_H

#include "PlaylistCommon.h"
#include "PlaylistOp.h"

namespace pu {

class File;
class FileTraits;
class Playlist;
class Song;

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
