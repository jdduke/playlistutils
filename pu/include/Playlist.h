/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "PlaylistUtilities.h"


#include <string>
#include <vector>

namespace pu {

class SongImpl : public Song {
public:
  SongImpl(const SongImpl& other)
    : mPath(other.mPath), mArtist(other.mArtist), mTitle(other.mTitle) { }
  SongImpl(const SongImpl&& other)
    : mPath(std::move(other.mPath)), mArtist(std::move(other.mArtist)), mTitle(std::move(other.mTitle)) { }
  SongImpl(const std::string&& path, const std::string&& artist, const std::string&& title )
    : mPath(std::move(path)), mArtist(std::move(artist)), mTitle(std::move(title)) { }
  SongImpl(const char* path, const char* artist = nullptr, const char* title = nullptr )
    : mPath(path), mArtist(artist), mTitle(title) { }

  const char* path()   const { return mPath.c_str(); }
  const char* artist() const { return mArtist.c_str(); }
  const char* title()  const { return mTitle.c_str(); }

protected:
  std::string mPath, mArtist, mTitle;
};

class PlaylistImpl : public Playlist {
public:
  PlaylistImpl() { }
  ~PlaylistImpl() { }

  void release() {
    delete this;
  }

  void        addSong(Song*);
  size_t      songCount() const;
  const Song& song(size_t index) const;

private:
  DISALLOW_COPY_AND_ASSIGN(PlaylistImpl);

  inline Song* first() {  return (*std::begin(mSongs)).get(); }
  inline const Song* first() const {  return (*std::begin(mSongs)).get(); }

  inline Song* last() { return first() + songCount(); }
  inline const Song* last() const {  return first() + songCount(); }

  std::vector< std::unique_ptr<Song> > mSongs;
  /*
  class Impl;
  common::pimpl<Impl> m;
  */
};

}

#endif
