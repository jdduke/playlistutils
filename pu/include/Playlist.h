/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "PlaylistUtilities.h"

#include <assert.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace pu {

typedef std::unique_ptr<Song> SongPtr;
typedef std::unique_ptr<PlaylistImporter> PlaylistImporterPtr;
typedef std::unique_ptr<PlaylistExporter> PlaylistExporterPtr;

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

  void        addSong(Song* song)           { if (song) mSongs.push_back( SongPtr(song) ); }
  size_t      songCount() const        { return mSongs.size(); }
  const Song& song(size_t index) const { assert(index < songCount()); return *mSongs[index]; }

private:
  DISALLOW_COPY_AND_ASSIGN(PlaylistImpl);

  inline       Song* first()       {  return songCount() > 0 ? (*std::begin(mSongs)).get() : nullptr; }
  inline const Song* first() const {  return songCount() > 0 ? (*std::begin(mSongs)).get() : nullptr; }

  inline       Song* last()       { return first() + songCount(); }
  inline const Song* last() const { return first() + songCount(); }

  std::vector< SongPtr > mSongs;
  /*
  class Impl;
  common::pimpl<Impl> m;
  */
};


class PlaylistManagerImpl : public PlaylistManager {
public:
  PlaylistManagerImpl() { loadDefaults(); }

  PlaylistPtr importFromFile(const char* fileName) const;
  bool        exportToFile(const Playlist& playlist, const char* fileName) const;

  bool registerImporter(PlaylistImporter* importer, const char* extension);
  bool supportsImport(const char* extension) const;

  bool registerExporter(PlaylistExporter* exporter, const char* extension);
  bool supportsExport(const char* extension) const;

protected:

  void loadDefaults();

  const PlaylistImporter* getImporter(const std::string& extension) const;
  const PlaylistExporter* getExporter(const std::string& extension) const;

  typedef std::unordered_map<std::string, PlaylistExporterPtr> ExporterMap;
  typedef std::unordered_map<std::string, PlaylistImporterPtr> ImporterMap;

  ExporterMap mExporters;
  ImporterMap mImporters;
};

}

#endif
