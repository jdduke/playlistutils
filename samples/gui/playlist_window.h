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
    PlaylistOp_Copy = 0,
    PlaylistOp_Move,
    PlaylistOp_Delete,
    PlaylistOp_Clear,
    PlaylistOp_Sort,
    PlaylistOps,

    PlaylistOp_Default = PlaylistOp_Copy,
    PlaylistOp_First   = 0,
    PlaylistOp_Last    = PlaylistOps - 1,
  };

  enum PlaylistSort {
    PlaylistSort_Path = 0,
    PlaylistSort_Title,
    PlaylistSort_Artist,
    PlaylistSort_Size,
    PlaylistSorts,

    PlaylistSort_Default = PlaylistSort_Path,
    PlaylistSort_First   = 0,
    PlaylistSort_Last    = PlaylistSorts - 1
  };

  enum FileBehavior {
    FileBehavior_Replace = 0,
    FileBehavior_Older,
    FileBehavior_Skip,
    FileBehavior_Query,
    FileBehaviors,

    FileBehavior_Default = FileBehavior_Skip,
    FileBehavior_First   = 0,
    FileBehavior_Last    = FileBehaviors - 1
  };

  virtual void dragEnterEvent(QDragEnterEvent*);
  virtual void dropEvent(QDropEvent*);

  /////////////////////////////////////////////////////////////////////////////

  void setPlaylist(QString playlistPath);
  void setDestination(QString destinationPath);
  int  setOpProgress(int value);
  void refreshPlaylist();

  /////////////////////////////////////////////////////////////////////////////

  const pu::Playlist* playlist() const { return mPlaylist.get(); }
        pu::Playlist* playlist()       { return mPlaylist.get(); }

  const pu::Song* selectedSong() const;
  PlaylistOp   selectedOp() const;
  PlaylistSort selectedSort() const;
  FileBehavior selectedBehavior() const;
  const char*  selectedDestination() const { return mDestinationPath.toLatin1(); }

signals:
  void cancelled();
  void stateChanged();
  void fileProgressChanged(int);
  void opProgressChanged(int);
  void titleChanged(const QString&);

private slots:
  void refreshOpState();
  void refreshState();
  void setOpState(OpState);
  void executeSongOp();
  void closeSongOp();
  void openPlaylistOp();
  void openDestOp();
  void cancelOps();
  void clearOps();
  bool beginOp(const char*, const pu::Song&);
  bool beginOp(const char*);
  bool endOp(bool);

  void customContextMenu(const QPoint&);

private:

  int setFileProgress(int value);

  size_t songCount() const;

  OpState    currentState() const;
  PlaylistOp currentOp() const;
  bool       readOnly() const;

  QString fileText() const;
  QString opText() const;

  QWidget* createSettingsWidget(QWidget*);
  QOpListener* createOpListener();

  QTableView*     mPlaylistView;
  PlaylistModel*  mPlaylistModel;

  QComboBox*      mSongOperatorComboBox;
  QComboBox*      mFileBehaviorComboBox;
  QComboBox*      mSortComboBox;
  QPushButton*    mFileButton;
  QString         mFileText;
  QProgressBar*   mFileProgress;
  QPushButton*    mOpButton;
  QLabel*         mOpLabel;
  QProgressBar*   mOpProgress;
  QString         mDestinationPath;
  QString         mPlaylistPath;
  QPushButton*    mExecuteButton;
  QPushButton*    mCloseButton;

  QTime mOpTime;
  QTime mFileTime;

  OpState mState;
  FileBehavior mFileBehavior;

  std::unique_ptr<tthread::thread>             mOpThread;
  std::unique_ptr<tthread::mutex>              mOpMutex;
  std::unique_ptr<tthread::condition_variable> mOpCondition;
  std::unique_ptr<pu::OpListener>              mOpListener;
  std::unique_ptr<pu::FileHandler>             mFileHandler;
  std::unique_ptr<pu::Playlist,pu::Releaser>   mPlaylist;
};

#endif
