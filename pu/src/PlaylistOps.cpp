/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#include "PlaylistOps.h"
#include "PlaylistFileUtils.h"

using namespace pu;

///////////////////////////////////////////////////////////////////////////

CopySongOp::CopySongOp( const char* destDir )
    : mDestDir(destDir) { }

bool CopySongOp::operator()( const Song& song ) const {
  File file( song.path(), handler() );
  auto success = false;
  if (listener().beginOp( "Copying", song )) {
    success = file.copy( mDestDir );
  }
  listener().endOp( success );
  return success;
}

///////////////////////////////////////////////////////////////////////////

MoveSongOp::MoveSongOp( const char* destDir )
  : mDestDir(destDir) { }

bool MoveSongOp::operator()( Song& song ) {
  File file( song.path(), handler() );
  auto success = false;
  if (listener().beginOp( "Moving", song )) {
    success = file.move( mDestDir );
  }
  listener().endOp( success );
  return success;
}

///////////////////////////////////////////////////////////////////////////

DeleteSongOp::DeleteSongOp( ) { }

bool DeleteSongOp::operator()( Song& song ) {
  File file( song.path(), handler() );
  auto success = false;
  if (listener().beginOp( "Deleting", song )) {
    success = file.remove();
  }
  listener().endOp( success );
  return success;
}

///////////////////////////////////////////////////////////////////////////

SortSongsOp::SortSongsOp( const SongComparator& compare /*= SongComparator() */ )
  :  mCompare( compare ) { }

bool SortSongsOp::operator()( Song* first, Song* last ) const {
  auto success = false;
  if (listener().beginOp( "Sorting..." )) {
    std::sort(first, last, mCompare);
    success = true;
  }
  listener().endOp( true );
  return success;
}
