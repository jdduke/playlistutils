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

enum GuiStringKey {
  LOAD_PLAYLIST = 0,
  LOAD_DESTINATION,
  CLOSE,
  CANCEL,
  EXECUTE,
  CANCELLED,
  OK,
  FAILED,
  ACTION,
  TITLE,
  NEW,
  RESUME,
  CANCELLING,
  PAUSE,
  OPEN_PLAYLIST,
  OPEN_DESTINATION,
  OP_LIST,
  BEHAVIOR_LIST,
  SORT_LIST,
  STRING_COUNT
};

static QString GuiStr[STRING_COUNT] = {
  QWidget::tr("Click or drop playlist here"),
  QWidget::tr("Click or drop destination here"),
  QWidget::tr("Close"),
  QWidget::tr("Cancel"),
  QWidget::tr("Execute"),
  QWidget::tr("Cancelled"),
  QWidget::tr("OK"),
  QWidget::tr("Failed"),
  QWidget::tr("Action"),
  QWidget::tr("Playlist Utilities"),
  QWidget::tr("New"),
  QWidget::tr("Resume"),
  QWidget::tr("Cancelling"),
  QWidget::tr("Pause"),
  QWidget::tr("Open Playlist"),
  QWidget::tr("Open Destination"),
  QString("Copy Songs;Move Songs;Delete Songs;Clear Songs;Sort Songs"),
  QString("Replace ;Replace Older  ;Skip  ;Ask   "),
  QString("Path;Title;Artist;Size"),
};

/////////////////////////////////////////////////////////////////////////////

template<typename T>
static inline bool inRange(const T& val, const T& low, const T& high) {
  return (low <= val && val <= high);
}

template<typename T>
static inline bool bound(const T& val, const T& low, const T& high) {
  return std::max(low, std::min(val, high));
}

/////////////////////////////////////////////////////////////////////////////

template<class T>
class TSongComparator : public pu::SongComparator {
public:
  TSongComparator(T comparator) : mComparator(comparator) { }
  virtual bool operator()( const pu::Song& song1, const pu::Song& song2 ) const {
    return mComparator(song1, song2);
  }
protected:
  T mComparator;
};

/////////////////////////////////////////////////////////////////////////////

static pu::SongComparator songComparator(PlaylistWindow::PlaylistSort sort, bool bReverse = false) {
  typedef std::function<bool(const pu::Song& s1, const pu::Song& s2)> CompareFunc;
  typedef std::function<bool(int,int)> CompareI;
  typedef std::function<bool(size_t,size_t)> CompareU;

  CompareFunc func;
  auto compI = bReverse ? CompareI(std::greater<int>())    : CompareI(std::less<int>());
  auto compU = bReverse ? CompareU(std::greater<size_t>()) : CompareU(std::less<size_t>());

  switch (sort) {
  case PlaylistWindow::PlaylistSort_Path:
    func = [=](const pu::Song& song1, const pu::Song& song2) {
      return compI(std::strcmp( song1.path(), song2.path()), 0);
    };
    break;
  case PlaylistWindow::PlaylistSort_Artist:
    func = [=](const pu::Song& song1, const pu::Song& song2) {
      return compI(std::strcmp( song1.artist(), song2.artist()), 0);
    };
    break;
  case PlaylistWindow::PlaylistSort_Size:
    func = [=](const pu::Song& song1, const pu::Song& song2) {
      return compU(song1.size(), song2.size());
    };
    break;
  case PlaylistWindow::PlaylistSort_Title:
  default:
    func = [=](const pu::Song& song1, const pu::Song& song2) {
      return compI(std::strcmp(song1.title(), song2.title()), 0);
    };
  };

  return TSongComparator<CompareFunc>(func);
}

