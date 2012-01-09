/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#include "playlist_window.h"
#include "playlist_widgets.h"
#include "playlist_utils.h"

#include "PlaylistUtilities.h"
#include "PlaylistOp.h"
#include "PlaylistOps.h"

#include "utils.h"

#include <tthread/tinythread.h>

#include <QtGui>

#include <QButtonGroup>
#include <QComboBox>
#include <QStackedWidget>

template<typename T>
static inline bool inRange(const T& val, const T& low, const T& high) {
  return (low <= val && val <= high);
}

class QOpListener : public pu::OpListener {
public:
  typedef std::function<bool(const char*,const pu::Song&)> A;
  typedef std::function<bool(const char*)>                 B;
  typedef std::function<bool(bool)>                        C;

  QOpListener(A a, B b, C c) : mA(a), mB(b), mC(c) { }

  bool beginOp(const char* opName, const pu::Song& song) const {
    return mA(opName, song);
  }

  bool beginOp(const char* opName) const {
    return mB(opName);
  }

  bool endOp(bool success) const {
    return mC(success);
  }

  A mA;
  B mB;
  C mC;
};

///////////////////////////////////////////////////////////////////////////
 
static QWidget* createOpsWidget(
  QWidget* parentCallback, QWidget* parent, QComboBox** comboBox,
  QPushButton** actionButton, QPushButton** closeButton,
  QLabel** fileLabel,                    QProgressBar** fileProgress,
  QLabel** opLabel,   QLabel** opLabel2, QProgressBar** opProgress, 
  const char* ops, const char* executeOp, const char* closeOp, const char* refreshOp) {

  auto* widget              = new QWidget(parent);
  auto* layout              = new QVBoxLayout(widget);
  auto* topWidget           = new QWidget(widget);
  auto* topLayout           = new QHBoxLayout(topWidget);
  auto* midWidget           = new QWidget(widget);
  auto* midLayout           = new QHBoxLayout(midWidget);
  auto* bottomWidget        = new QWidget(widget);
  auto* bottomLayout        = new QHBoxLayout(bottomWidget);
  *closeButton              = new QPushButton(QWidget::tr("Close"), midWidget);
  *actionButton             = new QPushButton(QWidget::tr("Execute"), midWidget);
  //auto* loadPlaylistButton  = new QPushButton(tr("Load playlist"), topWidget);
  *fileLabel                = new QLabel(QWidget::tr("Drop playlist here"), topWidget);
  *fileProgress             = new QProgressBar(widget);
  *opLabel                  = new QLabel(QWidget::tr("Select target folder"), bottomWidget);
  *opLabel2                 = new QLabel(QWidget::tr(""), bottomWidget);
  *opProgress               = new QProgressBar(widget);
  (*fileProgress)->setMinimum(0);
  (*opProgress)->setMinimum(0);
  (*comboBox)               = new QComboBox(midWidget);
  (*comboBox)->addItems( QString(ops).split(" ",QString::SkipEmptyParts) );

  topLayout->addWidget(*fileLabel);
  topLayout->addSpacing(30);
  topLayout->addStretch();

  midLayout->addSpacing(10);
  midLayout->addWidget(new QLabel("Action: ", widget));
  midLayout->addWidget(*comboBox);
  midLayout->addSpacing(20);
  midLayout->addWidget(*actionButton);
  midLayout->addSpacing(20);
  midLayout->addWidget(*closeButton);
  midLayout->addStretch();

  bottomLayout->addWidget(*opLabel);
  bottomLayout->addStretch();
  bottomLayout->addWidget(*opLabel2);

  layout->setSpacing(0);
  layout->addWidget(topWidget);
  layout->addWidget(*fileProgress);
  layout->addSpacing(20);
  layout->addWidget(bottomWidget);
  layout->addWidget(*opProgress);
  layout->addSpacing(20);
  layout->addWidget(midWidget);
  layout->addStretch();

  (*comboBox)->setCurrentIndex(0);

  QWidget::connect(*actionButton, SIGNAL(clicked()),            parentCallback, executeOp);
  QWidget::connect(*closeButton,  SIGNAL(clicked()),            parentCallback, closeOp);
  QWidget::connect(*comboBox, SIGNAL(currentIndexChanged(int)), parentCallback, refreshOp);

  return widget;
}

