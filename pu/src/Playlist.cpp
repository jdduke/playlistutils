/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#include "Playlist.h"

#include "pimpl_impl.h"

#include <assert.h>

namespace pu {

Playlist* createPlaylist(const char* name) {
  return new PlaylistImpl();
}

void releasePlaylist( Playlist* playlist ) {
  playlist->release();
}

}

using namespace pu;

void PlaylistImpl::addSong(Song* song) {
  //m->add(song);
  if (song) {
    mSongs.push_back( std::unique_ptr<Song>(song) );
  }
}

size_t PlaylistImpl::songCount() const {
  return mSongs.size();
}

const Song& PlaylistImpl::song(size_t index) const {
  assert(index < songCount());
  return *mSongs[index];
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