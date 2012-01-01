/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef PLAYLIST_WIDGETS_H
#define PLAYLIST_WIDGETS_H

#include <QAbstractItemDelegate>
#include <QAbstractTableModel>

class PlaylistModel : public QAbstractTableModel {
  Q_OBJECT

public:
  PlaylistModel(QObject *parent=0);
  PlaylistModel(QList< QPair<QString, QString> > listofPairs, QObject *parent=0);

  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
  bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
  bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());
  QList< QPair<QString, QString> > getList();

private:
  QList< QPair<QString, QString> > listOfPairs;
};

///////////////////////////////////////////////////////////////////////////

class ImageDelegate : public QAbstractItemDelegate {
public:
  ImageDelegate(QObject * parent = 0);
  void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
  QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;
};

#endif
