#include <PlaylistUtilities.h>

int main(int argc, char** argv) {

  pu::Playlist playlist;

  pu::SortSongsOp sort;

  pu::PlaylistSongsOp<pu::SortSongsOp> playlistOp;//(sort);

  playlistOp( playlist );

  return 0;
}
