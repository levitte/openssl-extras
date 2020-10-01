/* This is unlicensed, see UNLICENSE and http://unlicense.org/ */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <openssl/bio.h>
#include "btrace.h"

static int btrace_read(BIO *bio, char *in, size_t insize, size_t *numread)
{
  BIO *trace = BIO_get_data(bio);
  int ret;

  if (trace != NULL)
    BIO_printf(trace, "TRACE BIO[read_ex : %p] > in = %p, insize = %zu\n",
               (void *)bio, (void *)in, insize);
  ret = BIO_read_ex(BIO_next(bio), in, insize, numread);
  if (trace != NULL)
    BIO_printf(trace, "TRACE BIO[read_ex : %p] < %d : *numread = %zu\n",
               (void *)bio, ret, *numread);
  return ret;
}

static int btrace_write(BIO *bio, const char *out, size_t outlength,
                        size_t *numwritten)
{
  BIO *trace = BIO_get_data(bio);
  int ret;

  if (trace != NULL)
    BIO_printf(trace, "TRACE BIO[write_ex : %p] > out = %p, outlength = %zu\n",
               (void *)bio, (void *)out, outlength);
  ret = BIO_write_ex(BIO_next(bio), out, outlength, numwritten);
  if (trace != NULL)
    BIO_printf(trace, "TRACE BIO[write_ex : %p] < %d : *numwritten = %zu\n",
               (void *)bio, ret, *numwritten);
  return ret;
}

static long btrace_ctrl(BIO *bio, int cmd, long num, void *ptr)
{
  BIO *trace = BIO_get_data(bio);
  int ret;

  if (trace != NULL)
    BIO_printf(trace, "TRACE BIO[ctrl : %p] > cmd = %d, num = %ld, ptr = %p\n",
               (void *)bio, cmd, num, ptr);
  ret = BIO_ctrl(BIO_next(bio), cmd, num, ptr);
  if (trace != NULL)
    BIO_printf(trace, "TRACE BIO[ctrl : %p] < %d\n", (void *)bio, ret);
  return ret;
}

static long btrace_callback_ctrl(BIO *bio, int cmd, BIO_info_cb *fp)
{
  BIO *trace = BIO_get_data(bio);
  int ret;

  if (trace != NULL)
    BIO_printf(trace, "TRACE BIO[callback_ctrl : %p] > cmd = %d, num = %ld, ptr = %p\n",
               (void *)bio, cmd, (void *)fp);
  ret = BIO_callback_ctrl(BIO_next(bio), cmd, fp);
  if (trace != NULL)
    BIO_printf(trace, "TRACE BIO[callback_ctrl : %p] < %d\n", (void *)bio, ret);
  return ret;
}

static int btrace_gets(BIO *bio, char *buf, int size)
{
  BIO *trace = BIO_get_data(bio);
  int ret;

  if (trace != NULL)
    BIO_printf(trace, "TRACE BIO[gets : %p] > buf = %p, size = %zu\n",
               (void *)bio, buf, size);
  ret = BIO_gets(BIO_next(bio), buf, size);
  if (trace != NULL)
    BIO_printf(trace, "TRACE BIO[gets : %p] < %d\n", (void *)bio, ret);
  return ret;
}

static int btrace_puts(BIO *bio, const char *str)
{
  BIO *trace = BIO_get_data(bio);
  int ret;

  if (trace != NULL)
    BIO_printf(trace, "TRACE BIO[puts : %p] > str = %p\n",
               (void *)bio, (void *)str);
  ret = BIO_puts(BIO_next(bio), str);
  if (trace != NULL)
    BIO_printf(trace, "TRACE BIO[puts : %p] < %d\n", (void *)bio, ret);
  return ret;
}

const BIO_METHOD *make_btrace(void)
{
  static BIO_METHOD *meth = NULL;

  if (meth == NULL)
    {
      if ((meth = BIO_meth_new(BIO_TYPE_FILTER, "btrace")) == NULL
          || !BIO_meth_set_write_ex(meth, btrace_write)
          || !BIO_meth_set_read_ex(meth, btrace_read)
          || !BIO_meth_set_puts(meth, btrace_puts)
          || !BIO_meth_set_gets(meth, btrace_gets)
          || !BIO_meth_set_ctrl(meth, btrace_ctrl)
          || !BIO_meth_set_callback_ctrl(meth, btrace_callback_ctrl))
        {
          BIO_meth_free(meth);
          meth = NULL;
        }
    }
  return meth;
}

void free_btrace(BIO_METHOD *btrace)
{
  BIO_meth_free(btrace);
}

void set_btrace_output_bio(BIO *bio, BIO *trace)
{
  BIO_set_data(bio, trace);
}
