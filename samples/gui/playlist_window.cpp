/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#include "playlist_window.h"
#include "playlist_utils.h"
#include "utils.h"

#include <QtGui>

/////////////////////////////////////////////////////////////////////////////

PlaylistWindow::PlaylistWindow() {
  
  ///////////////////////////////////////////////////////////////////////////
  // Playlist Ops
  mOperatorComboBox = new QComboBox;
  connect(mOperatorComboBox, SIGNAL(activated(int)),
          this,              SLOT(refreshOp()));

  ///////////////////////////////////////////////////////////////////////////
  // Playlist Song Ops

  ///////////////////////////////////////////////////////////////////////////
  // Playlist Settings

  ///////////////////////////////////////////////////////////////////////////
  // Playlist About

  QGridLayout *mainLayout = new QGridLayout;
  setLayout(mainLayout);	

  lconnect(this, SIGNAL(testLambda()), [this](){
    qDebug() << "This is only a test" << this->windowTitle();
  });

  emit testLambda();

  /*
  mainLayout->addWidget(a.mButton, 0, 0, 2, 1);
  mainLayout->addWidget(a.mComboBox, 2, 0, 1, 1);
  mainLayout->addWidget(mOperatorComboBox, 1, 1);
  mainLayout->addWidget(b.mButton, 0, 2, 2, 1);
  mainLayout->addWidget(b.mComboBox, 2, 2, 1, 1);
  mainLayout->addWidget(equalLabel, 1, 3);
  mainLayout->addWidget(result.mButton, 0, 4, 2, 1);
  mainLayout->addWidget(result.mComboBox, 2, 4, 1, 1);
  mainLayout->setSizeConstraint(QLayout::SetFixedSize);

  QStackedWidget *TalkableViews;
  QButtonGroup *ViewButtonGroup;
  QPushButton *BuddiesViewButton;
  QPushButton *ChatsViewButton;

  QVBoxLayout *layout = new QVBoxLayout(this);
  QWidget *selectViewButtons = new QWidget(this);
  QHBoxLayout *selectViewButtonsLayout = new QHBoxLayout(selectViewButtons);
  ViewButtonGroup = new QButtonGroup(this);

  BuddiesViewButton = new QPushButton(tr("Buddies"), this);
  BuddiesViewButton->setCheckable(true);
  ChatsViewButton = new QPushButton(tr("Chats"), this);
  ChatsViewButton->setCheckable(true);
  ViewButtonGroup->addButton(BuddiesViewButton);
  ViewButtonGroup->addButton(ChatsViewButton);
  BuddiesViewButton->setChecked(true);
  connect(ViewButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(viewButtonClicked()));
  selectViewButtonsLayout->addWidget(BuddiesViewButton);
  selectViewButtonsLayout->addWidget(ChatsViewButton);
  selectViewButtonsLayout->addStretch(1);
  TalkableViews = new QStackedWidget(this);
  TalkableViews->addWidget(createBuddiesWidget(TalkableViews));
  TalkableViews->addWidget(createChatsWidget(TalkableViews));
  layout->addWidget(selectViewButtons);
  layout->addWidget(TalkableViews);
  viewButtonClicked();
  */

  setWindowTitle(tr("Playlist Utilities"));
}

void PlaylistWindow::executeOp() {
  emit testLambda();
}

void PlaylistWindow::refreshOps( ) {
  emit testLambda();
}

PlaylistWindow::PlaylistOp PlaylistWindow::currentOp() const {
  return PlaylistOp_None;
}

PlaylistWindow::~PlaylistWindow() {
  emit testLambda();
}
