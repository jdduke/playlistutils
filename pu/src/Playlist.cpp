/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#include "Playlist.h"

#include "PlaylistFileUtils.h"

#include "PlaylistM3U.h"
#include "PlaylistPLS.h"
#include "PlaylistWPL.h"
#include "PlaylistZPL.h"

namespace pu {

PlaylistManager& playlistManager() {
  static PlaylistManagerImpl playlistManager;
  return playlistManager;
}

}

using namespace pu;

static inline std::string extensionOf( const char* fileName ) {
  return File::fileExtension( fileName );
}

void PlaylistManagerImpl::loadDefaults() {
  registerImporter(new PlaylistImporterImpl<PlaylistM3UImport>(), "m3u");
  registerExporter(new PlaylistExporterImpl<PlaylistM3UExport>(), "m3u");
  // TODO: Implement pls/wmpl/zpl playlist importer/exporters  
}

const PlaylistImporter* PlaylistManagerImpl::getImporter( const std::string& extension ) const {
  auto it = mImporters.find( extension );
  return (it != mImporters.end()) ? it->second.get() : nullptr;
}

const PlaylistExporter* PlaylistManagerImpl::getExporter( const std::string& extension ) const {
  auto it = mExporters.find( extension );
  return (it != mExporters.end()) ? it->second.get() : nullptr;
}

pu::PlaylistPtr PlaylistManagerImpl::importFromFile( const char* fileName ) const {
  PlaylistPtr playlist;
  auto importer( getImporter( extensionOf(fileName) ) );
  if ( importer ) {
    playlist = (*importer)( fileName ) ;
  }
  return playlist;
}

bool PlaylistManagerImpl::exportToFile( const Playlist& playlist, const char* fileName ) const {
  bool success = false;
  auto exporter( getExporter( extensionOf(fileName) ) );
  if ( exporter ) {
    success = (*exporter)( playlist, fileName );
  }
  return success;
}

bool PlaylistManagerImpl::registerImporter( PlaylistImporter* importer, const char* extension ) {
  assert(nullptr != importer && "Invalid importer.");
  bool importerExists = supportsImport( extension );
  if ( importer ) {
    mImporters.insert( ImporterMap::value_type(extension, PlaylistImporterPtr(importer)) );
  }
  return importerExists;
}

bool PlaylistManagerImpl::supportsImport( const char* extension ) const {
  return nullptr != getImporter( extension );
}

bool PlaylistManagerImpl::registerExporter( PlaylistExporter* exporter, const char* extension ) {
  assert(nullptr != exporter && "Invalid exporter.");
  bool exporterExists = supportsExport( extension );
  if (exporter) {
    //assert(isValid(extension) && "Invalid exporter.");
    mExporters.insert( ExporterMap::value_type(extension, PlaylistExporterPtr(exporter)) );
  }
  return exporterExists;
}

bool PlaylistManagerImpl::supportsExport( const char* extension ) const {
  return nullptr != getExporter( extension );
}
