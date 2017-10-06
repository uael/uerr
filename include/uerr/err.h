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

#include "no.h"
#include "lvl.h"
#include "ret.h"

typedef struct err err_t;
typedef struct err_stack err_stack_t;

struct err {
  errlvl_t lvl;
  char_t __const *fn, *file;
  u32_t line;
  errno_t code;
  char_t msg[U8_MAX];
  i32_t col;
};

__extern_c__
static FORCEINLINE UNUSED err_t *
__err(err_t * self,
  errlvl_t lvl, char_t __const *fn, char_t __const *file, u32_t line, errno_t no) {
  self->lvl = lvl;
  self->fn = fn;
  self->file = file;
  self->line = line;
  self->code = no;
  strncpy(self->msg, strerror(no), (size_t) U8_MAX);
  return self;
}

__extern_c__
static UNUSED err_t *
__err_usr(err_t * self,
  errlvl_t lvl, char_t __const *fn, char_t __const *file, u32_t line,
  char_t __const *msg, ...) {
  va_list args;

  self->lvl = lvl;
  self->fn = fn;
  self->file = file;
  self->line = line;
  self->code = ERRNO_USR;
  va_start(args, msg);
  vsprintf(self->msg, msg, args);
  va_end(args);
  return self;
}

err_t __err_last;

#define err(LVL, CODE) \
  (*__err(&__err_last, LVL, __pretty_func__, __file__, __line__, CODE))

#define usrerr(LVL, MSG) \
  (*__err_usr(&__err_last, LVL, __pretty_func__, __file__, __line__, MSG))

#define usrerrf(LVL, MSG, ...) \
  (*__err_usr(&__err_last, LVL, __pretty_func__, __file__, __line__, MSG, __VA_ARGS__))

#define syserr() \
  (*__err(&__err_last, ERRLVL_ERROR, __pretty_func__, __file__, __line__, errno))

#define warning(MSG) \
  (*__err_usr(&__err_last, ERRLVL_WARNING, __pretty_func__, __file__, __line__, MSG))

#define warningf(MSG, ...) \
  (*__err_usr(& \
    __err_last, ERRLVL_WARNING, __pretty_func__, __file__, __line__, MSG, __VA_ARGS__ \
  ))

#define notice(MSG) \
  (*__err_usr(&__err_last, ERRLVL_NOTICE, __pretty_func__, __file__, __line__, MSG))

#define noticef(MSG, ...) \
  (*__err_usr(& \
    __err_last, ERRLVL_NOTICE, __pretty_func__, __file__, __line__, MSG, __VA_ARGS__ \
  ))

#define error(MSG) \
  (*__err_usr(&__err_last, ERRLVL_ERROR, __pretty_func__, __file__, __line__, MSG))

#define errorf(MSG, ...) \
  (*__err_usr(& \
    __err_last, ERRLVL_ERROR, __pretty_func__, __file__, __line__, MSG, __VA_ARGS__ \
  ))

#define fatal(MSG) \
  (*__err_usr(&__err_last, ERRLVL_ERROR, __pretty_func__, __file__, __line__, MSG))

#define fatalf(MSG, ...) \
  (*__err_usr(& \
    __err_last, ERRLVL_ERROR, __pretty_func__, __file__, __line__, MSG, __VA_ARGS__ \
  ))

struct err_stack {
  u16_t cap, len;
  err_t *buf;
};

__extern_c__
static FORCEINLINE void
err_stack_ctor(err_stack_t *__restrict self) {
  *self = (err_stack_t) {
    .cap = 0,
    .len = 0,
    .buf = nil
  };
}

__extern_c__
static FORCEINLINE void
err_stack_dtor(err_stack_t *__restrict self) {
  self->cap = 0;
  self->len = 0;
  if (self->buf) {
    free(self->buf);
    self->buf = nil;
  }
}

__extern_c__
static FORCEINLINE ret_t
err_stack_growth(err_stack_t *__restrict self, __const u16_t nmin) {
  if (nmin > 0) {
    if (self->cap) {
      if (self->cap < nmin) {
        u16_t cap;
        err_t *buf;

        cap = self->cap;
        do cap = cap << 1; while (cap < nmin);
        if ((buf = realloc(self->buf, sizeof(err_t) * (size_t) cap)) == nil) {
          return RET_ERRNO;
        }
        self->cap = cap;
        self->buf = buf;
      }
    } else {
      self->cap = 4;
      do self->cap = self->cap << 1; while (self->cap < nmin);
      if ((self->buf = malloc(sizeof(err_t) * (size_t) self->cap)) == nil) {
        self->cap = 0;
        return RET_ERRNO;
      }
    }
  }
  return RET_SUCCESS;
}

__extern_c__
static FORCEINLINE ret_t
err_stack_grow(err_stack_t *__restrict self, __const u16_t nmem) {
  u16_t u;

  u = self->len + nmem;
  if (u < self->len) {
    u = U16_MAX;
  }
  return err_stack_growth(self, (__const u16_t) u);
}

static FORCEINLINE ret_t
err_stack_push(err_stack_t *__restrict self, err_t item) {
  ret_t ret;

  if ((ret = err_stack_grow(self, 1)) > 0) {
    return ret;
  }
  self->buf[self->len++] = item;
  return RET_SUCCESS;
}

