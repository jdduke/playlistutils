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

class SongImpl;
class PlaylistImpl;

typedef std::unique_ptr<Song> SongPtr;
typedef std::unique_ptr<PlaylistImpl,Releaser> PlaylistImplPtr;
typedef std::unique_ptr<PlaylistImporter>      PlaylistImporterPtr;
typedef std::unique_ptr<PlaylistExporter>      PlaylistExporterPtr;

class SongImpl : public Song {
public:
  SongImpl(const SongImpl& other)
    : mLength(other.mLength), mPath(other.mPath), mArtist(other.mArtist), mTitle(other.mTitle) { }
  SongImpl(const SongImpl&& other)
    : mLength(other.mLength), mPath(std::move(other.mPath)), mArtist(std::move(other.mArtist)), mTitle(std::move(other.mTitle)) { }
  /*SongImpl(size_t length, const std::string&& path, const std::string&& artist, const std::string&& title )
    : mLength(length), mPath(std::move(path)), mArtist(std::move(artist)), mTitle(std::move(title)) { }*/
  SongImpl(size_t length, const std::string& path, const std::string& artist, const std::string& title )
    : mLength(length), mPath(path), mArtist(artist), mTitle(title) { }
  SongImpl(size_t length, const char* path, const char* artist = nullptr, const char* title = nullptr )
    : mLength(length), mPath(path), mArtist(artist), mTitle(title) { }

  size_t      length() const { return mLength; }
  const char* path()   const { return mPath.c_str(); }
  const char* artist() const { return mArtist.c_str(); }
  const char* title()  const { return mTitle.c_str(); }

protected:
  size_t mLength;
  std::string mPath, mArtist, mTitle;
};

class PlaylistImpl : public Playlist {
public:
  PlaylistImpl() { }
  ~PlaylistImpl() { }

  void release() {
    delete this;
  }

  void        addSong(SongPtr&& song)  { if (song) mSongs.push_back( std::move(song) ); }
  void        addSong(Song* song)      { addSong( SongPtr(song) ); }
  size_t      songCount() const        { return mSongs.size(); }
  const Song& song(size_t index) const { assert(index < songCount()); return *mSongs[index]; }

private:
  DISALLOW_COPY_AND_ASSIGN(PlaylistImpl);
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

private:
  void loadDefaults();

  const PlaylistImporter* getImporter(const std::string& extension) const;
  const PlaylistExporter* getExporter(const std::string& extension) const;

  typedef std::unordered_map<std::string, PlaylistExporterPtr> ExporterMap;
  typedef std::unordered_map<std::string, PlaylistImporterPtr> ImporterMap;

  ExporterMap mExporters;
  ImporterMap mImporters;
};

template< class Exporter >
class PlaylistExporterImpl : public PlaylistExporter {
public:
  PlaylistExporterImpl( const Exporter& exporter = Exporter() ) 
    : mExporter( exporter ) { }

  bool operator()(const Playlist& playlist, const char* fileName) const {
    bool success = false;
    std::ofstream ofs(fileName);
    const Exporter& exporter = mExporter;
    if ( ofs ) {
      success = true;

      // Write playlist header info
      ofs << exporter( playlist );

      // Write playlist song info
      //  Umm.... lambdas are nice
      playlist.applyOp( [&]( const SongPtr& song ) {
        ofs << exporter( *song );
      });

      // Write playlist end
      ofs << exporter( );
    }

    return success;
  }

private:
  Exporter mExporter;
};

template< class Importer >
class PlaylistImporterImpl : public PlaylistImporter {
public:
  PlaylistPtr operator()(const char* fileName) const {
    PlaylistImplPtr playlist( new PlaylistImpl() );
    std::ifstream ifs( fileName );
    if ( ifs ) {
      Importer it( ifs );
      while ( it.hasNext( ) ) {
        playlist->addSong( it.next( ) );
      };
    }
    return PlaylistPtr(playlist.release());
  }
};

}

#endif