/////////////////////////////////////////////////////////////////////////////

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
  QWidget* parentCallback,
  QWidget* parent,
  QComboBox** comboBox,
  QComboBox** behaviorComboBox,
  QComboBox** sortComboBox,
  QPushButton** actionButton,
  QPushButton** closeButton,
  QPushButton** fileButton,
  QProgressBar** fileProgress,
  QPushButton** opButton,
  QLabel** opLabel,
  QProgressBar** opProgress,
  const char* ops,
  const char* behaviors,
  const char* sorts,
  const char* executeOp,
  const char* closeOp,
  const char* fileOp,
  const char* opOp,
  const char* refreshOp) {

  auto* widget              = new QWidget(parent);
  auto* topWidget           = new QWidget(widget);
  auto* midWidget           = new QWidget(widget);
  auto* bottomWidget        = new QWidget(widget);
  auto* layout              = new QVBoxLayout(widget);
  auto* topLayout           = new QHBoxLayout(topWidget);
  auto* midLayout           = new QHBoxLayout(midWidget);
  auto* bottomLayout        = new QHBoxLayout(bottomWidget);
  *closeButton              = new QPushButton(GuiStr[CLOSE], midWidget);
  *actionButton             = new QPushButton(GuiStr[EXECUTE], midWidget);
  *fileButton               = new QPushButton(GuiStr[LOAD_PLAYLIST], topWidget);
  *opButton                 = new QPushButton(GuiStr[LOAD_DESTINATION], bottomWidget);
  *opLabel                  = new QLabel("", bottomWidget);
  *fileProgress             = new QProgressBar(widget);
  *opProgress               = new QProgressBar(widget);
  *comboBox                 = new QComboBox(midWidget);
  *behaviorComboBox         = new FlatComboBox(topWidget);
  *sortComboBox             = new QComboBox(midWidget);

  (*fileProgress)->setMinimum(0);
  (*opProgress)->setMinimum(0);
  (*fileProgress)->setTextVisible(true);
  (*opProgress)->setTextVisible(true);
  (*fileButton)->setFlat(true);
  (*opButton)->setFlat(true);
  (*comboBox)->addItems( QString(ops).split(";",QString::SkipEmptyParts) );
  (*behaviorComboBox)->addItems( QString(behaviors).split(";",QString::SkipEmptyParts) );
  (*sortComboBox)->addItems( QString(sorts).split(";",QString::SkipEmptyParts) );
  (*sortComboBox)->setVisible(false);

  topLayout->addWidget(*fileButton);
  //topLayout->addSpacing(30);
  topLayout->addStretch();
  topLayout->addWidget(*behaviorComboBox);

  midLayout->addSpacing(10);
  midLayout->addWidget(new QLabel(GuiStr[ACTION] + ": ", widget));
  midLayout->addWidget(*comboBox);
  midLayout->addSpacing(20);
  midLayout->addWidget(*actionButton);
  midLayout->addSpacing(20);
  midLayout->addWidget(*closeButton);
  midLayout->addStretch();

  bottomLayout->addWidget(*opButton);
  bottomLayout->addStretch();
  bottomLayout->addWidget(*opLabel);

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

  QWidget::connect(*actionButton,     SIGNAL(clicked()),                parentCallback, executeOp);
  QWidget::connect(*fileButton,       SIGNAL(clicked()),                parentCallback, fileOp);
  QWidget::connect(*opButton,         SIGNAL(clicked()),                parentCallback, opOp);
  QWidget::connect(*closeButton,      SIGNAL(clicked()),                parentCallback, closeOp);
  QWidget::connect(*comboBox,         SIGNAL(currentIndexChanged(int)), parentCallback, refreshOp);

  return widget;
}

///////////////////////////////////////////////////////////////////////////

