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

#include "utils.h"

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
  typedef std::function<void(const char*,const pu::Song&)> A;
  typedef std::function<void(const char*)>                 B;
  typedef std::function<void(bool)>                        C;

  QtOpListener(A a, B b, C c) : mA(a), mB(b), mC(c) { }

  void beginOp(const char* opName, const pu::Song& song) const {
    mA(opName, song);
  }

  void beginOp(const char* opName) const {
    mB(opName);
  }

  void endOp(bool success) const {
    mC(success);
  }

  A mA;
  B mB;
  C mC;
};

///////////////////////////////////////////////////////////////////////////
 
static QWidget* createOpsWidget(
  QWidget* parentCallback, QWidget* parent, QComboBox** comboBox, QPushButton** actionButton,
  QLabel** fileLabel, QProgressBar** fileProgress,
  QLabel** opLabel,   QProgressBar** opProgress,
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
  *opProgress               = new QProgressBar(widget);
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

PlaylistWindow::PlaylistWindow() : mState(OpStates) {

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
    createOpsWidget(this,mPlaylistViews,&mSongOperatorComboBox,&mExecuteButton,&mFileLabel,&mFileProgress,&mOpLabel,&mOpProgress,
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
  mPlaylistView->setItemDelegateForColumn(0, new ImageDelegate(bottomWidget));
  mPlaylistView->setColumnWidth(0, 15);
  mPlaylistView->setSelectionBehavior(QAbstractItemView::SelectRows);
  mPlaylistView->horizontalHeader()->setStretchLastSection(true);
  mPlaylistView->verticalHeader()->hide();
  mPlaylistView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  mPlaylistView->setSelectionMode(QAbstractItemView::SingleSelection);

  bottomLayout->addWidget( mPlaylistView );

  // Full View
  layout->setSpacing(0);
  layout->addWidget(topWidget);
  layout->addWidget(bottomWidget);
  //layout->setSizeConstraint(QLayout::SetFixedSize);

  mOpListener.reset( new QtOpListener( 
    [this](const char* opName, const pu::Song& song) { },
    [this](const char* opName) { },
    [this](bool success) { }
  ));

  connect(this, SIGNAL(stateChanged()), 
          this, SLOT(refreshOpState()));

  mSongOperatorComboBox->setCurrentIndex(0);
  mPlaylistViews->setCurrentIndex(0);
  refreshState();

  setWindowTitle(tr("Playlist Utilities"));

  setAcceptDrops(true);
}

void PlaylistWindow::executeSongOp() {
  if (mState != OpState_Executing && mState != OpState_Invalid) {
    qDebug() << "Executing Song Op: " << mSongOperatorComboBox->currentText();
  } else {
    qDebug() << "Pausing Song Op: " << mSongOperatorComboBox->currentText();
    // Handle pause
  }
  
}

PlaylistWindow::PlaylistOp PlaylistWindow::currentOp() const {
  return PlaylistOp_None;
}

PlaylistWindow::~PlaylistWindow() {

}

/*
QWidget* PlaylistWindow::createOpsWidget(QWidget* parent, QComboBox** comboBox, QTextEdit** textEdit, const char* ops, const char* executeOp, const char* refreshOp) {
  auto* widget              = new QWidget(parent);
  auto* layout              = new QVBoxLayout(widget);
  auto* topWidget           = new QWidget(widget);
  auto* topLayout           = new QHBoxLayout(topWidget);
  auto* loadPlaylistButton  = new QPushButton(tr("Load playlist"), topWidget);
  auto* executeButton       = new QPushButton(tr("Execute"), topWidget);
  (*comboBox) = new QComboBox(topWidget);
  (*comboBox)->addItems( QString(ops).split(" ",QString::SkipEmptyParts) );

  topLayout->setSpacing(30);
  topLayout->addSpacing(30);
  topLayout->addWidget(loadPlaylistButton);
  topLayout->addWidget(*comboBox);
  topLayout->addWidget(executeButton);
  topLayout->addSpacing(30);
  topLayout->addStretch();

  *textEdit = new QTextEdit;//(widget);
  (*textEdit)->setMaximumHeight(150);
  (*textEdit)->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

  layout->addWidget(topWidget);
  layout->addWidget(*textEdit);
  layout->addStretch();

  connect(executeButton, SIGNAL(clicked()), 
          this,          executeOp);

  connect(mPlaylistOperatorComboBox, SIGNAL(activated(int)),
          this,                      refreshOp);

  return widget;
}
*/


QWidget* PlaylistWindow::createSettingsWidget(QWidget* parent) {
  auto* widget = new QWidget(parent);
  auto* layout = new QHBoxLayout(widget);
  widget->setLayout(layout);
  return widget;
}

void PlaylistWindow::refreshOpState() {
  mFileLabel->setText(fileText());
  mOpLabel->setText(opText());
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
  if (opState != mState) {
    mState = opState;
  }
}

void PlaylistWindow::dragEnterEvent(QDragEnterEvent* e) {
  if (mState != OpState_Executing && e->mimeData()->hasFormat("text/uri-list"))
    e->acceptProposedAction();
}

void PlaylistWindow::setPlaylist(QString playlistPath) {
  mPlaylistPath = playlistPath;
  mPlaylist = pu::playlistModule().importFromFile(mPlaylistPath.toLatin1());
  mPlaylistModel->setPlaylist( mPlaylist.get() );
  refreshState();
}

void PlaylistWindow::setDestination(QString destinationPath) {
  mDestinationPath = destinationPath;
  refreshState();
}

void PlaylistWindow::dropEvent(QDropEvent* e) {
  
  if (e->mimeData()->hasUrls()) {
    foreach(QUrl url, e->mimeData()->urls()) {
      QFileInfo info( url.toLocalFile() );
      if (info.isFile() && info.exists()) {
        setPlaylist(info.absoluteFilePath());
        break;
      } else if (info.isDir() && info.exists()) {
        setDestination(info.absolutePath());
        break;
      } 
    }
  }

  e->acceptProposedAction();
}

void PlaylistWindow::refreshState() {
  if (mState == OpState_Executing)
    return;

  int songOp = PlaylistSongOp_First + mSongOperatorComboBox->currentIndex();
  switch( songOp ) {
  case PlaylistSongOp_Move:
  case PlaylistSongOp_Copy:
    setOpState(mPlaylist && mPlaylist->songCount() > 0 && !mDestinationPath.isEmpty() ? OpState_Valid : OpState_Invalid);
    emit stateChanged();
    break;
  case PlaylistSongOp_Delete:
    setOpState(mPlaylist && mPlaylist->songCount() > 0 ? OpState_Valid : OpState_Invalid);
    emit stateChanged();
    break;
  default:
    qDebug() << " Invalid song operation";
  };
}