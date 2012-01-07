/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#include "playlist_widgets.h"

#include <PlaylistUtilities.h>

#include <QFileInfo>
#include <QPainter>

///////////////////////////////////////////////////////////////////////////

PlaylistModel::PlaylistModel(QObject *parent)
  : QAbstractTableModel(parent), mPlaylist(nullptr) {
}

PlaylistModel::PlaylistModel(pu::Playlist* playlist, QObject *parent)
  : QAbstractTableModel(parent) {
  setPlaylist(playlist);
}

void PlaylistModel::setPlaylist(pu::Playlist* playlist) {
  if (playlist != mPlaylist) {
    emit layoutAboutToBeChanged();
    mPlaylist     = playlist;
    mStatusString = QVector<QString>(rowCount());
    mStatus       = QVector<Status>(rowCount(), Status_Empty);
    emit layoutChanged();
  }
}

int PlaylistModel::rowCount(const QModelIndex&) const {
  return rowCount();
}

int PlaylistModel::rowCount() const {
  return mPlaylist ? (int)mPlaylist->songCount() : 0;
}

int PlaylistModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return 4;
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  if (index.row() >= rowCount() || index.row() < 0)
    return QVariant();

  if (role == Qt::DisplayRole) {
    if (index.column() == 0)
      return (int)mStatus.at(index.row());
    else if (index.column() == 1)
      return mPlaylist ? mPlaylist->song(index.row()).path() : "";
    else if (index.column() == 2)
      return mPlaylist ? QFileInfo(mPlaylist->song(index.row()).path()).size() : 0;
    else if (index.column() == 3)
      return mStatusString.at(index.row());
  }
  return QVariant();
}

QVariant PlaylistModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) {
    switch (section) {
    case 0:
      return tr("");
    case 1:
      return tr("Source File");
    case 2:
      return tr("Size");
    case 3:
      return tr("Status");

    default:
      return QVariant();
    }
  }
  return QVariant();
}

bool PlaylistModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  if (index.isValid() && role == Qt::EditRole) {
    int row = index.row();

    if (index.column() == 0)
      mStatus[row] = static_cast<Status>(value.toUInt());
    else if (index.column() == 3)
      mStatusString[row] = value.toString();
    else
      return false;

    emit(dataChanged(index, index));
    return true;
  }

  return false;
}

Qt::ItemFlags PlaylistModel::flags(const QModelIndex &index) const {
  if (!index.isValid())
    return Qt::ItemIsEnabled;

  return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

///////////////////////////////////////////////////////////////////////////

ImageDelegate::ImageDelegate(const QStringList& imageList, QObject * parent)
  : QAbstractItemDelegate(parent) {
  for (int i = 0; i < imageList.size(); ++i) {
    mImages.push_back(QPixmap(imageList.at(i)));
  }
}

QSize ImageDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const {
  return QSize(32,32);
}

void ImageDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const {
  int i = index.data(Qt::DisplayRole).toInt();
  if (0 <= i && i < mImages.size() && !mImages[i].isNull())
    painter->drawPixmap(option.rect, mImages[i]);
}