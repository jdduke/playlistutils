/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#include "playlist_window.h"
#include "playlist_utils.h"
#include "utils.h"

#include <QtGui>

#include <QButtonGroup>
#include <QComboBox>
#include <QStackedWidget>

template<typename T>
static inline bool inRange(const T& val, const T& low, const T& high) {
  return (low <= val && val <= high);
}

///////////////////////////////////////////////////////////////////////////
 
static QWidget* createOpsWidget(
  QWidget* parentCallback, QWidget* parent, QComboBox** comboBox,
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
  auto* executeButton       = new QPushButton(QWidget::tr("Execute"), midWidget);
  auto* closeButton         = new QPushButton(QWidget::tr("Close"), midWidget);
  //auto* loadPlaylistButton  = new QPushButton(tr("Load playlist"), topWidget);
  *fileLabel                = new QLabel(QWidget::tr("Drop playlist here"), topWidget);
  *fileProgress             = new QProgressBar(widget);
  *opLabel                  = new QLabel(QWidget::tr("Select target folder"), bottomWidget);
  *opProgress               = new QProgressBar(widget);
  (*comboBox)               = new QComboBox(midWidget);
  (*comboBox)->addItems( QString(ops).split(" ",QString::SkipEmptyParts) );

  //topLayout->setSpacing(30);
  //topLayout->addSpacing(30);
  topLayout->addWidget(*fileLabel);
  topLayout->addSpacing(30);
  topLayout->addStretch();

  midLayout->addStretch();
  midLayout->addWidget(*comboBox);
  midLayout->addSpacing(30);
  midLayout->addWidget(executeButton);
  midLayout->addStretch();
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
  
  QWidget::connect(executeButton,  SIGNAL(clicked()), 
          parentCallback, executeOp);

  QWidget::connect(*comboBox, SIGNAL(activated(int)),
          parentCallback, refreshOp);

  lconnect(closeButton, SIGNAL(clicked()), []() {
    QApplication::quit();
  });

  return widget;
}

///////////////////////////////////////////////////////////////////////////

PlaylistWindow::PlaylistWindow() {

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
  mPlaylistViews->addWidget(
    createOpsWidget(this,mPlaylistViews,&mPlaylistOperatorComboBox,&mPlaylistFileLabel,&mPlaylistFileProgress,&mPlaylistOpLabel,&mPlaylistOpProgress,
    "New Move Delete Copy Merge Sort",SLOT(executePlaylistOp()),SLOT(refreshPlaylistOp(int))));
  mPlaylistViews->addWidget(
    createOpsWidget(this,mPlaylistViews,&mSongOperatorComboBox,&mFileLabel,&mFileProgress,&mOpLabel,&mOpProgress,
    "    Move Delete Copy           ",SLOT(executeSongOp()),    SLOT(refreshSongOp(int))));
  mPlaylistViews->addWidget(createSettingsWidget(mPlaylistViews));
  mPlaylistViews->setCurrentIndex(0);

  //connect(mViewButtonGroup, SIGNAL(buttonClicked(int)), mPlaylistViews, SLOT(setCurrentIndex(int)));
  lconnect(mViewButtonGroup, SIGNAL(buttonClicked(int)), [this]() {
    qDebug() << "Setting view: " << mViewButtonGroup->checkedId();
    mPlaylistViews->setCurrentIndex(mViewButtonGroup->checkedId());
  });

  topLayout->addWidget(selectViewButtons);
  topLayout->addWidget(mPlaylistViews);
  //topLayout->setSizeConstraint(QLayout::Size)

  // Bottom View
  auto* text = new QTextEdit(bottomWidget);
  text->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  bottomLayout->addWidget( text );

  // Full View
  layout->addWidget(topWidget);
  layout->addWidget(bottomWidget);
  //layout->setSizeConstraint(QLayout::SetFixedSize);

  setWindowTitle(tr("Playlist Utilities"));
}

void PlaylistWindow::executePlaylistOp() {
  qDebug() << "Executing Playlist Op: " << mPlaylistOperatorComboBox->currentText();
}

void PlaylistWindow::executeSongOp() {
  qDebug() << "Executing Song Op: " << mSongOperatorComboBox->currentText();
}

void PlaylistWindow::refreshPlaylistOp(int op) {
  qDebug() << "Refreshing Playlist Op: " << op;
}

void PlaylistWindow::refreshSongOp(int op) {
  qDebug() << "Refreshing Song Op: " << op;
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
