/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef PLAYLIST_WIDGETS_H
#define PLAYLIST_WIDGETS_H

#include <QAbstractItemDelegate>
#include <QAbstractTableModel>

#include <PlaylistForward.h>

class PlaylistModel : public QAbstractTableModel {
  Q_OBJECT

public:

  enum Status {
    Status_Empty = 0,
    Status_Success,
    Status_Failure,
    Status_Count
  };

  enum Column {
    Column_Image = 0,
    Column_Source,
    Column_Size,
    Column_Status,
    Column_Count
  };

  PlaylistModel(QObject *parent = nullptr);
  PlaylistModel(pu::Playlist* playlist, QObject *parent = nullptr);

  void setPlaylist(pu::Playlist* playlist);
  void addSong(const char*);

  int rowCount() const;
  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);

signals:
  void rowModified(QVariant);

private:

  QVector< QString > mStatusString;
  QVector< Status >  mStatus;
  pu::Playlist*    mPlaylist;
};

///////////////////////////////////////////////////////////////////////////

class ImageDelegate : public QAbstractItemDelegate {
public:
  ImageDelegate(const QStringList& imageList, QObject * parent = 0);
  void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
  QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;
private:
  QVector<QPixmap> mImages;
};

#endif
