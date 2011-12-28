#include "Playlist.h"

#include "pimpl_impl.h"

#include <assert.h>

namespace pu {

Playlist* createPlaylist(const char* name) {
  return new Playlist();
}

void releasePlaylist( Playlist* playlist ) {
  delete playlist;
}

/*
class Playlist::Impl {
public:

  inline void add(const Song& song) {
    mSongs.push_back(song);
  }

  inline void add(const Song&& song) {
    mSongs.push_back(song);
  }

  inline Song* first() {
    return &(*std::begin(mSongs));
  }

  inline const Song* first() const {
    return &(*std::begin(mSongs));
  }

  inline Song* last() {
    return &(*std::end(mSongs));
  }

  inline const Song* last() const {
    return &(*std::end(mSongs));
  }

  std::vector<Song> mSongs;
};
*/

}

using namespace pu;

void Playlist::addSong(const Song& song) {
  //m->add(song);
  mSongs.push_back(song);
}

void Playlist::addSong(const Song&& song) {
  mSongs.push_back(song);//m->add(song);
}

size_t Playlist::songCount() const {
  return mSongs.size();
}

const Song& Playlist::song(size_t index) const {
  assert(index < songCount());
  return mSongs[index];
}

/*
Song* Playlist::first() {
  return &(*std::begin(mSongs));
  //return m->first();
}

const Song* Playlist::first() const {
  return &(*std::begin(mSongs));
  //return m->first();
}

Song* Playlist::last() {
  return &(*std::end(mSongs));
  //return m->last();
}

const Song* Playlist::last() const {
  return &(*std::end(mSongs));
  //return m->last();
}
*/