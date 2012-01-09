/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef PLAYLIST_WINDOW_H
#define PLAYLIST_WINDOW_H

#include <QWidget>
#include <QTime>

#include <PlaylistForward.h>

#include <memory>

namespace pu {
class Releaser;
class Playlist;
}

namespace tthread {
  class thread;
  class mutex;
  class condition_variable;
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

class QOpListener;

class PlaylistWindow : public QWidget {
  Q_OBJECT

public:

  PlaylistWindow(const QString& playlistPath = QString(), const QString& destPath = QString());
  ~PlaylistWindow();

  enum OpState {
    OpState_Invalid = 0,
    OpState_Valid,
    OpState_Executing,
    OpState_Paused,
    OpState_Cancel,
    OpState_Complete,
    OpState_Shutdown,
    OpStates
  };

  enum PlaylistOp {
    PlaylistSongOp_Copy = 0,
    PlaylistSongOp_Move,
    PlaylistSongOp_Delete,
    PlaylistSongOp_Clear,
    PlaylistOp_New,
    PlaylistOp_Move,
    PlaylistOp_Delete,
    PlaylistOp_Copy,
    PlaylistOp_Merge,
    PlaylistOp_Sort,
    PlaylistOps,

    PlaylistSongOp_First = PlaylistSongOp_Copy,
    PlaylistSongOp_Last  = PlaylistSongOp_Clear,
    PlaylistSongOp_Count = PlaylistSongOp_First - PlaylistSongOp_Last,
    PlaylistOp_First     = PlaylistOp_New,
    PlaylistOp_Last      = PlaylistOp_Sort,
    PlaylistOp_Count     = PlaylistOp_First - PlaylistOp_Last,
  };

  virtual void dragEnterEvent(QDragEnterEvent*);
  virtual void dropEvent(QDropEvent*);
  const pu::Song* selectedSong() const;

signals:
  void stateChanged();
  void fileProgressChanged(int);
  void opProgressChanged(int);

private slots:
  void refreshOpState();
  void refreshState();
  void setOpState(OpState);
  void executeSongOp();
  void closeSongOp();

  void customContextMenu(const QPoint&);

private:
  void setPlaylist(QString playlistPath);
  void setDestination(QString destinationPath);

  int setFileProgress(int value);
  int setOpProgress(int value);

  OpState    currentState() const;
  PlaylistOp currentOp() const;
  bool       readOnly() const;
  
  QString fileText() const;
  QString opText() const;

  QWidget* createSettingsWidget(QWidget*);
  QOpListener* createOpListener();

  QTableView*     mPlaylistView;
  PlaylistModel*  mPlaylistModel;

  QComboBox*      mPlaylistOperatorComboBox;
  QComboBox*      mSongOperatorComboBox;
  QLabel*         mFileLabel;
  QString         mFileText;
  QProgressBar*   mFileProgress;
  QLabel*         mOpLabel;
  QProgressBar*   mOpProgress;
  QLabel*         mOpLabel2;
  QString         mDestinationPath, mPlaylistPath;
  QPushButton*    mExecuteButton;
  QPushButton*    mCloseButton;

  QTime mOpTime;
  QTime mFileTime; 

  OpState mState;

  std::unique_ptr<tthread::thread>             mOpThread;
  std::unique_ptr<tthread::mutex>              mOpMutex;
  std::unique_ptr<tthread::condition_variable> mOpCondition;
  std::unique_ptr<pu::OpListener>            mOpListener;
  std::unique_ptr<pu::FileHandler>           mFileHandler;
  std::unique_ptr<pu::Playlist,pu::Releaser> mPlaylist;
};

#endif
