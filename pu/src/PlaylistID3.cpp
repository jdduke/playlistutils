/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#include "PlaylistID3.h"

#include <stdio.h>
#include <string>
#include <fstream>

namespace pu {

bool loadID31Data(const char* header, ID31& id31) {
  std::string test(header, 3);
  if(test.compare(0, 3, "TAG") == 0) {
    id31 = *reinterpret_cast<const ID31*>(header);
    return true;
  } else {
    return false;
  }
}

#if 1

bool loadID31(const char* path, ID31& id31) {
  bool success = false;
  FILE* f = NULL;
  if (fopen_s(&f, path, "r") == 0) {
    if (fseek(f, (long)(0-sizeof(id31)), (int)SEEK_END) == 0) {
      std::vector<char> input(sizeof(ID31));
      if (fread(input.data(), 1, sizeof(ID31), f) == sizeof(ID31)) {
        success = loadID31Data(input.data(), id31);
      }
    }
    fclose(f);
  }
  return success;
}

#else

bool loadID31(const char* fileName, ID31& id31) {
  std::ifstream ifs(fileName);
  if (ifs.is_open()) {
    std::vector<char> buf(sizeof(ID31));
    ifs.seekg(sizeof(ID31), std::ios::end);
    ifs.read(buf.data(), sizeof(ID31));
    return loadID31Data(buf.data(), id31);
  } else {
    return false;
  }
}

#endif 

}
