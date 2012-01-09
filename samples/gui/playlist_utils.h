/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef PLAYLIST_QT_UTILS_H
#define PLAYLIST_QT_UTILS_H

#include "PlaylistUtilities.h"

#include <QString>

class QtFileHandler : public pu::FileHandler {
public:
  virtual bool copy(  const char* sourcePath, const char* destPath) const;
  virtual bool rename(const char* sourcePath, const char* destName) const;
  virtual bool move(  const char* sourcePath, const char* destPath) const;
  virtual bool remove(const char* sourcePath) const;
  virtual bool exists(const char* sourcePath) const;
  virtual size_t size(const char* sourcePath) const;
};

inline QString toString(const pu::Song& song) {
  return QString(song.artist()) + " - " + song.title();
}

#endif
