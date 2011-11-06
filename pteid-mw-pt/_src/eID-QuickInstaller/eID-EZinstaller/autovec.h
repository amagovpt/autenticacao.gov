// This code is based on bits/std_memory.h from GCC.
// The copyright and license information from the
// original code is below.

// Copyright (C) 2001 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU General Public License.

/*
 * Copyright (c) 1997-1999
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 */

#ifndef _util_misc_autovec_h
#define _util_misc_autovec_h

#include <stddef.h>

namespace sc {

template <class T>
class auto_vec {
    T* d_;
  public:
    typedef T element_type;

    explicit auto_vec(T*d = 0) throw(): d_(d) {}

    auto_vec(auto_vec &av) throw(): d_(av.release()) {}

    ~auto_vec() throw() { delete[] d_; }

    auto_vec &operator = (auto_vec &av) throw() {
      reset(av.release());
      return *this;
    }

    T* get() const throw() { return d_; }

    T &operator[](size_t i) throw() { return d_[i]; }

    T* release() throw() {
      T *r = d_;
      d_ = 0;
      return r;
    }

    void reset(T*d=0) throw() {
      if (d != d_) {
          delete[] d_;
          d_ = d;
        }
    }
      
};

}

#endif // _util_misc_autovec_h

