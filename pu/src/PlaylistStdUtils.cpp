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

bool StdFileHandler::copy(const char* sourcePath, const char* destPath) const {
  std::ifstream ifs(sourcePath, std::ios::in  | std::ios::binary | std::ios::_Nocreate);
  if (ifs.is_open()) {
    std::ofstream ofs(destPath, std::ios::out | std::ios::binary);
    if (ofs.is_open()) {
      ofs << ifs.rdbuf();
      return !ofs.bad() /* && ifs.eof() */ ;
    }
  }
  return false;
}

bool StdFileHandler::rename(const char* sourcePath, const char* destName) const {
  return ::rename(sourcePath, destName) == 0;
}

bool StdFileHandler::remove(const char* sourcePath) const {
  return ::remove(sourcePath) == 0;
}

bool StdFileHandler::move(const char* sourcePath, const char* destPath) const {
  return copy(sourcePath, destPath) && remove(sourcePath);
}

bool pu::StdFileHandler::exists(const char* sourcePath) const {
  std::ifstream ifs(sourcePath);
  return !(!ifs);
  //return nullptr != ifs;
}

size_t pu::StdFileHandler::size(const char* sourcePath) const {
  std::streampos fsize = 0;
  std::ifstream file(sourcePath, std::ios::binary);
  if (file.is_open()) {
    fsize = file.tellg();
    file.seekg( 0, std::ios::end );
    fsize = file.tellg() - fsize;
  }
  return fsize;
}
