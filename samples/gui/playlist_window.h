/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef PLAYLIST_WINDOW_H
#define PLAYLIST_WINDOW_H

#include <QWidget>
#include <PlaylistForward.h>
#include <memory>

namespace pu {
class Releaser;
class Playlist;
}

QT_BEGIN_NAMESPACE
class QComboBox;
class QStackedWidget;
class QButtonGroup;
class QTextEdit;
class QLineEdit;
class QLabel;
class QProgressBar;
QT_END_NAMESPACE

class PlaylistWindow : public QWidget {
  Q_OBJECT
public:
  PlaylistWindow();
  ~PlaylistWindow();

  enum PlaylistOp {
    PlaylistOp_None = 0,
    PlaylistSongOp_Move,
    PlaylistSongOp_Delete,
    PlaylistSongOp_Copy,
    PlaylistOp_New,
    PlaylistOp_Move,
    PlaylistOp_Delete,
    PlaylistOp_Copy,
    PlaylistOp_Merge,
    PlaylistOp_Sort,
    PlaylistOps,

    PlaylistSongOp_First = PlaylistSongOp_Move,
    PlaylistSongOp_Last  = PlaylistSongOp_Copy,
    PlaylistSongOp_Count = PlaylistSongOp_First - PlaylistSongOp_Last,
    PlaylistOp_First     = PlaylistOp_New,
    PlaylistOp_Last      = PlaylistOp_Sort,
    PlaylistOp_Count     = PlaylistOp_First - PlaylistOp_Last,

  };

signals:
  void pushMsg(const char*);

private slots:
  void refreshPlaylistOp(int);
  void refreshSongOp(int);
  void executePlaylistOp();
  void executeSongOp();

private:
  PlaylistOp currentOp() const;

  QWidget* createSettingsWidget(QWidget*);

  QComboBox*      mPlaylistOperatorComboBox;
  QComboBox*      mSongOperatorComboBox;
  QStackedWidget* mPlaylistViews;
  QButtonGroup*   mViewButtonGroup;
  QTextEdit*      mSongPlaylistText;
  QLabel*         mFileLabel;
  QProgressBar*   mFileProgress;
  QLabel*         mOpLabel;
  QProgressBar*   mOpProgress;
  QLabel*         mPlaylistFileLabel;
  QProgressBar*   mPlaylistFileProgress;
  QLabel*         mPlaylistOpLabel;
  QProgressBar*   mPlaylistOpProgress;

  std::unique_ptr<pu::Playlist,pu::Releaser> mPlaylist;
};

#endif
