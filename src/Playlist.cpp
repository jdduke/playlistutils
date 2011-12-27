#include "Playlist.h"

#include "pimpl_impl.h"

#include <assert.h>
#include <vector>

namespace pu {

class Playlist::Impl {
public:

  inline void add(const Song& song) {
    mSongs.push_back(song);
  }

  inline void add(const Song&& song) {
    mSongs.push_back(song);
  }

  inline Song* begin() {
    return &(*mSongs.begin());
  }

  inline const Song* begin() const {
    return &(*mSongs.begin());
  }

  inline Song* end() {
    return &(*mSongs.end());
  }

  inline const Song* end() const {
    return &(*mSongs.end());
  }

  std::vector<Song> mSongs;
};

}

using namespace pu;

Playlist::Playlist() { }

Playlist::~Playlist() { }

void Playlist::addSong(const Song& song) {
  m->add(song);
}

void Playlist::addSong(const Song&& song) {
  m->add(song);
}

size_t Playlist::songCount() const {
  return m->mSongs.size();
}

const Song& Playlist::song(size_t index) const {
  assert(index < songCount());
  return m->mSongs[index];
}

Song* Playlist::begin() {
  return m->begin();
}

const Song* Playlist::begin() const {
  return m->begin();
}

Song* Playlist::end() {
  return m->end();
}

const Song* Playlist::end() const {
  return m->end();
}
