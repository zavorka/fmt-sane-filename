#include <string.h>

#include "strbuf.h"

#define alloc_nr(x) (((x)+16)*3/2)

#define REALLOC_ARRAY(x, alloc) (x) = xrealloc((x), st_mult(sizeof(*(x)), (alloc)))

#define ALLOC_GROW(x, nr, alloc) \
       	do { \
               	if ((nr) > (alloc)) { \
                       	if (alloc_nr(alloc) < (nr)) \
                               	(alloc) = (nr); \
                       	else \
                               	(alloc) = alloc_nr(alloc); \
                       	REALLOC_ARRAY(x, alloc); \
               	} \
       	} while (0)


char strbuf_slopbuf[1];

void strbuf_init(struct strbuf *sb, size_t hint)
{
	sb->alloc = sb->len = 0;
	sb->buf = strbuf_slopbuf;
	if (hint)
		strbuf_grow(sb, hint);
}

void strbuf_release(struct strbuf *sb)
{
	if (sb->alloc) {
		free(sb->buf);
		strbuf_init(sb, 0);
	}
}

void strbuf_grow(struct strbuf *sb, size_t extra)
{
	int new_buf = !sb->alloc;
	if (unsigned_add_overflows(extra, 1) ||
	    unsigned_add_overflows(sb->len, extra + 1))
		die("you want to use way too much memory");
	if (new_buf)
		sb->buf = NULL;
	ALLOC_GROW(sb->buf, sb->len + extra + 1, sb->alloc);
	if (new_buf)
		sb->buf[0] = '\0';
}

void strbuf_splice(struct strbuf *sb, size_t pos, size_t len,
                   const void *data, size_t dlen)
{
  if (unsigned_add_overflows(pos, len))
    die("you want to use way too much memory");
  if (pos > sb->len)
    die("`pos' is too far after the end of the buffer");
  if (pos + len > sb->len)
    die("`pos + len' is too far after the end of the buffer");

  if (dlen >= len)
    strbuf_grow(sb, dlen - len);
  memmove(sb->buf + pos + dlen,
          sb->buf + pos + len,
          sb->len - pos - len);
  memcpy(sb->buf + pos, data, dlen);
  strbuf_setlen(sb, sb->len + dlen - len);
}


void strbuf_remove(struct strbuf *sb, size_t pos, size_t len)
{
  strbuf_splice(sb, pos, len, "", 0);
}

ssize_t strbuf_write(struct strbuf *sb, FILE *f)
{
  return sb->len ? fwrite(sb->buf, 1, sb->len, f) : 0;
}

void NORETURN die(const char *err, ...)
{
  va_list params;
  va_start(params, err);
  vprintf(err, params);
  va_end(params);
  exit(127);
}

void *xrealloc(void *ptr, size_t size)
{
  void *ret;
  ret = realloc(ptr, size);
  if (!ret && !size)
	ret = realloc(ptr, 1);
  if (!ret)
	die("Out of memory, realloc failed");
  return ret;
}
