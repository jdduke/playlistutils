#ifndef PLAYLIST_STD_UTILS_H
#define PLAYLIST_STD_UTILS_H

#include "PlaylistCommon.h"
#include "PlaylistFileUtils.h"

namespace pu {

class PU_API StdFileTraits : public FileTraits {
public:
  virtual bool copy(const char* sourcePath, const char* destPath) const;
  virtual bool rename(const char* sourcePath, const char* destName) const;
  virtual bool remove(const char* sourcePath) const;
  virtual bool move(const char* sourcePath, const char* destPath) const;
};

}

#endif