PlaylistWindow::PlaylistWindow(const QString& playlistPath, const QString& destPath)
  : mState(OpStates),
    mOpMutex(new tthread::mutex),
    mOpCondition(new tthread::condition_variable) {

  auto* layout    = new QVBoxLayout(this);

  // Top View
  auto* topWidget =
    createOpsWidget(this,
                    this,
                    &mSongOperatorComboBox,
                    &mFileBehaviorComboBox,
                    &mSortComboBox,
                    &mExecuteButton,
                    &mCloseButton,
                    &mFileButton,
                    &mFileProgress,
                    &mOpButton,
                    &mOpLabel,
                    &mOpProgress,
                    GuiStr[OP_LIST].toLatin1(),
                    GuiStr[BEHAVIOR_LIST].toLatin1(),
                    GuiStr[SORT_LIST].toLatin1(),
                    SLOT(executeSongOp()),
                    SLOT(closeSongOp()),
                    SLOT(openPlaylistOp()),
                    SLOT(openDestOp()),
                    SLOT(refreshState()));

  // Bottom View
  auto* bottomWidget = new QWidget(this);
  auto* bottomLayout = new QHBoxLayout(bottomWidget);

  mPlaylistModel = new PlaylistModel(bottomWidget);
  mPlaylistView  = new QTableView(bottomWidget);
  mPlaylistView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  mPlaylistView->setColumnWidth(PlaylistModel::Column_Size,   7);
  mPlaylistView->setColumnWidth(PlaylistModel::Column_Status, 10);
  mPlaylistView->setModel(mPlaylistModel);
  mPlaylistView->setItemDelegateForColumn(0, new ImageDelegate(QString(" :/image/success :/image/failure").split(" "), bottomWidget));
  mPlaylistView->setColumnWidth(0, 15);
  mPlaylistView->setSelectionBehavior(QAbstractItemView::SelectRows);
  mPlaylistView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  mPlaylistView->setSelectionMode(QAbstractItemView::SingleSelection);
  mPlaylistView->setContextMenuPolicy(Qt::CustomContextMenu);
  mPlaylistView->horizontalHeader()->setStretchLastSection(true);
  mPlaylistView->verticalHeader()->hide();

  connect(mPlaylistView, SIGNAL(customContextMenuRequested(const QPoint&)),
          this,          SLOT(customContextMenu(const QPoint&)));
  lconnect<int>(mPlaylistModel, SIGNAL(rowModified(QVariant)), [this](int row) {
    this->mPlaylistView->scrollTo(this->mPlaylistModel->index(row, 0));
  });

  bottomLayout->addWidget(mPlaylistView);

  // Full View
  layout->setSpacing(0);
  layout->addWidget(topWidget);
  layout->addWidget(bottomWidget);

  // Op Listener
  mOpListener.reset(createOpListener());
  pu::playlistModule().setOpListener(mOpListener.get());

  // File Handler
  //mFileHandler.reset( new QtFileHandler() );
  //pu::playlistModule().setFileHandler(mFileHandler.get());

  // Events
  connect(this, SIGNAL(stateChanged()),
          this, SLOT(refreshOpState()));

  connect(this, SIGNAL(cancelled()),
          this, SLOT(cancelOps()));

  connect(this, SIGNAL(titleChanged(const QString&)),
          this, SLOT(setWindowTitle(const QString&)));

  connect(this,          SIGNAL(fileProgressChanged(int)),
          mFileProgress, SLOT(setValue(int)), Qt::BlockingQueuedConnection);

  connect(this,          SIGNAL(opProgressChanged(int)),
          mOpProgress,   SLOT(setValue(int)), Qt::BlockingQueuedConnection);

  setPlaylist(playlistPath);
  setDestination(destPath);

  emit titleChanged(GuiStr[TITLE]);
  setAcceptDrops(true);
}

PlaylistWindow::~PlaylistWindow() {
  mPlaylistModel->setPlaylist(nullptr);
  mPlaylistView->setModel(nullptr);
  mPlaylistView->clearSelection();
  mPlaylistView->clearSpans();
  {
    tthread::lock_guard<tthread::mutex> guard(*mOpMutex);
    mState = OpState_Shutdown;
    mOpCondition->notify_all();
  }

  if (mOpThread && mOpThread->joinable())
    mOpThread->join();
}

/////////////////////////////////////////////////////////////////////////////

