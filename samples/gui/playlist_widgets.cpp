/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#include "playlist_widgets.h"

#include <QPainter>

///////////////////////////////////////////////////////////////////////////

PlaylistModel::PlaylistModel(QObject *parent)
  : QAbstractTableModel(parent) {
}

PlaylistModel::PlaylistModel(QList< QPair<QString, QString> > pairs, QObject *parent)
  : QAbstractTableModel(parent) {
    listOfPairs=pairs;
}

int PlaylistModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return listOfPairs.size();
}

int PlaylistModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return 2;
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  if (index.row() >= listOfPairs.size() || index.row() < 0)
    return QVariant();

  if (role == Qt::DisplayRole) {
    QPair<QString, QString> pair = listOfPairs.at(index.row());

    if (index.column() == 0)
      return pair.first;
    else if (index.column() == 1)
      return pair.second;
  }
  return QVariant();
}

QVariant PlaylistModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) {
    switch (section) {
    case 0:
      return tr("Name");

    case 1:
      return tr("Address");

    default:
      return QVariant();
    }
  }
  return QVariant();
}

bool PlaylistModel::insertRows(int position, int rows, const QModelIndex &index) {
  Q_UNUSED(index);
  beginInsertRows(QModelIndex(), position, position+rows-1);

  for (int row=0; row < rows; row++) {
    QPair<QString, QString> pair(" ", " ");
    listOfPairs.insert(position, pair);
  }

  endInsertRows();
  return true;
}

bool PlaylistModel::removeRows(int position, int rows, const QModelIndex &index) {
  Q_UNUSED(index);
  beginRemoveRows(QModelIndex(), position, position+rows-1);

  for (int row=0; row < rows; ++row) {
    listOfPairs.removeAt(position);
  }

  endRemoveRows();
  return true;
}

bool PlaylistModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  if (index.isValid() && role == Qt::EditRole) {
    int row = index.row();

    QPair<QString, QString> p = listOfPairs.value(row);

    if (index.column() == 0)
      p.first = value.toString();
    else if (index.column() == 1)
      p.second = value.toString();
    else
      return false;

    listOfPairs.replace(row, p);
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

QList< QPair<QString, QString> > PlaylistModel::getList() {
  return listOfPairs;
}

///////////////////////////////////////////////////////////////////////////

ImageDelegate::ImageDelegate(QObject * parent)
  : QAbstractItemDelegate(parent) {
}

QSize ImageDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const {
  return QSize(32,32);
}

void ImageDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const {
  int data = index.data(Qt::DisplayRole).toInt();

  switch (data) {
  case 0:
    {
      QPixmap pm(":/img/red");
      painter->drawPixmap(option.rect, pm);
      return;
    }
  case 1:
    {
      QPixmap pm(":/img/green");
      painter->drawPixmap(option.rect, pm);
      return;
    }
  case 2:
    {
      QPixmap pm(":/img/blue");
      painter->drawPixmap(option.rect, pm);
      return;
    }
  };
}