///////////////////////////////////////////////////////////////////////////

PlaylistWindow::PlaylistWindow(const QString& playlistPath, const QString& destPath)  
  : mState(OpStates), mOpMutex(new tthread::mutex), mOpCondition(new tthread::condition_variable) {  

  auto* layout    = new QVBoxLayout(this);
  
  // Top View
  auto* topWidget =
    createOpsWidget(this,this,&mSongOperatorComboBox,
                    &mExecuteButton,&mCloseButton,&mFileLabel,
                    &mFileProgress,&mOpLabel,&mOpLabel2,&mOpProgress,
                    "Copy Move Delete Clear",
                    SLOT(executeSongOp()), SLOT(closeSongOp()), SLOT(refreshState()));
  
  // Bottom View
  auto* bottomWidget = new QWidget(this);
  auto* bottomLayout = new QHBoxLayout(bottomWidget);

  mPlaylistView = new QTableView(bottomWidget);
  mPlaylistView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  mPlaylistView->setColumnWidth(PlaylistModel::Column_Size,   7);
  mPlaylistView->setColumnWidth(PlaylistModel::Column_Status, 10);
  mPlaylistModel = new PlaylistModel(bottomWidget);
  mPlaylistView->setModel(mPlaylistModel);
  mPlaylistView->setItemDelegateForColumn(0, new ImageDelegate(QString(" :/image/success :/image/failure").split(" "), bottomWidget));
  mPlaylistView->setColumnWidth(0, 15);
  mPlaylistView->setSelectionBehavior(QAbstractItemView::SelectRows);
  mPlaylistView->horizontalHeader()->setStretchLastSection(true);
  mPlaylistView->verticalHeader()->hide();
  mPlaylistView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  mPlaylistView->setSelectionMode(QAbstractItemView::SingleSelection);
  mPlaylistView->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(mPlaylistView, SIGNAL(customContextMenuRequested(const QPoint&)),
          this,          SLOT(customContextMenu(const QPoint&)));
  lconnect<int>(mPlaylistModel, SIGNAL(rowModified(QVariant)), [this](int row) {
    this->mPlaylistView->scrollTo(this->mPlaylistModel->index(row, 0));
  });

  bottomLayout->addWidget( mPlaylistView );

  // Full View
  layout->setSpacing(0);
  layout->addWidget(topWidget);
  layout->addWidget(bottomWidget);

  // Op Listener
  mOpListener.reset( createOpListener() );
  pu::playlistModule().setOpListener(mOpListener.get());

  // File Handler
  //mFileHandler.reset( new QtFileHandler() );
  //pu::playlistModule().setFileHandler(mFileHandler.get());

  // Events
  connect(this, SIGNAL(stateChanged()), 
          this, SLOT(refreshOpState()));

  connect(this,          SIGNAL(fileProgressChanged(int)),
          mFileProgress, SLOT(setValue(int)), Qt::BlockingQueuedConnection);

  connect(this,          SIGNAL(opProgressChanged(int)),
          mOpProgress,   SLOT(setValue(int)), Qt::BlockingQueuedConnection);

  setPlaylist(playlistPath);
  setDestination(destPath);

  setWindowTitle(tr("Playlist Utilities"));
  setAcceptDrops(true);
}