void PlaylistWindow::executeSongOp() {
  if (mState == OpState_Paused) {
    setOpState(OpState_Executing);
  } else if (mState != OpState_Executing && mState != OpState_Invalid) {
    auto executeOp = [](void* data) {
      auto* playlistWindow = static_cast<PlaylistWindow*>(data);
      if (nullptr == playlistWindow) return;

      auto* playlist = playlistWindow->playlist();
      if (nullptr == playlist) return;

      switch(playlistWindow->currentOp()) {
      case PlaylistWindow::PlaylistOp_Move:
        playlist->applyOp(pu::MoveSongOp(playlistWindow->selectedDestination()));
        break;
      case PlaylistWindow::PlaylistOp_Delete:
        playlist->applyOp(pu::DeleteSongOp());
        break;
      case PlaylistWindow::PlaylistOp_Copy:
        playlist->applyOp(pu::CopySongOp(playlistWindow->selectedDestination()));
        break;
      case PlaylistWindow::PlaylistOp_Clear:
        playlistWindow->setOpProgress( (int)playlistWindow->songCount() );
        playlistWindow->setPlaylist("");
        break;
      case PlaylistWindow::PlaylistOp_Sort:
        playlist->apply(pu::SortSongsOp(songComparator(playlistWindow->selectedSort())));
        playlistWindow->refreshPlaylist();
        playlistWindow->setOpProgress( (int)playlistWindow->songCount() );
        break;
      default:
        break;
      }
    };
    setOpState(OpState_Executing);
    clearOps();
    mOpThread.reset( new tthread::thread(executeOp, (void*)this) );
  } else {
    setOpState(OpState_Paused);
  }
}

void PlaylistWindow::closeSongOp() {
  if (readOnly()) {
    emit cancelled();
  } else {
    QApplication::quit();
  }
}

void PlaylistWindow::openPlaylistOp() {
  QFileInfo playlistFile( QFileDialog::getOpenFileName(this,
                                                       GuiStr[OPEN_PLAYLIST],
                                                       "",
                                                       tr("Playlist Files (*.m3u *.pls *.wpl)")) );
  if (playlistFile.exists())
    setPlaylist(playlistFile.absoluteFilePath());
}

void PlaylistWindow::openDestOp() {
  QFileInfo destinationDir( QFileDialog::getExistingDirectory(this,
                                                              GuiStr[OPEN_DESTINATION]) );

  if (destinationDir.exists() && destinationDir.isDir())
    setDestination(destinationDir.absoluteFilePath());
}

void PlaylistWindow::cancelOps() {
  setOpState(OpState_Cancel);
  if (mOpThread->joinable()) {
    mOpThread->join();
  }

  for (auto i = mOpProgress->value(); i < (int)songCount(); ++i) {
    auto playlistModelIndexI = mPlaylistModel->index( i, PlaylistModel::Column_Image );
    auto playlistModelIndexS = mPlaylistModel->index( i, PlaylistModel::Column_Status );
    mPlaylistModel->setData( playlistModelIndexI, PlaylistModel::Status_Failure );
    mPlaylistModel->setData( playlistModelIndexS, GuiStr[CANCELLED] );
  }
  if (songCount() > 0)
    mPlaylistView->scrollTo(mPlaylistModel->index(0, 0));

  setOpState(OpState_Valid);
}

void PlaylistWindow::clearOps() {
  for (auto i = 0; i < (int)songCount(); ++i) {
    auto playlistModelIndexI = mPlaylistModel->index( i, PlaylistModel::Column_Image );
    auto playlistModelIndexS = mPlaylistModel->index( i, PlaylistModel::Column_Status );
    mPlaylistModel->setData( playlistModelIndexI, PlaylistModel::Status_Empty );
    mPlaylistModel->setData( playlistModelIndexS, "" );
  }
  if (songCount() > 0)
    mPlaylistView->scrollTo(mPlaylistModel->index(0, 0));
}

bool PlaylistWindow::beginOp(const char* opName, const pu::Song& song) {
  auto listIndex          = mOpProgress->value();
  auto playlistModelIndex = mPlaylistModel->index( listIndex, PlaylistModel::Column_Status );
  mPlaylistModel->setData( playlistModelIndex, opName );

  auto songString = toString(song);
  if (songString.isEmpty())
    songString = QString(song.path());
  if (!songString.isEmpty() && !songString.isNull())
    mFileButton->setText( songString.mid(0, 60) );
  setFileProgress( 0 );
  emit titleChanged(QString(opName) + ": " + mOpProgress->text());
  return true;
}

