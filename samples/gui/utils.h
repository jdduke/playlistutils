/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef GUI_UTILS_H
#define GUI_UTILS_H

#include <QObject>
#include <QDebug>

#include <functional>

class connect_functor_helper : public QObject {
  Q_OBJECT
public:
  connect_functor_helper(QObject *parent, const std::function<void()>& f_) 
    : QObject(parent), f(f_) { }
  ~connect_functor_helper() {
    qDebug() << "Lambda destroyed.\n";
  }

public Q_SLOTS:
  void signaled() {
    f();
  }

private:
  std::function<void()> f;
};

template <class T>
inline bool lconnect(QObject *sender, const char *signal, const T &reciever, Qt::ConnectionType type = Qt::AutoConnection) {
  return QObject::connect(sender, signal, new connect_functor_helper(sender, reciever), SLOT(signaled()), type);
}

#endif