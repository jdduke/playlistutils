/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#include <PlaylistUtilities.h>
#include <PlaylistOps.h>
#include <PlaylistStdUtils.h>

#include <iostream>

// Simple console output
class CommandLineListener : public pu::OpListener {
public:
  CommandLineListener() { }
  void beginOp( const char* opName ) {
    std::cout << opName << " ... ";
  }
  void endOp( bool success ) {
    std::cout << (success ? " succeeded." : "failed.") << std::endl;
  }
};

class Releaser {
public:
  template< typename T>
  void operator()(T* ptr) {
    if( ptr ) { ptr->release(); }
  }
};

///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {

  std::unique_ptr<pu::Playlist, Releaser> playlist(pu::createPlaylist(""));
  pu::StdFileTraits traits;
  CommandLineListener listener;

  ///////////////////////////////////////////////////////////////////////////

  pu::SongComparator comparator;
  pu::SortSongsOp sortOp( comparator, listener );
  pu::PlaylistSongsOp<pu::SortSongsOp> sortPlaylist( sortOp );
  sortPlaylist( *playlist );

  ///////////////////////////////////////////////////////////////////////////

  pu::DeleteSongOp deleteOp( traits, listener );
  pu::PlaylistSongOp<pu::DeleteSongOp> deletePlaylist( deleteOp );
  deletePlaylist( *playlist );

  ///////////////////////////////////////////////////////////////////////////

  pu::MoveSongOp moveOp( "", traits, listener );
  pu::PlaylistSongOp<pu::MoveSongOp> movePlaylist( moveOp );
  movePlaylist( *playlist );

  ///////////////////////////////////////////////////////////////////////////

  // Functors
  playlist->applyOp( deleteOp );

  ///////////////////////////////////////////////////////////////////////////

  // Lambdas
  playlist->applyOp( [](const pu::Song& song) { std::cout << "Hello " << song.artist() << std::endl;} );

  playlist->apply( [&comparator](pu::Song* start, pu::Song* stop) { std::sort(start, stop, comparator); } );

  ///////////////////////////////////////////////////////////////////////////

  return 0;
}