bool PlaylistWindow::beginOp(const char* opName) {
  auto listIndex          = mOpProgress->value();
  auto playlistModelIndex = mPlaylistModel->index( listIndex, PlaylistModel::Column_Status );
  mPlaylistModel->setData( playlistModelIndex, opName );
  mFileButton->setText( opName );
  mFileProgress->setValue( 0 );
  emit titleChanged(QString(opName) + ": " + mOpProgress->text());
  return true;
}

bool PlaylistWindow::endOp(bool success) {
  for (auto i = mOpProgress->value(); i < (int)songCount(); ++i) {
    auto playlistModelIndexI = mPlaylistModel->index( i, PlaylistModel::Column_Image );
    auto playlistModelIndexS = mPlaylistModel->index( i, PlaylistModel::Column_Status );
    mPlaylistModel->setData( playlistModelIndexI, success ? PlaylistModel::Status_Success : PlaylistModel::Status_Failure );
    mPlaylistModel->setData( playlistModelIndexS, success ? GuiStr[OK] : GuiStr[FAILED] );
  }
  if (songCount() > 0) {
    mPlaylistView->scrollTo(mPlaylistModel->index(0, 0));
  }

  auto sizeInSongs = (double)mPlaylist->songCount();
  auto timeInS     = (double)setFileProgress( success ? mFileProgress->maximum() : 0 ) / 1000;
  if (timeInS > 0.) {
    mOpLabel->setText(QString::number(sizeInSongs/timeInS, 'g', 4) + " Songs/s");
  }
  return true;
}

PlaylistWindow::OpState PlaylistWindow::currentState() const {
  return mState;
}

PlaylistWindow::PlaylistOp PlaylistWindow::currentOp() const {
  return (PlaylistWindow::PlaylistOp)(PlaylistOp_First + mSongOperatorComboBox->currentIndex());
}

bool PlaylistWindow::readOnly() const {
  return (mState == OpState_Executing ||
          mState == OpState_Paused    ||
          mState == OpState_Shutdown  ||
          mState == OpState_Cancel);
}

QWidget* PlaylistWindow::createSettingsWidget(QWidget* parent) {
  auto* widget = new QWidget(parent);
  auto* layout = new QHBoxLayout(widget);
  widget->setLayout(layout);
  return widget;
}

QOpListener* PlaylistWindow::createOpListener() {
  auto handleCancel = [this]() -> bool {
    //tthread::lock_guard<tthread::mutex> guard(*this->mOpMutex);
    return !(this->currentState() == OpState_Shutdown || this->currentState() == OpState_Cancel);
  };
  auto handlePause = [this]() -> bool {
    tthread::lock_guard<tthread::mutex> guard(*this->mOpMutex);
    while (this->currentState() == PlaylistWindow::OpState_Paused) {
      this->mOpCondition->wait(*this->mOpMutex);
    }
    return true;
  };

  return new QOpListener(
    [=,this](const char* opName, const pu::Song& song) -> bool {
      return handlePause() && handleCancel() && this->beginOp(opName, song);
    },
    [=,this](const char* opName) -> bool {
      return handlePause() && handleCancel() && this->beginOp(opName);
    },
    [=,this](bool success) -> bool {
      return handleCancel() && this->endOp(success);
    }
  );
}

