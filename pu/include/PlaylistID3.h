/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef PLAYLIST_ID3_H
#define PLAYLIST_ID3_H

#include "PlaylistUtilities.h"

namespace pu {

struct ID31 { 
  char header[3];
  char title[30];
  char artist[30];
  char album[30];
  char year[4];
  char comment[28]; 
  char zero;
  unsigned char tracknum;
  unsigned char genre;
};

bool loadID31(const char*, ID31&);

}

#endif
