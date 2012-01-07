/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef PLAYLIST_M3U_H
#define PLAYLIST_M3U_H

#include "PlaylistUtilities.h"
#include "PlaylistUtils.h"

namespace pu {

struct PlaylistM3UExport {

  PlaylistM3UExport() { }

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


class PlaylistM3UImport : public PlaylistImportIterator {
public:
  PlaylistM3UImport( std::ifstream& ifs, const char* fileName ) { load(ifs, fileName); }

  virtual void handle(const std::string& line, const std::string& filePath) {
    if (!line.empty()) {
      if ( beginsWith(line, "#") ) {

      } else if ( endsWith(line, "mp3") ) {
        mSongs.emplace_back(absPath(filePath, line).c_str());
      }
    }
  }
};

}

#endif
