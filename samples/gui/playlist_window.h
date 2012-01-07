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

namespace tthread {
  class thread;
}

QT_BEGIN_NAMESPACE
class QButtonGroup;
class QComboBox;
class QLabel;
class QProgressBar;
class QPushButton;
class QStackedWidget;
class QTableView;
QT_END_NAMESPACE

class PlaylistModel;

class PlaylistWindow : public QWidget {
  Q_OBJECT

public:

  PlaylistWindow();
  ~PlaylistWindow();

  enum OpState {
    OpState_Invalid = 0,
    OpState_Valid,
    OpState_Executing,
    OpState_Complete,
    OpStates
  };

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

  virtual void dragEnterEvent(QDragEnterEvent*);
  virtual void dropEvent(QDropEvent*);
  const pu::Song* selectedSong() const;

signals:
  void pushMsg(const char*);
  void stateChanged();

private slots:
  void refreshOpState();
  void refreshState();
  void setOpState(OpState);
  void executeSongOp();

  void customContextMenu(const QPoint&);

private:
  void setPlaylist(QString playlistPath);
  void setDestination(QString destinationPath);

  PlaylistOp currentOp() const;
  
  QString fileText() const;
  QString opText() const;

  QWidget* createSettingsWidget(QWidget*);

  QTableView*     mPlaylistView;
  PlaylistModel*  mPlaylistModel;

  QComboBox*      mPlaylistOperatorComboBox;
  QComboBox*      mSongOperatorComboBox;
  QStackedWidget* mPlaylistViews;
  QButtonGroup*   mViewButtonGroup;
  QLabel*         mFileLabel;
  QString         mFileText;
  QProgressBar*   mFileProgress;
  QLabel*         mOpLabel;
  QProgressBar*   mOpProgress;
  QString         mDestinationPath, mPlaylistPath;
  QPushButton*    mExecuteButton;

  OpState mState;
  
  std::unique_ptr<tthread::thread> mOpThread;
  std::unique_ptr<pu::OpListener>            mOpListener;
  std::unique_ptr<pu::Playlist,pu::Releaser> mPlaylist;
};

#endif