void PlaylistWindow::refreshOpState() {
  mFileButton->setText(fileText());
  mOpButton->setText(opText());
  mOpLabel->setText("");
  mCloseButton->setText(GuiStr[CLOSE]);

  bool ro = readOnly();
  mSongOperatorComboBox->setDisabled(ro);
  mFileBehaviorComboBox->setDisabled(ro);
  mSortComboBox->setDisabled(ro);
  mFileButton->setDisabled(ro);
  mOpButton->setDisabled(ro);

  mExecuteButton->setEnabled(true);
  mExecuteButton->setText(GuiStr[EXECUTE]);
  mCloseButton->setEnabled(true);
  setWindowTitle(GuiStr[TITLE]);

  switch (mState) {
  case OpState_Invalid:
    mExecuteButton->setEnabled(false);
  case OpState_Valid:
    mFileProgress->setValue(0);
    mOpProgress->setValue(0);
    break;
  case OpState_Executing:
    mExecuteButton->setText(GuiStr[PAUSE]);
    mCloseButton->setText(GuiStr[CANCEL]);
    mFileProgress->setMaximum(100);
    mOpProgress->setMaximum((int)songCount());
    break;
  case OpState_Cancel:
    mExecuteButton->setEnabled(false);
    mCloseButton->setEnabled(false);
    mExecuteButton->setText(GuiStr[CANCELLING]);
    mCloseButton->setText(GuiStr[CANCELLING]);
    break;
  case OpState_Paused:
    mExecuteButton->setText(GuiStr[RESUME]);
    mCloseButton->setText(GuiStr[CANCEL]);
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
  switch(currentOp()) {
  case PlaylistOp_Move:
  case PlaylistOp_Copy:
    return mDestinationPath.isEmpty() ? GuiStr[LOAD_DESTINATION] : mDestinationPath;
  case PlaylistOp_Delete:
  case PlaylistOp_Clear:
  case PlaylistOp_Sort:
  default:
    return "";
  };
}

QString PlaylistWindow::fileText() const {
  switch(currentOp()) {
  case PlaylistOp_Move:
  case PlaylistOp_Copy:
  case PlaylistOp_Delete:
  case PlaylistOp_Sort:
    return mState == OpState_Executing ? mFileText :
      mPlaylistPath.isEmpty() ? GuiStr[LOAD_PLAYLIST] : mPlaylistPath;
  default:
    return "";
  };
}

void PlaylistWindow::setOpState(OpState opState) {
  tthread::lock_guard<tthread::mutex> guard(*mOpMutex);
  mState = opState;
  mOpCondition->notify_all();
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
  refreshPlaylist();
}

void PlaylistWindow::refreshPlaylist() {
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
          mPlaylistPath = GuiStr[NEW];
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

  PlaylistOp songOp = currentOp();
  switch( songOp ) {
  case PlaylistOp_Move:
  case PlaylistOp_Copy:
    setOpState(songCount() > 0 && !mDestinationPath.isEmpty() ? OpState_Valid : OpState_Invalid);
    break;
  case PlaylistOp_Delete:
  case PlaylistOp_Clear:
  case PlaylistOp_Sort:
    setOpState(songCount() > 0 ? OpState_Valid : OpState_Invalid);
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

  auto* model = mPlaylistView->selectionModel();
  if (nullptr == model) return nullptr;

  auto rows = model->selectedRows();
  if( rows.empty() ) return nullptr;

  auto index = rows.at(0);
  if (index.row() < 0 || index.row() >= (int)songCount()) return nullptr;

  const auto& song = mPlaylist->song((size_t)index.row());
  return song.empty() ? nullptr : &song;
}

PlaylistWindow::PlaylistOp PlaylistWindow::selectedOp() const {
  return (PlaylistOp)mSongOperatorComboBox->currentIndex();
}

PlaylistWindow::PlaylistSort PlaylistWindow::selectedSort() const {
  return (PlaylistSort)mSortComboBox->currentIndex();
}

PlaylistWindow::FileBehavior PlaylistWindow::selectedBehavior() const {
  return (FileBehavior)mFileBehaviorComboBox->currentIndex();
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
  if (value >= (int)songCount()) {
    setOpState(OpState_Complete);
  }
  if (mOpTime.isNull()) {
    mOpTime.start();
    return 1;
  }
  else
    return mOpTime.restart();
}

size_t PlaylistWindow::songCount() const {
  return mPlaylist ? mPlaylist->songCount() : 0;
}
