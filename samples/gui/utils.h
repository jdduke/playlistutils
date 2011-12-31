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

class connect_functor : public QObject {
  Q_OBJECT
public:
  connect_functor(QObject *parent) 
    : QObject(parent) { }

public Q_SLOTS:
  void signaled() {
    fImpl();
  }

protected:
  virtual void fImpl() = 0;
};

template <class T, class U = void>
class connect_functor_helper : public connect_functor {
public:
  connect_functor_helper(QObject* parent, T&& f_)
    : connect_functor(parent), f(std::move(f_)) { }
  connect_functor_helper(QObject* parent, const T& f_)
    : connect_functor(parent), f(f_) { }

  template <class U> 
  typename std::enable_if< !std::is_same<U,void>::value, void>::type fImpl(const U& arg) { f(arg); }
  typename std::enable_if<  std::is_same<U,void>::value      >::type fImpl() { f(); }
private:
  T f;
};

template <class T>
inline bool lconnect(QObject* sender, const char* signal, const T& reciever, Qt::ConnectionType type = Qt::AutoConnection) {
  return QObject::connect(sender, signal, new connect_functor_helper<T>(sender, reciever), SLOT(signaled()), type);
}

template <class T>
inline bool lconnect(QObject* sender, const char* signal, T&& reciever, Qt::ConnectionType type = Qt::AutoConnection) {
  return QObject::connect(sender, signal, new connect_functor_helper<T>(sender, reciever), SLOT(signaled()), type);
}

#endif