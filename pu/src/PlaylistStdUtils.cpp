/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#include "PlaylistStdUtils.h"

#include <stdio.h>
#include <fstream>

#if defined(PU_WINDOWS)
#include <Windows.h>
#else
#include <fcntl.h>
#include <stdlib.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif


using namespace pu;

///////////////////////////////////////////////////////////////////////////

#if 0
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
#elif defined(PU_WINDOWS)
bool StdFileHandler::copy(const char* sourcePath, const char* destPath) const {
  return CopyFile(sourcePath, destPath, false) != 0;
}
#else
bool StdFileHandler::copy(const char* sourcePath, const char* destPath) const {
  bool success = false;
  int read_fd, write_fd;
  struct stat stat_buf;
  off_t offset = 0;

  read_fd = open(sourcePath, O_RDONLY);
  if(read_fd != -1) {
    if (fstat(read_fd, &stat_buf) == 0) {
      write_fd = open(destPath, O_WRONLY | O_CREAT, stat_buf.st_mode);
      if (write_fd != -1) {
        success = sendfile(write_fd, read_fd, &offset, stat_buf.st_size) != -1;
        close(write_fd);
      }
    }
    close(read_fd);
  }
  return success;
}
#endif

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