void PlaylistWindow::executeSongOp() {
  if (mState == OpState_Paused) {
    qDebug() << "Resuming Song Op: " << mSongOperatorComboBox->currentText();
    setOpState(OpState_Executing);
  } else if (mState != OpState_Executing && mState != OpState_Invalid) {
    auto executeOp = [this](void* data) {
      pu::Playlist* playlist = this->mPlaylist.get();
      if (playlist) {
        size_t op = (size_t)data;
        switch(op) {
        case PlaylistSongOp_Move:
          playlist->applyOp(pu::MoveSongOp(this->mDestinationPath.toLatin1()));
          break;
        case PlaylistSongOp_Delete:
          playlist->applyOp(pu::DeleteSongOp());
          break;
        case PlaylistSongOp_Copy:
          playlist->applyOp(pu::CopySongOp(this->mDestinationPath.toLatin1()));
          break;
        case PlaylistSongOp_Clear:
          this->setOpProgress( (int)mPlaylist->songCount() );
          setPlaylist("");
          break;
        default:
          break;
        }
      }
    };
    setOpState(OpState_Executing);
    qDebug() << "Executing Song Op: " << mSongOperatorComboBox->currentText();
    mOpThread.reset( new tthread::thread(executeOp, (void*)currentOp()) );
  } else {
    setOpState(OpState_Paused);
    qDebug() << "Pausing Song Op: " << mSongOperatorComboBox->currentText();
    // Handle pause
  }
  
}

void PlaylistWindow::closeSongOp() {
  if (readOnly()) {
    setOpState(OpState_Cancel);
    if (mOpThread->joinable()) {
      mOpThread->join();
    }
    setOpState(OpState_Valid);
  } else {
    QApplication::quit();
  }
}

PlaylistWindow::OpState PlaylistWindow::currentState() const {
  return mState;
}

PlaylistWindow::PlaylistOp PlaylistWindow::currentOp() const {
  return (PlaylistWindow::PlaylistOp)mSongOperatorComboBox->currentIndex();
}

bool PlaylistWindow::readOnly() const {
  return (mState == OpState_Executing ||
          mState == OpState_Paused    ||
          mState == OpState_Shutdown  ||
          mState == OpState_Cancel);
}

PlaylistWindow::~PlaylistWindow() {
  setOpState(OpState_Shutdown);
  if (mOpThread)
    mOpThread->join();
}

QWidget* PlaylistWindow::createSettingsWidget(QWidget* parent) {
  auto* widget = new QWidget(parent);
  auto* layout = new QHBoxLayout(widget);
  widget->setLayout(layout);
  return widget;
}

QOpListener* PlaylistWindow::createOpListener() {
  return new QOpListener(
    [this](const char* opName, const pu::Song& song) -> bool {
      {
        tthread::lock_guard<tthread::mutex> guard(*this->mOpMutex);
        while (this->currentState() == PlaylistWindow::OpState_Paused) {
          this->mOpCondition->wait(*this->mOpMutex);
        }
      }
      if (this->currentState() == OpState_Shutdown || this->currentState() == OpState_Cancel)
        return false;
      QString songString(toString(song));
      if (songString.isEmpty()) 
        songString = QString(song.path());
      if (!songString.isEmpty() && !songString.isNull())
        this->mFileLabel->setText( songString.mid(0, 60) );
      this->setFileProgress( 0 );
      int listIndex = this->mOpProgress->value();
      QModelIndex playlistModelIndex = this->mPlaylistModel->index( listIndex, PlaylistModel::Column_Status );
      this->mPlaylistModel->setData( playlistModelIndex, opName );
      return true;
  },
    [this](const char* opName) -> bool {
      {
        tthread::lock_guard<tthread::mutex> guard(*this->mOpMutex);
        if (this->currentState() == OpState_Shutdown || this->currentState() == OpState_Cancel)
          return false;
      }
      this->mFileLabel->setText( opName );
      this->mFileProgress->setValue( 0 );
      int listIndex = this->mOpProgress->value();
      QModelIndex playlistModelIndex = this->mPlaylistModel->index( listIndex, PlaylistModel::Column_Status );
      this->mPlaylistModel->setData( playlistModelIndex, opName );
      return true;
  },
    [this](bool success) -> bool {
      {
        tthread::lock_guard<tthread::mutex> guard(*this->mOpMutex);
        if (this->currentState() == OpState_Shutdown || this->currentState() == OpState_Cancel)
          return false;
      }
      int listIndex = this->mOpProgress->value();
      QModelIndex playlistModelIndexI = this->mPlaylistModel->index( listIndex, PlaylistModel::Column_Image );
      this->mPlaylistModel->setData( playlistModelIndexI, success ? PlaylistModel::Status_Success : PlaylistModel::Status_Failure );
      QModelIndex playlistModelIndexS = this->mPlaylistModel->index( listIndex, PlaylistModel::Column_Status );
      this->mPlaylistModel->setData( playlistModelIndexS, success ? "OK" : "Error" );
      this->setOpProgress( std::min(listIndex + 1, this->mOpProgress->maximum()) );
      double sizeInMB = (double)this->mPlaylist->song(listIndex).size() / (1024*1024);
      double timeInS =  (double)this->setFileProgress( success ? mFileProgress->maximum() : 0 ) / 1000;
      if (timeInS > 0.) {
        this->mOpLabel2->setText(QString::number(sizeInMB/timeInS, 'g', 4) + " MB/s");
      }
      return true;
  }
  );
}

