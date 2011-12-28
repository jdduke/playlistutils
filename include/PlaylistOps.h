#ifndef PLAYLIST_OPS_H
#define PLAYLIST_OPS_H

#include "PlaylistCommon.h"
#include "PlaylistOp.h"
#include "PlaylistFileUtils.h"

namespace pu {

class Playlist;
class Song;


class PU_API CopySongOp : public ConstSongOp {
public:
  CopySongOp( const char* destDir, const FileTraits& traits, const SongOpListener& listener = SongOpListener() );

  virtual bool operator()( const Song& song ) const;

private:

  inline std::string opName( const File& file ) const;

  std::string mDestDir;
  const FileTraits& mTraits;
  SongOpListener mListener;
};


class PU_API MoveSongOp : public SongOp {
public:
  MoveSongOp( const char* destDir, const FileTraits& traits, const SongOpListener& listener = SongOpListener() );

  virtual bool operator()( const Song& song );

private:

  inline std::string opName( const File& file ) const;

  std::string mDestDir;
  const FileTraits& mTraits;
  SongOpListener mListener;
};


class PU_API DeleteSongOp : public SongOp {
public:
  DeleteSongOp( const FileTraits& traits, const SongOpListener& listener = SongOpListener() );

  virtual bool operator()( const Song& song );

private:

  inline std::string opName( const File& file ) const;

  const FileTraits& mTraits;
  SongOpListener mListener;
};

}

#endif
