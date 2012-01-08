/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#include "Playlist.h"

#include "PlaylistFileUtils.h"
#include "PlaylistOp.h"
#include "PlaylistStdUtils.h"

#include "PlaylistID3.h"
#include "PlaylistM3U.h"
#include "PlaylistPLS.h"
#include "PlaylistWPL.h"
#include "PlaylistZPL.h"

namespace pu {

PlaylistModule& playlistModule() {
  static PlaylistModuleImpl playlistModule;
  return playlistModule;
}

}

using namespace pu;

static void fileData(const std::string& path, size_t& length, size_t& size, std::string& artist, std::string& title) {
  // TODO: Parse mp3 tag
  File file(path.c_str());
  size = file.getSize();

  ID31 id31;
  if (loadID31(path.c_str(), id31)) {
    artist.assign(id31.artist, 30);
    trimInPlace(artist);
    title.assign(id31.title, 30);
    trimInPlace(title);
  }
}

Song::Song(const char* path) : mLength(0), mSize(0), mPath(path) {
  fileData(mPath, mLength, mSize, mArtist, mTitle);
}

///////////////////////////////////////////////////////////////////////////

PlaylistModuleImpl::PlaylistModuleImpl()
  : mFileHandler(nullptr), mXmlHandler(nullptr), mLogHandler(nullptr) {
  loadDefaults(); 
}

void PlaylistModuleImpl::loadDefaults() {
  registerImporter(new PlaylistImporterImpl<PlaylistM3UImport>(), "m3u");
  registerExporter(new PlaylistExporterImpl<PlaylistM3UExport>(), "m3u");
  // TODO: Implement pls/wmpl/zpl playlist importer/exporters  
}

const PlaylistImporter* PlaylistModuleImpl::getImporter( const std::string& extension ) const {
  auto it = mImporters.find( extension );
  return (it != mImporters.end()) ? it->second.get() : nullptr;
}

const PlaylistExporter* PlaylistModuleImpl::getExporter( const std::string& extension ) const {
  auto it = mExporters.find( extension );
  return (it != mExporters.end()) ? it->second.get() : nullptr;
}

pu::PlaylistPtr PlaylistModuleImpl::create() const
{
  return PlaylistPtr(new PlaylistImpl());
}

pu::PlaylistPtr PlaylistModuleImpl::createFromFile( const char* fileName ) const {
  PlaylistPtr playlist;
  auto importer( getImporter( extensionOf(fileName) ) );
  if ( importer ) {
    playlist = (*importer)( fileName ) ;
  }
  return playlist;
}

bool PlaylistModuleImpl::saveToFile( const Playlist& playlist, const char* fileName ) const {
  bool success = false;
  auto exporter( getExporter( extensionOf(fileName) ) );
  if ( exporter ) {
    success = (*exporter)( playlist, fileName );
  }
  return success;
}

bool PlaylistModuleImpl::registerImporter( PlaylistImporter* importer, const char* extension ) {
  assert(nullptr != importer && "Invalid importer.");
  auto importerExists = supportsImport( extension );
  if ( importer ) {
    mImporters.insert( ImporterMap::value_type(extension, PlaylistImporterPtr(importer)) );
  }
  return importerExists;
}

bool PlaylistModuleImpl::supportsImport( const char* extension ) const {
  return nullptr != getImporter( extension );
}

bool PlaylistModuleImpl::registerExporter( PlaylistExporter* exporter, const char* extension ) {
  assert(nullptr != exporter && "Invalid exporter.");
  auto exporterExists = supportsExport( extension );
  if (exporter) {
    //assert(isValid(extension) && "Invalid exporter.");
    mExporters.insert( ExporterMap::value_type(extension, PlaylistExporterPtr(exporter)) );
  }
  return exporterExists;
}

bool PlaylistModuleImpl::supportsExport( const char* extension ) const {
  return nullptr != getExporter( extension );
}

void PlaylistModuleImpl::setFileHandler( FileHandler* handler ) {
  mFileHandler = handler;
}

const FileHandler& PlaylistModuleImpl::fileHandler() const {
  static StdFileHandler defaultHandler;
  return mFileHandler ? *mFileHandler : defaultHandler;
}

void PlaylistModuleImpl::setXmlHandler( XmlHandler* handler ) {
  mXmlHandler = handler;
}

const XmlHandler& PlaylistModuleImpl::xmlHandler() const {
  static StdXmlHandler defaultHandler;
  return mXmlHandler ? *mXmlHandler : defaultHandler;
}

void PlaylistModuleImpl::setLogHandler( LogHandler* handler ) {
  mLogHandler = handler;
}

const LogHandler& PlaylistModuleImpl::logHandler() const {
  static StdLogHandler defaultHandler;
  return mLogHandler ? *mLogHandler : defaultHandler;
}

void PlaylistModuleImpl::setOpListener( OpListener* listener )
{
  mOpListener = listener;
}

const OpListener& PlaylistModuleImpl::opListener() const
{
  static OpListener defaultListener;
  return mOpListener ? *mOpListener : defaultListener;
}