#if __has_builtin__(popcount)
#define __ISPOW2(n) (__builtin_popcount(n) == 1)
#else
#define __ISPOW2(n) (((n) != 0) && (((n) & (~(n) + 1)) == (n)))
#endif

__extern_c__
static FORCEINLINE ret_t
err_stack_pop(err_stack_t *__restrict self, err_t *__restrict out) {
  if (self->len == 0) {
    return RET_FAILURE;
  }
  if (out != nil) {
      *out = self->buf[0];
    }
    if (self->len == 1) {
      --self->len;
    } else {
      memmove(
        self->buf,
        self->buf + 1,
        (size_t) --self->len * sizeof(err_t)
      );
    }
  if (__ISPOW2(self->len) && self->len < self->cap) {
    u16_t cap;
    err_t *buf;

    cap = self->len;
    if ((buf = realloc(self->buf, sizeof(err_t) * (size_t) cap)) == nil) {
      return RET_ERRNO;
    }
    self->cap = cap;
    self->buf = buf;
  }
  return RET_SUCCESS;
}

__extern_c__
static FORCEINLINE ret_t
err_stack_merge(err_stack_t *__restrict self, err_stack_t *__restrict x) {
  ret_t ret;

  if (x->len > 0) {
    if ((ret = err_stack_grow(self, self->len)) > 0) {
      return ret;
    }
    memcpy(self->buf + self->len, self->buf, (size_t) x->len * sizeof(err_t));
    self->len += x->len;
    err_stack_dtor(x);
  }
  return RET_SUCCESS;
}

#ifndef CC_MSVC
# define _COLOR_RESET   "\033[0m"
# define _COLOR_RED     "\033[31m"
# define _COLOR_YELLOW  "\033[33m"
# define _COLOR_CYAN    "\033[36m"
# define _COLOR_BOLD    "\033[1m"
#else
# define _COLOR_RESET ""
# define _COLOR_RED ""
# define _COLOR_YELLOW ""
# define _COLOR_CYAN ""
# define _COLOR_BOLD ""
#endif

__extern_c__
static FORCEINLINE void
err_dump(err_t *__restrict self, FILE *__restrict stream) {
  char_t __const *lvl, *lvl_color;
  FILE *file;

  switch (self->lvl) {
    case ERRLVL_NOTICE:
      lvl = "notice";
      lvl_color = _COLOR_CYAN;
      break;
    case ERRLVL_WARNING:
      lvl = "warning";
      lvl_color = _COLOR_YELLOW;
      break;
    case ERRLVL_FATAL:
      lvl = "fatal";
      lvl_color = _COLOR_RED;
      break;
    default:
      lvl = "error";
      lvl_color = _COLOR_RED;
      break;
  }
  if (self->fn) {
    fprintf(stream,
      _COLOR_BOLD "%s:" _COLOR_RESET " In function '" _COLOR_BOLD
        "%s" _COLOR_RESET "':\n",
      self->file, self->fn
    );
  } else {
    fprintf(stream,
      _COLOR_BOLD "%s:" _COLOR_RESET " In function:\n",
      self->file
    );
  }
  if (self->code > 0 && self->code != ERRNO_USR) {
    fprintf(stream,
      _COLOR_BOLD "%s:%d:" _COLOR_RESET " %s" _COLOR_BOLD "%s (%d):"
        _COLOR_RESET " %s%s\n" _COLOR_RESET,
      self->file, self->line, lvl_color, lvl, self->code, lvl_color, self->msg
    );
  } else {
    fprintf(stream,
      _COLOR_BOLD "%s:%d:" _COLOR_RESET " %s" _COLOR_BOLD "%s:" _COLOR_RESET
        " %s%s\n" _COLOR_RESET,
      self->file, self->line, lvl_color, lvl, lvl_color, self->msg
    );
  }
  if ((file = fopen(self->file, "r")) != nil) {
    char_t buf[4096], *begin, *end;
    u64_t size;
    u16_t i;
    i16_t j;

    i = 0;
    j = (i16_t) (self->line - 2);
    if (j < 0) j = 0;
    begin = end = nil;
    while ((size = fread(buf, 1, 4096, file)) > 0) {
      end = buf;
      loop:
      while (size && *end && *end != '\n') {
        ++end;
        if (--size == 0) continue;
      }
      if (*end == '\n') {
        if (++i == self->line) break;
        ++end;
        if (i < j) begin = end;
        if (--size) goto loop;
      }
    }
    if (begin && end) {
      fprintf(stream, "%.*s\n", (int) (end - begin), begin);
      if (self->col) {
        while (--self->col) putc(' ', stream);
      } else {
        while (*begin && *(begin++) <= ' ') putc(' ', stream);
      }
      fputs("^\n", stream);
    }
    fclose(file);
  }
}

__extern_c__
static FORCEINLINE ret_t
err_stack_dump(err_stack_t *__restrict self, FILE *__restrict stream) {
  ret_t ret;
  err_t err;

  while ((ret = err_stack_pop(self, &err)) == RET_SUCCESS) {
    err_dump(&err, stream);
  }
  if (ret == RET_ERRNO) {
    return RET_ERRNO;
  }
  return RET_SUCCESS;
}

#endif /* !__UERR_ERR_H */
