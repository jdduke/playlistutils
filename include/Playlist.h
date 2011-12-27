#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "PlaylistCommon.h"

#include "pimpl.h"

#include <algorithm>
#include <string>

namespace pu {

class PU_API Song {
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

class PU_API Playlist {
public:
  Playlist();
  ~Playlist();

  void addSong(const Song&);
  void addSong(const Song&&);

  size_t songCount() const;

  const Song& song(size_t index) const;

  template<class Op> void apply( const Op& op ) const {
    std::for_each( begin(), end(), op);
  }

  template<class Op> void apply( Op& op ) {
    std::for_each( begin(), end(), op);
  }

private:

  Song* begin();
  const Song* begin() const;

  Song* end();
  const Song* end() const;

  DISALLOW_COPY_AND_ASSIGN(Playlist);

  class Impl;
  pimpl<Impl> m;
};

}

#endif
