/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#include "PlaylistOps.h"
#include "PlaylistFileUtils.h"

using namespace pu;

///////////////////////////////////////////////////////////////////////////

CopySongOp::CopySongOp( const char* destDir, const FileTraits& traits, const OpListener& listener )
    : mDestDir(destDir), mTraits( traits ), mListener( listener ) { }

bool CopySongOp::operator()( const Song& song ) const {
  File file( song.path(), mTraits );
  mListener.beginOp( opName(file).c_str() );
  bool success = file.copy( mDestDir );
  mListener.endOp( success );
  return success;
}

std::string CopySongOp::opName( const File& file ) const {
  std::string opName("Copying ");
  opName.append( file.getPath() );
  opName.append( " to " );
  opName.append( mDestDir );
  return opName;
}

///////////////////////////////////////////////////////////////////////////

MoveSongOp::MoveSongOp( const char* destDir, const FileTraits& traits, const OpListener& listener )
  : mDestDir(destDir), mTraits( traits ), mListener( listener ) { }

bool MoveSongOp::operator()( Song& song ) {
  File file( song.path(), mTraits );
  mListener.beginOp( opName(file).c_str() );
  bool success = file.move( mDestDir );
  mListener.endOp( success );
  return success;
}

std::string MoveSongOp::opName( const File& file ) const {
  std::string opName("Moving ");
  opName.append( file.getPath() );
  opName.append( " to " );
  opName.append( mDestDir );
  return opName;
}

///////////////////////////////////////////////////////////////////////////

DeleteSongOp::DeleteSongOp( const FileTraits& traits, const OpListener& listener )
    : mTraits( traits ), mListener( listener ) { }

bool DeleteSongOp::operator()( Song& song ) {
  File file( song.path(), mTraits );
  mListener.beginOp( opName(file).c_str() );
  bool success = file.remove();
  mListener.endOp( success );
  return success;
}

std::string DeleteSongOp::opName( const File& file ) const {
  std::string opName("Deleting ");
  opName.append( file.getPath() );
  return opName;
}

///////////////////////////////////////////////////////////////////////////

SortSongsOp::SortSongsOp( const SongComparator& compare /*= SongComparator() */,  const OpListener& listener /*= OpListener()*/ )
  :  mCompare( compare ), mListener( listener ) {

}

bool SortSongsOp::operator()( Song* first, size_t count ) const {
  mListener.beginOp( "Sorting..." );
  std::sort(first, first + count, mCompare);
  mListener.endOp( true );
  return true;
}
