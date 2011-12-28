/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "PlaylistCommon.h"

#include "pimpl.h"

#include <algorithm>
#include <string>
#include <vector>

namespace pu {

class Song {
public:
  Song(const Song& other)
    : mPath(other.mPath), mArtist(other.mArtist), mTitle(other.mTitle) { }
  Song(const Song&& other)
    : mPath(std::move(other.mPath)), mArtist(std::move(other.mArtist)), mTitle(std::move(other.mTitle)) { }
  Song(const std::string&& path, const std::string&& artist, const std::string&& title )
    : mPath(std::move(path)), mArtist(std::move(artist)), mTitle(std::move(title)) { }
  Song(const char* path, const char* artist = nullptr, const char* title = nullptr )
    : mPath(path), mArtist(artist), mTitle(title) { }

  const char* path()   const { return mPath.c_str(); }
  const char* artist() const { return mArtist.c_str(); }
  const char* title()  const { return mTitle.c_str(); }

protected:
  std::string mPath, mArtist, mTitle;
};

class PU_API SongComparator {
public:
  SongComparator() { }
  virtual ~SongComparator() { }
  virtual bool operator()( const Song& song1, const Song& song2 ) const {
    return std::strcmp( song1.title(), song2.title() ) < 0;
  }
};

class Playlist {
public:
  Playlist() { }
  ~Playlist() { }

  void release() {
    delete this;
  }

  void addSong(const Song&);
  void addSong(const Song&&);

  size_t songCount() const;

  const Song& song(size_t index) const;

  template<class Op> void apply( const Op& op ) const {
    op( first(), last() );
  }

  template<class Op> void apply( const Op& op ) {
    op( first(), last() );
  }

  template<class Op> void applyOp( const Op& op ) const {
    std::for_each( first(), last(), op);
  }

  template<class Op> void applyOp( const Op& op ) {
    std::for_each( first(), last(), op);
  }

private:
  DISALLOW_COPY_AND_ASSIGN(Playlist);

  inline Song* first() {  return &(*std::begin(mSongs)); }
  inline const Song* first() const {  return &(*std::begin(mSongs)); }

  inline Song* last() { return &(*std::end(mSongs)); }
  inline const Song* last() const {  return &(*std::end(mSongs)); }

  std::vector<Song> mSongs;
  /*
  class Impl;
  common::pimpl<Impl> m;
  */
};

PU_API Playlist* createPlaylist(const char* name);

}

#endif
