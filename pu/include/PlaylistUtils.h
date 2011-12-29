/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef PLAYLIST_UTILS_H
#define PLAYLIST_UTILS_H

#include "PlaylistUtilities.h"

#include "Playlist.h"
#include "PlaylistFileUtils.h"

#include <fstream>
#include <sstream>
#include <deque>
#include <algorithm>
#include <functional> 
#include <locale> 

namespace pu {

struct PlaylistImportIterator {
  PlaylistImportIterator( std::ifstream& ifs ) {
    if ( ifs.is_open() ) {
      std::string line;
      while ( ifs.good() ) {
        std::getline(ifs, line);
        if ( line.empty() ) {
          continue;
        } else {
          handle(line);
        }
      }
      ifs.close();
    }
  }

  virtual void handle(const std::string& line ) { }

  bool hasNext( ) const { return !mSongs.empty(); }

  SongPtr next( ) { 
    SongPtr val;
    if ( hasNext() ) {
      val.reset( mSongs.front().release() );
      mSongs.pop_front();
    }
    return val;
  }

  std::deque<SongPtr> mSongs;
};

static inline std::string&& trim(const std::string& str) {
  std::string temp(str);
  std::string::size_type pos = temp.find_last_not_of(' ');
  if (pos != std::string::npos) {
    temp.erase(pos + 1);
    pos = temp.find_first_not_of(' ');
    if(pos != std::string::npos) temp.erase(0, pos);
  }
  else temp.erase(temp.begin(), temp.end());
  return std::move(temp);
}

}

#endif
