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

class QtOpListener : public pu::OpListener {
public:
  typedef std::function<bool(const char*,const pu::Song&)> A;
  typedef std::function<bool(const char*)>                 B;
  typedef std::function<bool(bool)>                        C;

  QtOpListener(A a, B b, C c) : mA(a), mB(b), mC(c) { }

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
  QWidget* parentCallback, QWidget* parent, QComboBox** comboBox, QPushButton** actionButton,
  QLabel** fileLabel,                    QProgressBar** fileProgress,
  QLabel** opLabel,   QLabel** opLabel2, QProgressBar** opProgress, 
  const char* ops, const char* executeOp, const char* refreshOp) {

  auto* widget              = new QWidget(parent);
  auto* layout              = new QVBoxLayout(widget);
  auto* topWidget           = new QWidget(widget);
  auto* topLayout           = new QHBoxLayout(topWidget);
  auto* midWidget           = new QWidget(widget);
  auto* midLayout           = new QHBoxLayout(midWidget);
  auto* bottomWidget        = new QWidget(widget);
  auto* bottomLayout        = new QHBoxLayout(bottomWidget);
  auto* closeButton         = new QPushButton(QWidget::tr("Close"), midWidget);
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

  //topLayout->setSpacing(30);
  //topLayout->addSpacing(30);
  topLayout->addWidget(*fileLabel);
  topLayout->addSpacing(30);
  topLayout->addStretch();

  midLayout->addSpacing(10);
  midLayout->addWidget(new QLabel("Action: ", widget));
  midLayout->addWidget(*comboBox);
  midLayout->addSpacing(20);
  midLayout->addWidget(*actionButton);
  midLayout->addSpacing(20);
  midLayout->addWidget(closeButton);
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
  
  QWidget::connect(*actionButton, SIGNAL(clicked()),            parentCallback, executeOp);

  QWidget::connect(*comboBox, SIGNAL(currentIndexChanged(int)), parentCallback, refreshOp);

  lconnect(closeButton, SIGNAL(clicked()), []() {
    QApplication::quit();
  });

  return widget;
}

///////////////////////////////////////////////////////////////////////////

