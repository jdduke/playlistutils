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
  inline void signaled() {
    fImpl();
  }
  inline void signaled(QVariant arg) {
    fImpl(arg);
  }

protected:
  virtual void fImpl() = 0;
  virtual void fImpl(QVariant) = 0;
};

template <class T = void>
class connect_functor_helper : public connect_functor {
public:
  typedef std::function<void(T)> F;
  connect_functor_helper(QObject* parent, F&& f_)
    : connect_functor(parent), f(std::move(f_)) { }
  connect_functor_helper(QObject* parent, const F& f_)
    : connect_functor(parent), f(f_) { }

  template <class R> 
  typename std::enable_if< !std::is_same<R,void>::value, void>::type fImplHelper(QVariant arg) { f(qVariantValue<T>(arg)); }
  template <class R> 
  typename std::enable_if< !std::is_same<R,void>::value, void>::type fImplHelper() { }
  template <class R> 
  typename std::enable_if<  std::is_same<R,void>::value      >::type fImplHelper(QVariant) { f(); }
  template <class R> 
  typename std::enable_if<  std::is_same<R,void>::value      >::type fImplHelper() { f(); }

  void fImpl(QVariant arg) { fImplHelper<T>(arg); }
  void fImpl()             { fImplHelper<T>(); }

private:
  F f;
};

template <class T>
inline bool lconnect(QObject* sender, const char* signal, const std::function<void(T)>& reciever, Qt::ConnectionType type = Qt::AutoConnection) {
  return QObject::connect(sender, signal, new connect_functor_helper<T>(sender, reciever), SLOT(signaled(QVariant)), type);
}

template <class T>
inline bool lconnect(QObject* sender, const char* signal, std::function<void(T)>&& reciever, Qt::ConnectionType type = Qt::AutoConnection) {
  return QObject::connect(sender, signal, new connect_functor_helper<T>(sender, reciever), SLOT(signaled(QVariant)), type);
}

inline bool lconnect(QObject* sender, const char* signal, const std::function<void(void)>& reciever, Qt::ConnectionType type = Qt::AutoConnection) {
  return QObject::connect(sender, signal, new connect_functor_helper<>(sender, reciever), SLOT(signaled()), type);
}

inline bool lconnect(QObject* sender, const char* signal, std::function<void(void)>&& reciever, Qt::ConnectionType type = Qt::AutoConnection) {
  return QObject::connect(sender, signal, new connect_functor_helper<>(sender, reciever), SLOT(signaled()), type);
}

#endif