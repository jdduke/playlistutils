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

typedef std::unique_ptr<PlaylistImpl,Releaser> PlaylistImplPtr;
typedef std::unique_ptr<PlaylistImporter>      PlaylistImporterPtr;
typedef std::unique_ptr<PlaylistExporter>      PlaylistExporterPtr;

class PlaylistImpl : public Playlist {
//public:
  //PlaylistImpl() : Playlist() { }
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
      playlist.applyOp( [&]( const Song& song ) {
        ofs << exporter( song );
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
