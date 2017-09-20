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

/*!@file uerr/err.h
 * @author uael
 */
#ifndef __UERR_ERR_H
# define __UERR_ERR_H

#include <uty.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "no.h"
#include "lvl.h"

typedef struct err err_t;
typedef struct err_stack err_stack_t;

struct err {
  errlvl_t lvl;
  i8_t const *fn, *file;
  u32_t line;
  errno_t code;
  i8_t msg[U8_MAX];
};

static FORCEINLINE err_t *
__err(err_t * self,
  errlvl_t lvl, i8_t const *fn, i8_t const *file, u32_t line, errno_t no) {
  *self = (err_t) {lvl, fn, file, line, no};
  strncpy(self->msg, strerror(no), U8_MAX);
  return self;
}

static FORCEINLINE err_t *
__err_usr(err_t * self,
  errlvl_t lvl, i8_t const *fn, i8_t const *file, u32_t line, i8_t const *msg) {
  *self = (err_t) {lvl, fn, file, line, ERRNO_USR};
  strncpy(self->msg, msg, U8_MAX);
  return self;
}

err_t __err_last;

#define err(LVL, CODE) \
  (*__err(&__err_last, LVL, __func__, __file__, __line__, CODE))

#define err_usr(LVL, MSG) \
  (*__err_usr(&__err_last, LVL, __func__, __file__, __line__, MSG))

struct err_stack {
  u16_t cap, len;
  err_t *buf;
};

__extern_c__
static FORCEINLINE void
err_stack_ctor(err_stack_t *__restrict__ self) {
  *self = (err_stack_t) {
    .cap = 0,
    .len = 0,
    .buf = nil
  };
}

__extern_c__
static FORCEINLINE void
err_stack_dtor(err_stack_t *__restrict__ self) {
  self->cap = 0;
  self->len = 0;
  if (self->buf) {
    free(self->buf);
    self->buf = nil;
  }
}

__extern_c__
static FORCEINLINE errno_t
err_stack_growth(err_stack_t *__restrict__ self, const u16_t nmin) {
  if (nmin > 0) {
    if (self->cap) {
      if (self->cap < nmin) {
        u16_t cap;
        err_t *buf;

        cap = self->cap;
        do cap = cap << 1; while (cap < nmin);
        if ((buf = realloc(self->buf, sizeof(err_t) * (size_t) cap)) == nil) {
          return (errno_t) errno;
        }
        self->cap = cap;
        self->buf = buf;
      }
    } else {
      self->cap = 4;
      do self->cap = self->cap << 1; while (self->cap < nmin);
      if ((self->buf = malloc(sizeof(err_t) * (size_t) self->cap)) == nil) {
        self->cap = 0;
        return (errno_t) errno;
      }
    }
  }
  return ERRNO_NOERR;
}

__extern_c__
static FORCEINLINE errno_t
err_stack_grow(err_stack_t *__restrict__ self, const u16_t nmem) {
  u16_t u;
  u = self->len + nmem;
  if (u < self->len) {
    u = U16_MAX;
  }
  return err_stack_growth(self, (const u16_t) u);
}

static FORCEINLINE errno_t
err_stack_push(err_stack_t *__restrict__ self, err_t item) {
  errno_t err;
  if ((err = err_stack_grow(self, 1)) > 0) {
    return err;
  }
  self->buf[self->len++] = item;
  return ERRNO_NOERR;
}

#if __has_builtin(__builtin_popcount)
#define __ISPOW2(n) (__builtin_popcount(n) == 1)
#else
#define __ISPOW2(n) (((n) != 0) && (((n) & (~(n) + 1)) == (n)))
#endif

__extern_c__
static FORCEINLINE errno_t
err_stack_pop(err_stack_t *__restrict__ self, err_t *__restrict__ out) {
  if (self->len == 0) { return ERRNO_USR; }
  --self->len;
  if (out != nil) { *out = self->buf[self->len]; }
  if (__ISPOW2(self->len) && self->len < self->cap) {
    u16_t cap;
    err_t *buf;

    cap = self->len;
    if ((buf = realloc(self->buf, sizeof(err_t) * (size_t) cap)) == nil) {
      return (errno_t) errno;
    }
    self->cap = cap;
    self->buf = buf;
  }
  return ERRNO_NOERR;
}

__extern_c__
static FORCEINLINE void
err_dump(err_t *__restrict__ self, FILE *__restrict stream) {
  i8_t const *lvl;

  switch (self->lvl) {
    case ERRLVL_NOTICE:
      lvl = "notice";
      break;
    case ERRLVL_WARNING:
      lvl = "warning";
      break;
    case ERRLVL_FATAL:
      lvl = "fatal";
      break;
    default:
      lvl = "error";
      break;
  }
  if (self->code > 0) {
    fprintf(stream, "%s (%d): '%s' in %s@%s:%d\n",
      lvl, self->code, self->msg, self->fn, self->file, self->line
    );
  } else {
    fprintf(stream, "%s: '%s' in %s@%s:%d\n",
      lvl, self->msg, self->fn, self->file, self->line
    );
  }
}

__extern_c__
static FORCEINLINE void
err_stack_dump(err_stack_t *__restrict__ self, FILE *__restrict stream) {
  err_t err;
  u16_t i;

  if (err_stack_pop(self, &err) == 0) {
    err_dump(&err, stream);
  }
  if (self->len) {
    fprintf(stream, "stack trace:\n");
    i = 0;
    while (err_stack_pop(self, &err) == 0) {
      fprintf(stream, "  %d. ", ++i);
      err_dump(&err, stream);
    }
  }
}

#endif /* !__UERR_ERR_H */
