/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef PLAYLIST_WINDOW_H
#define PLAYLIST_WINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QComboBox;
QT_END_NAMESPACE

class PlaylistWindow : public QWidget {
  Q_OBJECT
public:
  PlaylistWindow();

  enum PlaylistOp {
    PlaylistSongOp_Move = 0,
    PlaylistSongOp_Delete,
    PlaylistSongOp_Copy,
    PlaylistOp_New,
    PlaylistOp_Move,
    PlaylistOp_Delete,
    PlaylistOp_Copy,
    PlaylistOp_Merge,
    PlaylistOp_Sort,
  };

signals:
  void testLambda();

private slots:
  void executeOp();

private:
  void       refreshOp();
  void       refreshOps();
  PlaylistOp currentOp() const;

  QComboBox* mOperatorComboBox;
};

#endif
