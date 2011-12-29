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


struct PlaylistM3UImport : public PlaylistImportIterator {
  PlaylistM3UImport( std::ifstream& ifs ) : PlaylistImportIterator(ifs) { }

  void handle(const std::string& line) {
    if (!line.empty()) {
      if ( hasEnding(line, "mp3") ) {
        if (line.find_first_of(":\\")  != std::string::npos &&
            line.find_first_of("\\\\") != std::string::npos) {
          mSongs.push_back( SongPtr(new SongImpl(0, trim(line), std::string(), std::string())) );
        }
      }
    }
  }
};

}

#endif
