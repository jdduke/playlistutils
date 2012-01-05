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
  void beginOp(const char* opName, const pu::Song& song) const {
    std::cout << opName << " : " << song.path();
  }
  void endOp( bool success ) {
    std::cout << (success ? " succeeded." : "failed.") << std::endl;
  }
};

///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {

  auto& module = pu::playlistModule();
  pu::PlaylistPtr playlist;

  if ( argc <= 1 ) {
     playlist = module.importFromFile("test.m3u");
  } else {
    playlist = module.importFromFile(argv[1]);
  }
  
  if ( playlist && playlist->songCount() > 0 ) {

    ///////////////////////////////////////////////////////////////////////////

    pu::SongComparator comparator;
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
    playlist->applyOp( [](const pu::Song& song) {
      std::cout << "Hello " << song.path() << std::endl;
    });

    playlist->apply( [](pu::Song* first, pu::Song* last) { 
      std::sort(first, last, [](const pu::Song& s1, const pu::Song& s2) {
        return std::strcmp( s1.path(), s2.path() ) < 0;
      }); 
    });

    playlist->applyOp( [](const pu::Song& song) {
      std::cout << "Goodbye " << song.path() << std::endl;
    });
  }

  return 0;
}
