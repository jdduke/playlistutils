#ifndef PIMPL_H
#define PIMPL_H

#include "common.h"

#include <memory>

namespace common {

template <typename T>
class pimpl {
public:
  pimpl();
  template<typename Arg1>
  pimpl( Arg1&& arg1 );
  template<typename Arg1, typename Arg2>
  pimpl( Arg1&& arg1, Arg2&& arg2 );
  template<typename Arg1, typename Arg2, typename Arg3>
  pimpl( Arg1&& arg1, Arg2&& arg2, Arg3&& arg3 );
  template<typename Arg1, typename Arg2, typename Arg3, typename Arg4>
  pimpl( Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4 );

  ~pimpl();

  T* operator->();
  const T* operator->() const;
  T& operator*();
  const T& operator*() const;

private:
  std::unique_ptr<T> m;
};

}

#endif
