/*
 * MIT License
 *
 * Copyright (c) 2016-2017 Abel Lucas <www.github.com/uael>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*!@file uerr/no.h
 * @author uael
 */
#ifndef __UERR_NO_H
# define __UERR_NO_H

#include <errno.h>

enum errno_wrap {
  ERRNO_PERM = EPERM,
  ERRNO_NOENT = ENOENT,
  ERRNO_SRCH = ESRCH,
  ERRNO_INTR = EINTR,
  ERRNO_IO = EIO,
  ERRNO_NXIO = ENXIO,
  ERRNO_2BIG = E2BIG,
  ERRNO_NOEXEC = ENOEXEC,
  ERRNO_BADF = EBADF,
  ERRNO_CHILD = ECHILD,
  ERRNO_AGAIN = EAGAIN,
  ERRNO_NOMEM = ENOMEM,
  ERRNO_ACCES = EACCES,
  ERRNO_FAULT = EFAULT,
  ERRNO_BUSY = EBUSY,
  ERRNO_EXIST = EEXIST,
  ERRNO_XDEV = EXDEV,
  ERRNO_NODEV = ENODEV,
  ERRNO_NOTDIR = ENOTDIR,
  ERRNO_ISDIR = EISDIR,
  ERRNO_INVAL = EINVAL,
  ERRNO_NFILE = ENFILE,
  ERRNO_MFILE = EMFILE,
  ERRNO_NOTTY = ENOTTY,
  ERRNO_FBIG = EFBIG,
  ERRNO_NOSPC = ENOSPC,
  ERRNO_SPIPE = ESPIPE,
  ERRNO_ROFS = EROFS,
  ERRNO_MLINK = EMLINK,
  ERRNO_PIPE = EPIPE,
  ERRNO_DOM = EDOM,
  ERRNO_RANGE = ERANGE,
  ERRNO_DEADLK = EDEADLK,
  ERRNO_DEADLOCK = EDEADLOCK,
  ERRNO_NAMETOOLONG = ENAMETOOLONG,
  ERRNO_NOLCK = ENOLCK,
  ERRNO_NOSYS = ENOSYS,
  ERRNO_NOTEMPTY = ENOTEMPTY,
  ERRNO_ILSEQ = EILSEQ,
  ERRNO_USR = ERRNO_ILSEQ + 1
};

typedef enum errno_wrap errno_t;

#endif /* !__UERR_NO_H */