void PlaylistWindow::refreshOpState() {
  mFileLabel->setText(fileText());
  mOpLabel->setText(opText());
  mOpLabel2->setText("");
  mSongOperatorComboBox->setDisabled(false);
  mExecuteButton->setEnabled(true);
  mExecuteButton->setText("Execute");
  mCloseButton->setEnabled(true);
  mCloseButton->setText("Close");
  switch (mState) {
  case OpState_Invalid:
    mExecuteButton->setEnabled(false);
  case OpState_Valid:
    mFileProgress->setValue(0);
    mOpProgress->setValue(0);
    break;
  case OpState_Executing:
    mExecuteButton->setText("Pause");
    mCloseButton->setText("Cancel");
    mSongOperatorComboBox->setDisabled(true);
    mFileProgress->setMaximum(100);
    mOpProgress->setMaximum((int)mPlaylist->songCount());
    break;
  case OpState_Cancel:
    mExecuteButton->setEnabled(false);
    mSongOperatorComboBox->setDisabled(true);
    mCloseButton->setText("Cancelling");
    mCloseButton->setEnabled(false);
    break;
  case OpState_Paused:
    mExecuteButton->setText("Resume");
    mCloseButton->setText("Cancel");
    mSongOperatorComboBox->setDisabled(true);
    break;
  case OpState_Complete:
    mExecuteButton->setEnabled(false);
    mFileProgress->setValue(mFileProgress->maximum());
    mOpProgress->setValue(mOpProgress->maximum());
    break;
  default:
    qDebug() << "Invalid op state.";
  };
}

QString PlaylistWindow::opText() const {
  switch(mSongOperatorComboBox->currentIndex()+PlaylistSongOp_First) {
  case PlaylistSongOp_Move:
  case PlaylistSongOp_Copy:
    return mDestinationPath.isEmpty() ? "Select Destination" : mDestinationPath;
  case PlaylistSongOp_Delete:
  case PlaylistSongOp_Clear:
  default:
    return "";
  };
  
}

QString PlaylistWindow::fileText() const {
  switch(mSongOperatorComboBox->currentIndex()+PlaylistSongOp_First) {
  case PlaylistSongOp_Move:
  case PlaylistSongOp_Copy:
  case PlaylistSongOp_Delete:
    return mState == OpState_Executing ? mFileText : 
      mPlaylistPath.isEmpty() ? "Load playlist" : mPlaylistPath;
  default:
    return "";
  };
}

void PlaylistWindow::setOpState(OpState opState) {
  tthread::lock_guard<tthread::mutex> guard(*mOpMutex);
  mOpCondition->notify_all();
  mState = opState;
  emit stateChanged();
}

void PlaylistWindow::dragEnterEvent(QDragEnterEvent* e) {
  if (mState != OpState_Executing && e->mimeData()->hasFormat("text/uri-list"))
    e->acceptProposedAction();
}

void PlaylistWindow::setPlaylist(QString playlistPath) {
  if (playlistPath.isEmpty() || playlistPath.isNull() || !QFileInfo(playlistPath).exists()) {
    mPlaylistPath = "";
    mPlaylist     = pu::playlistModule().create();
  } else {
    mPlaylistPath = playlistPath;
    mPlaylist     = pu::playlistModule().createFromFile(mPlaylistPath.toLatin1());
  }
  mPlaylistModel->setPlaylist( mPlaylist.get() );
  refreshState();
}

void PlaylistWindow::setDestination(QString destinationPath) {
  mDestinationPath = destinationPath;
  refreshState();
}

