/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef PLAYLIST_PLS_H
#define PLAYLIST_PLS_H

#include "PlaylistUtilities.h"

#include "Playlist.h"
#include "PlaylistFileUtils.h"
#include "PlaylistUtils.h"

#include <fstream>
#include <sstream>
#include <deque>

namespace pu {

struct PlaylistPLSExport {

  std::string operator()( const Playlist& playlist ) const {
    return "#EXTM3U\n";
  }

  std::string operator()( const Song& song ) const {
    std::stringstream ss;
    ss << "#EXTINFO" << song.length() << ", ";
    ss << song.artist() << " - " << song.title() << std::endl;
    ss << song.path();
    return ss.str();
  }

  std::string operator()( ) const {
    return "";
  }
};

class PlaylistPLSImport : public PlaylistImportIterator {
public:
  PlaylistPLSImport( std::ifstream& ifs, const char* fileName ) { load( ifs, fileName ); }

  void handle( const std::string& line, const std::string& fileDir ) {
    if (!line.empty()) {
      //TODO: Handle
    }
  }
};

}

#endif
