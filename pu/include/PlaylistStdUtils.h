/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef PLAYLIST_STD_UTILS_H
#define PLAYLIST_STD_UTILS_H

#include "PlaylistCommon.h"
#include "PlaylistFileUtils.h"

namespace pu {

class StdFileHandler : public FileHandler {
public:
  StdFileHandler() { }
  bool copy(  const char* sourcePath, const char* destPath) const;
  bool rename(const char* sourcePath, const char* destName) const;
  bool move(  const char* sourcePath, const char* destPath) const;
  bool remove(const char* sourcePath) const;
  bool exists(const char* sourcePath) const;
};

// TODO: Implement
class StdXmlHandler : public XmlHandler {
public:
  StdXmlHandler() { }
  void load( std::ifstream& ifs ) { }
  void nextElement() { }
  void hasNextElement() const { }
};

//TODO: Implement
class StdLogHandler : public LogHandler {
public:
  StdLogHandler() { }
  virtual void operator()( const char* msg, LogLevel level ) const { }
};

}

#endif