void PlaylistWindow::dropEvent(QDropEvent* e) {
  mOpMutex->lock();

  bool songsAdded = false;
  if (readOnly())
    return;
  if (e->mimeData()->hasUrls()) {
    foreach(QUrl url, e->mimeData()->urls()) {
      QFileInfo info( url.toLocalFile() );
      if (info.isFile() && info.exists() && pu::playlistModule().supportsImport(info.completeSuffix().toLatin1())) {
        mOpMutex->unlock();
        setPlaylist(info.absoluteFilePath());
        break;
      } else if (info.isDir() && info.exists()) {
        mOpMutex->unlock();
        setDestination(info.absoluteFilePath());
        break;
      } else if (info.isFile() && info.exists() && info.completeSuffix().endsWith("mp3", Qt::CaseInsensitive)) {
        if (!mPlaylist) {
          mPlaylistPath = "New";
          mPlaylist     = pu::playlistModule().create();
          mPlaylistModel->setPlaylist( mPlaylist.get() );
        }
        mPlaylistModel->addSong(info.absoluteFilePath().toLatin1());
        songsAdded = true;
      }
    }
  }

  e->acceptProposedAction();
  mOpMutex->unlock();
  if (songsAdded)
    refreshState();
}

void PlaylistWindow::refreshState() {
  if (mState == OpState_Executing) {
    setOpState(OpState_Executing);
    return;
  }

  int songOp = PlaylistSongOp_First + mSongOperatorComboBox->currentIndex();
  switch( songOp ) {
  case PlaylistSongOp_Move:
  case PlaylistSongOp_Copy:
    setOpState(mPlaylist && mPlaylist->songCount() > 0 && !mDestinationPath.isEmpty() ? OpState_Valid : OpState_Invalid);
    break;
  case PlaylistSongOp_Delete:
    setOpState(mPlaylist && mPlaylist->songCount() > 0 ? OpState_Valid : OpState_Invalid);
    break;
  case PlaylistSongOp_Clear:
    setOpState(mPlaylist && mPlaylist->songCount() > 0 ? OpState_Valid : OpState_Invalid);
    break;
  default:
    qDebug() << " Invalid song operation";
  };
}

void PlaylistWindow::customContextMenu(const QPoint& p) {
  if (nullptr != mPlaylistView->selectionModel()) {
    QMenu menu(this);

    QAction* openInExplorer = new QAction(tr("&Open location..."), this);
    openInExplorer->setStatusTip(tr("Open file location"));
    openInExplorer->setShortcut(tr("Ctrl+O"));
    lconnect(openInExplorer, SIGNAL(triggered()), [this]() { 
      const pu::Song* song = selectedSong();
      if (nullptr != song) {
        //QString path = QDir::toNativeSeparators(song->path());
        QDesktopServices::openUrl(QUrl::fromLocalFile(song->path()));
      }
    });
    menu.addAction(openInExplorer);
    menu.exec(QCursor::pos());
  }
}

const pu::Song* PlaylistWindow::selectedSong() const {
  if (!mPlaylist || mState == OpState_Executing) return nullptr;

  auto* model = this->mPlaylistView->selectionModel();
  if (nullptr == model) return nullptr;

  auto rows = model->selectedRows();
  if( rows.empty() ) return nullptr;

  auto index = rows.at(0);
  if (index.row() < 0 || index.row() >= (int)mPlaylist->songCount()) return nullptr;

  const auto& song = mPlaylist->song((size_t)index.row());
  return song.empty() ? nullptr : &song;
}

int PlaylistWindow::setFileProgress( int value ) {
  emit fileProgressChanged( value );
  if (mFileTime.isNull()) {
    mFileTime.start(); 
    return 1;
  }
  else
    return mFileTime.restart();
}

int PlaylistWindow::setOpProgress( int value ) {
  emit opProgressChanged( value );
  if (value >= (int)mPlaylist->songCount()) {
    setOpState(OpState_Complete);
  }
  if (mOpTime.isNull()) {
    mOpTime.start(); 
    return 1;
  }
  else
    return mOpTime.restart();
}

