/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#include "PlaylistFileUtils.h"

/*#ifndef _T
#define _T(x) (x)
#endif*/

#if defined(PU_WINDOWS)
#include <direct.h>
#include <io.h>
#define ChDir  _chdir
#define Open   _open
#define GetCwd _getcwd
const char DIR_SEP = '\\';
#else 
const char DIR_SEP = '/';
#endif

namespace pu {

bool beginsWith( const std::string& path, const std::string& beginning ) {
  if (path.length() >= beginning.length()) {
    return (0 == path.compare(0, beginning.length(), beginning));
  } else {
    return false;
  }
}

bool endsWith( const std::string& path, const std::string& ending ) {
  if (path.length() >= ending.length()) {
    return (0 == path.compare(path.length() - ending.length(), ending.length(), ending));
  } else {
    return false;
  }
}

bool endsWith( const std::string& path, const char* ending ) {
  return endsWith(path, std::string(ending));
}

bool beginsWith( const std::string& path, const char* beginning ) {
  return beginsWith(path, std::string(beginning));
}

inline bool isSlash(char c)
{
	return (c == '\\') || (c == '/');
}


bool isAbsPath(const std::string& path) {
  if (path.empty())
    return false;
  if (isSlash(path[0]))
    return true;
  size_t i = path.find(':');
  if (i == std::string::npos)
    return false;
  size_t j = path.find_first_of("/\\");
  if (j == std::string::npos || j > i)
    return true;
  return false;
}

std::string absPath( const std::string &base, const std::string &rel ) {
  std::string path;
  if (isAbsPath(rel)) {
    return rel;
  }
  else {
    path = base;
    path += DIR_SEP;
    path += rel;

    std::string::reverse_iterator first = path.rbegin();
    std::string rev;
    size_t ignore = 0;

    while (first != path.rend()) {
      while (first != path.rend() && isSlash(*first)) ++first;
      std::string::reverse_iterator last = first;
      while (last != path.rend() && !isSlash(*last)) ++last;

      if (first != last) {
        std::string component(first, last);
        if (component == "..") {
          ++ignore;
        } else if (component == ".") {
          // skip
        } else {
          if (ignore) {
            --ignore;
          } else {
            rev += component;
            if (last != path.rend())
              rev += DIR_SEP;
          }
        }
      }

      first = last;
    }

    std::reverse(rev.begin(), rev.end());
    if (rev.empty()) rev = DIR_SEP;
    return rev;
  }
}


std::string extensionOf( const char* fileName ) {
  return File::fileExtension( fileName );
}

}
