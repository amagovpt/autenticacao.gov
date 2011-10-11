/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */

#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H

#ifdef _WIN32
#include <windows.h>
#else
#include <wintypes.h>
#include <stddef.h>
#endif

namespace eidmw { namespace pcscproxy {
				  template <class T>
				  class ArrayBuffer {
public:
				  ArrayBuffer();
				  ArrayBuffer(const T* _buf, size_t _len);
				  ~ArrayBuffer();
				  void clear();
				  void put(const T* _buf, size_t _len);
				  size_t size() const;
				  const T* const get() const;
private:
				  ArrayBuffer(const ArrayBuffer&);            //do not implement
				  ArrayBuffer& operator=(const ArrayBuffer&); //do not implement

				  T      * buf;
				  size_t len;
				  };

				  template <class T>
				  ArrayBuffer<T>::ArrayBuffer() : buf(NULL), len(0)
				  {
				  }

				  template <class T>
				  ArrayBuffer<T>::ArrayBuffer(const T* _buf, size_t _len) : buf(NULL), len(0)
				  {
					  put(buf, len);
				  }

				  template <class T>
				  ArrayBuffer<T>::~ArrayBuffer()
				  {
					  if (buf)
						  delete[] buf;
				  }

				  template <class T>
				  void ArrayBuffer<T>::put(const T* _buf, size_t _len)
				  {
					  T *tmpBuf = buf;
					  buf = new T[_len];
					  len = _len;
					  memcpy(buf, _buf, len);
					  if (tmpBuf)
					  {
						  delete[] tmpBuf;
					  }
				  }

				  template <class T>
				  void ArrayBuffer<T>::clear()
				  {
					  if (buf)
					  {
						  delete[] buf;
						  buf = NULL;
					  }
					  len = 0;
				  }

				  template <class T>
				  size_t ArrayBuffer<T>::size() const
				  {
					  return len;
				  }

				  template <class T>
				  const T* const ArrayBuffer<T>::get() const
				  {
					  return buf;
				  }
				  } //pcscproxy
}                   //eidmw

#endif
