#ifndef STRBUF_H
#define STRBUF_H

#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define bitsizeof(x)  (CHAR_BIT * sizeof(x))

#define maximum_unsigned_value_of_type(a) \
    (UINTMAX_MAX >> (bitsizeof(uintmax_t) - bitsizeof(a)))

#define unsigned_add_overflows(a, b) \
    ((b) > maximum_unsigned_value_of_type(a) - (a))

#define unsigned_mult_overflows(a, b) \
    ((a) && (b) > maximum_unsigned_value_of_type(a) / (a))

#define NORETURN __attribute__((__noreturn__))

struct strbuf;

/* General helper functions */
NORETURN void die(const char *err, ...) __attribute__((format (printf, 1, 2)));

void *xrealloc(void *ptr, size_t size);

static inline size_t st_mult(size_t a, size_t b)
{
  if (unsigned_mult_overflows(a, b))
	die("size_t overflow: %"PRIuMAX" * %"PRIuMAX,
		(uintmax_t)a, (uintmax_t)b);
  return a * b;
}

struct strbuf {
	size_t alloc;
	size_t len;
	char *buf;
};

extern char strbuf_slopbuf[];

void strbuf_init(struct strbuf *sb, size_t alloc);

void strbuf_release(struct strbuf *sb);

static inline size_t strbuf_avail(const struct strbuf *sb)
{
	return sb->alloc ? sb->alloc - sb->len - 1 : 0;
}

void strbuf_grow(struct strbuf *sb, size_t amount);

static inline void strbuf_setlen(struct strbuf *sb, size_t len)
{
	if (len > (sb->alloc ? sb->alloc - 1 : 0))
		die("BUG: strbuf_setlen() beyond buffer");
	sb->len = len;
	if (sb->buf != strbuf_slopbuf)
		sb->buf[len] = '\0';
	else
		assert(!strbuf_slopbuf[0]);
}

static inline void strbuf_addch(struct strbuf *sb, int c)
{
	if (!strbuf_avail(sb))
		strbuf_grow(sb, 1);
	sb->buf[sb->len++] = c;
	sb->buf[sb->len] = '\0';
}

void strbuf_remove(struct strbuf *sb, size_t pos, size_t len);

void strbuf_splice(struct strbuf *sb, size_t pos, size_t len,
                   const void *data, size_t data_len);

ssize_t strbuf_write(struct strbuf *sb, FILE *stream);

#endif /* STRBUF_H */