PlaylistWindow::PlaylistWindow() 
  : mState(OpStates), mOpMutex(new tthread::mutex), mOpCondition(new tthread::condition_variable) {

  lconnect(this, SIGNAL(pushMsg(const char*)), [](){
    qDebug() << "New Message";
  });

  auto* layout                  = new QVBoxLayout(this);
  auto* topWidget               = new QWidget(this);
  auto* topLayout               = new QHBoxLayout(topWidget);
  auto* bottomWidget            = new QWidget(this);
  auto* bottomLayout            = new QHBoxLayout(bottomWidget);
  
  // Top View
  auto* selectViewButtons       = new QWidget(topWidget);
  auto* selectViewButtonsLayout = new QVBoxLayout(selectViewButtons);
  auto* playlistViewButton      = new QPushButton(tr("Edit Playlist"), this);
  auto* songViewButton          = new QPushButton(tr("Edit Playlist Songs"), this);
  auto* settingsViewButton      = new QPushButton(tr("Edit Settings"), this);

  playlistViewButton->setCheckable(true);
  songViewButton->setCheckable(true);
  settingsViewButton->setCheckable(true);
  playlistViewButton->setChecked(true);

  mViewButtonGroup = new QButtonGroup(this);
  mViewButtonGroup->addButton(playlistViewButton, 0);
  mViewButtonGroup->addButton(songViewButton,     1);
  mViewButtonGroup->addButton(settingsViewButton, 2);

  selectViewButtonsLayout->setSpacing(30);
  selectViewButtonsLayout->insertSpacing(0, 30);
  selectViewButtonsLayout->addWidget(playlistViewButton);
  selectViewButtonsLayout->addWidget(songViewButton);
  selectViewButtonsLayout->addWidget(settingsViewButton);
  selectViewButtonsLayout->addStretch();
  //selectViewButtonsLayout->insertSpacing(3, 25);
  //selectViewButtonsLayout->setStretch(1);
  //selectViewButtonsLayout->SetMinimumSize(QLayout::SetFixedSize);

  mPlaylistViews = new QStackedWidget(topWidget);
  /*mPlaylistViews->addWidget(
    createOpsWidget(this,mPlaylistViews,&mPlaylistOperatorComboBox,&mPlaylistFileLabel,&mPlaylistFileProgress,&mPlaylistOpLabel,&mPlaylistOpProgress,
    "New Move Delete Copy Merge Sort",SLOT(executePlaylistOp()),SLOT(refreshPlaylistOp(int))));*/
  mPlaylistViews->addWidget(
    createOpsWidget(this,mPlaylistViews,&mSongOperatorComboBox,&mExecuteButton,&mFileLabel,&mFileProgress,&mOpLabel,&mOpLabel2,&mOpProgress,
    "    Move Delete Copy           ",SLOT(executeSongOp()),    SLOT(refreshState())));
  //mPlaylistViews->addWidget(createSettingsWidget(mPlaylistViews));
  
  //connect(mViewButtonGroup, SIGNAL(buttonClicked(int)), mPlaylistViews, SLOT(setCurrentIndex(int)));
  lconnect(mViewButtonGroup, SIGNAL(buttonClicked(int)), [this]() {
    qDebug() << "Setting view: " << mViewButtonGroup->checkedId();
    mPlaylistViews->setCurrentIndex(mViewButtonGroup->checkedId());
  });

  //topLayout->addWidget(selectViewButtons);
  topLayout->addWidget(mPlaylistViews);
  //topLayout->setSizeConstraint(QLayout::Size)

  // Bottom View
  mPlaylistView = new QTableView(bottomWidget);
  mPlaylistView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
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

  bottomLayout->addWidget( mPlaylistView );

  // Full View
  layout->setSpacing(0);
  layout->addWidget(topWidget);
  layout->addWidget(bottomWidget);
  //layout->setSizeConstraint(QLayout::SetFixedSize);

  mOpListener.reset( new QtOpListener( 
    [this](const char* opName, const pu::Song& song) -> bool {
      {
        tthread::lock_guard<tthread::mutex> guard(*this->mOpMutex);
        while (this->currentState() == PlaylistWindow::OpState_Paused) {
          this->mOpCondition->wait(*this->mOpMutex);
          //tthread::this_thread::sleep_for(tthread::chrono::milliseconds(100));
        }
      }
      if (this->currentState() == OpState_Shutdown || this->currentState() == OpState_Cancel)
        return false;
      QString songPath(song.path());
      if (!songPath.isEmpty() && !songPath.isNull())
        this->mFileLabel->setText( songPath.mid(0, 70) );
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
  ));
  pu::playlistModule().setOpListener(mOpListener.get());

  //mFileHandler.reset( new QtFileHandler() );
  //pu::playlistModule().setFileHandler(mFileHandler.get());

  connect(this, SIGNAL(stateChanged()), 
          this, SLOT(refreshOpState()));

  connect(this,          SIGNAL(fileProgressChanged(int)),
          mFileProgress, SLOT(setValue(int)), Qt::BlockingQueuedConnection);

  connect(this,          SIGNAL(opProgressChanged(int)),
          mOpProgress,   SLOT(setValue(int)), Qt::BlockingQueuedConnection);

  mSongOperatorComboBox->setCurrentIndex(0);
  mPlaylistViews->setCurrentIndex(0);
  refreshState();

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

PlaylistWindow::OpState PlaylistWindow::currentState() const {
  return mState;
}

PlaylistWindow::PlaylistOp PlaylistWindow::currentOp() const {
  return (PlaylistWindow::PlaylistOp)mSongOperatorComboBox->currentIndex();
}


bool PlaylistWindow::readOnly() const {
  return (currentOp() == OpState_Executing ||
          currentOp() == OpState_Paused    ||
          currentOp() == OpState_Shutdown  ||
          currentOp() == OpState_Cancel);
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

void PlaylistWindow::refreshOpState() {
  mFileLabel->setText(fileText());
  mOpLabel->setText(opText());
  mOpLabel2->setText("");
  mSongOperatorComboBox->setDisabled(false);
  mExecuteButton->setEnabled(true);
  mExecuteButton->setText("Execute");
  switch (mState) {
  case OpState_Invalid:
    mExecuteButton->setEnabled(false);
  case OpState_Valid:
    mFileProgress->setValue(0);
    mOpProgress->setValue(0);
    break;
  case OpState_Executing:
    mExecuteButton->setText("Pause");
    mSongOperatorComboBox->setDisabled(true);
    mFileProgress->setMaximum(100);
    mOpProgress->setMaximum((int)mPlaylist->songCount());
    break;
  case OpState_Paused:
    mExecuteButton->setText("Resume");
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
  //tthread::lock_guard<tthread::mutex> guard(*mOpMutex);
  mPlaylistPath = playlistPath;
  mPlaylist = pu::playlistModule().createFromFile(mPlaylistPath.toLatin1());
  mPlaylistModel->setPlaylist( mPlaylist.get() );
  refreshState();
}

void PlaylistWindow::setDestination(QString destinationPath) {
  //tthread::lock_guard<tthread::mutex> guard(*mOpMutex);
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
  if (index.row() < 0 || index.row() >= mPlaylist->songCount()) return nullptr;

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
  if (value >= mPlaylist->songCount()) {
    setOpState(OpState_Complete);
  }
  if (mOpTime.isNull()) {
    mOpTime.start(); 
    return 1;
  }
  else
    return mOpTime.restart();
}

