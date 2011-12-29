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

///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {

  
  pu::PlaylistManager& manager = pu::playlistManager();
  pu::PlaylistPtr playlist;

  if ( argc <= 1 ) {
     playlist = manager.importFromFile("test.m3u");
  } else {
    playlist = manager.importFromFile(argv[1]);
  }
  
  if ( playlist && playlist->songCount() > 0 ) {

    ///////////////////////////////////////////////////////////////////////////

    pu::SongComparator comparator;
    pu::StdFileTraits traits;
    CommandLineListener listener;

    /*
    ///////////////////////////////////////////////////////////////////////////

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
    */

    // Lambdas
    playlist->applyOp( [](const pu::SongPtr& song) {
      std::cout << "Hello " << song->path() << std::endl;
    });

    playlist->apply( [&comparator](pu::Song* start, size_t count) { 
      std::sort(start, start + count, comparator); 
    });
  }

  return 0;
}
