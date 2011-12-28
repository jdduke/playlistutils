/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef PIMPL_IMPL_H
#define PIMPL_IMPL_H

#include "pimpl.h"

#include <utility>

namespace common {

template<typename T>
pimpl<T>::pimpl() : m( new T() ) { }

template<typename T>
template<typename Arg1>
pimpl<T>::pimpl( Arg1&& arg1 )
    : m( new T( std::forward<Arg1>(arg1) ) ) { }

template<typename T>
template<typename Arg1, typename Arg2>
pimpl<T>::pimpl( Arg1&& arg1, Arg2&& arg2 )
    : m( new T( std::forward<Arg1>(arg1), std::forward<Arg2>(arg2) ) ) { }

template<typename T>
template<typename Arg1, typename Arg2, typename Arg3>
pimpl<T>::pimpl( Arg1&& arg1, Arg2&& arg2, Arg3&& arg3 )
    : m( new T( std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3) ) ) { }

template<typename T>
template<typename Arg1, typename Arg2, typename Arg3, typename Arg4>
pimpl<T>::pimpl( Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4  )
  : m( new T( std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3),  std::forward<Arg4>(arg4) ) ) { }

template<typename T>
pimpl<T>::~pimpl() { }

template<typename T>
T* pimpl<T>::operator->() { return m.get(); }

template<typename T>
const T* pimpl<T>::operator->() const { return m.get(); }

template<typename T>
T& pimpl<T>::operator*() { return *m.get(); }

template<typename T>
const T& pimpl<T>::operator*() const { return *m.get(); }

}

#endif
