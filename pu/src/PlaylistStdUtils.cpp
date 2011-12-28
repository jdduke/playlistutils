/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#include "PlaylistStdUtils.h"

#include <stdio.h>
#include <fstream>

using namespace pu;

///////////////////////////////////////////////////////////////////////////

bool StdFileTraits::copy(const char* sourcePath, const char* destPath) const {
  std::ifstream ifs(sourcePath, std::ios::binary);
  std::ofstream ofs(destPath,   std::ios::binary);
  if (ifs.is_open()  && ofs.is_open()) {
    ofs << ifs.rdbuf();
    return true;
  }
  return false;
}

bool StdFileTraits::rename(const char* sourcePath, const char* destName) const {
  return ::rename(sourcePath, destName) == 0;
}

bool StdFileTraits::remove(const char* sourcePath) const {
  return ::remove(sourcePath) == 0;
}

bool StdFileTraits::move(const char* sourcePath, const char* destPath) const {
  return copy(sourcePath, destPath) && remove(sourcePath);
}
