/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
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
#include "bn.h"
#include "cryptlib.h"

#ifdef WIN32
#pragma warning (disable: 4311)
#endif

static void dumphex(FILE *f, const char *msg, const unsigned char *buf, int len)
{
   int i;
   fprintf(f, "%s: ", msg);
   for (i = 0; i < len; i++)
      fprintf(f, "%02x ", buf[i]);
   fprintf(f, "(%d bytes)\n", len);
}

int eay_rsa_raw(const unsigned char *in, int in_len,
                const unsigned char *exp, int exp_len,
                const unsigned char *mod, int mod_len,
                unsigned char *out, int *out_len)
{
   BIGNUM *b_in, *b_exp, *b_mod, *b_out;
   BN_CTX *ctx;
   int res;

   if (in_len > mod_len || exp_len > mod_len || *out_len < mod_len)
      return BN_R_INVALID_LENGTH;

   b_in = BN_bin2bn(in, in_len, NULL);
   b_exp = BN_bin2bn(exp, exp_len, NULL);
   b_mod = BN_bin2bn(mod, mod_len, NULL);
   b_out = BN_new();
   ctx = BN_CTX_new();

   /* The actual modular exponentiation */
   res = BN_mod_exp(b_out, b_in, b_exp, b_mod, ctx);

   BN_free(b_in);
   BN_free(b_exp);
   BN_free(b_mod);

   if (res == 1) {
      *out_len = BN_bn2bin(b_out, out);
      BN_free(b_out);
      res = 0;
   }

   BN_CTX_free(ctx);

   return res;
}

int eay_rsa_pkcs1_encrypt(const unsigned char *in, int in_len,
                          const unsigned char *exp, int exp_len,
                          const unsigned char *mod, int mod_len,
                          unsigned char *out, int *out_len)
{
   int i, res, ps_end;
   unsigned char *raw_in;
   unsigned long a, b;

   if (in_len > mod_len - 11)
      return BN_R_INVALID_LENGTH;

   raw_in = (unsigned char *) OPENSSL_malloc(mod_len);
   if (raw_in == NULL)
      return ERR_R_MALLOC_FAILURE;

   /* PKCS#1 V1.5 padding */
   raw_in[0] = 0;
   raw_in[1] = 2;
   /* Init a and b with some random values */
   a = (unsigned long) raw_in;
   b = 256 * (256 * (in[0] + 456) + (in[1] + 321)) + in[3] + 267;
   ps_end = mod_len - in_len - 1;
   for (i = 2; i < ps_end; i++) {
      do {
         a += b + mod[i];
         b ^= a;
         raw_in[i] = (unsigned char) (2 * a + b);
      }
      while (raw_in[i] == 0);
   }
   raw_in[ps_end] = 0;
   memcpy(raw_in + ps_end + 1, in, in_len);
   /*dumphex(stdout, "raw_in", raw_in, mod_len);*/

   res = eay_rsa_raw(raw_in, mod_len, exp, exp_len, mod, mod_len, out, out_len);

   OPENSSL_free(raw_in);

   return res;
}

int eay_rsa_pkcs1_decrypt(const unsigned char *in, int in_len,
                          const unsigned char *exp, int exp_len,
                          const unsigned char *mod, int mod_len,
                          unsigned char *out, int *out_len)
{
   int i, res, raw_len;
   unsigned char *raw_out;

   if (in_len > mod_len)
      return BN_R_INVALID_LENGTH;

   raw_out = (unsigned char *) OPENSSL_malloc(mod_len);
   if (raw_out == NULL)
      return ERR_R_MALLOC_FAILURE;

   raw_len = mod_len;
   res = eay_rsa_raw(in, in_len, exp, exp_len, mod, mod_len, raw_out, &raw_len);
   if (res != 0)
      goto done;
   /* dumphex(stdout, "\nraw_out", raw_out, raw_len); */

   /* PKCS#1 V1.5 un-padding */
   if (raw_len != mod_len - 1 || raw_out[0] != 2) {
      res = BN_R_ENCODING_ERROR;
      goto done;
   }
   for (i = 1; raw_out[i] != 0 && i < raw_len; i++)
      ;
   if (i == raw_len || i < 8) {
      res = BN_R_ENCODING_ERROR;
      goto done;
   }
   i++;

   if (*out_len >= raw_len - i) {
      memcpy(out, raw_out + i, raw_len - i);
      res = 0;
   }
   else
      res = BN_R_INVALID_LENGTH;

   *out_len = raw_len - i;

done:
   OPENSSL_free(raw_out);

   return res;
}

int eay_rsa_pkcs1_sign(const unsigned char *in, int in_len,
            const unsigned char *exp, int exp_len,
            const unsigned char *mod, int mod_len,
            unsigned char *sig, int *sig_len)
{
   int i, res, ps_end;
   unsigned char *raw_in;

   if (in_len > mod_len - 11)
      return BN_R_INVALID_LENGTH;

   raw_in = (unsigned char *) OPENSSL_malloc(mod_len);
   if (raw_in == NULL)
      return ERR_R_MALLOC_FAILURE;

   /* PKCS#1 V1.5 padding */
   raw_in[0] = 0;
   raw_in[1] = 1;
   ps_end = mod_len - in_len - 1;
   for (i = 2; i < ps_end; i++)
         raw_in[i] = 0xFF;
   raw_in[ps_end] = 0;
   memcpy(raw_in + ps_end + 1, in, in_len);
   /*dumphex(stdout, "raw_in", raw_in, mod_len);*/

   res = eay_rsa_raw(raw_in, mod_len, exp, exp_len, mod, mod_len, sig, sig_len);

   OPENSSL_free(raw_in);

   return res;
}

int eay_rsa_pkcs1_verify(const unsigned char *sig, int sig_len,
            const unsigned char *exp, int exp_len,
            const unsigned char *mod, int mod_len,
            unsigned char *msg, int msg_len)
{
   int i, res, raw_len;
   unsigned char *raw_out;

   raw_out = (unsigned char *) OPENSSL_malloc(mod_len);
   if (raw_out == NULL)
      return ERR_R_MALLOC_FAILURE;

   raw_len = mod_len;
   res = eay_rsa_raw(sig, sig_len, exp, exp_len, mod, mod_len, raw_out, &raw_len);
   if (res < 0)
      goto done;

   /*dumphex(stdout, "eay_rsa_pkcs1_verify(): raw_out", raw_out, res);*/

   /* PKCS#1 V1.5 un-padding */
   if (raw_len != mod_len - 1 || raw_out[0] != 1) {
      res = 0;
      goto done;
   }
   for (i = 1 ; i < raw_len; i++) {
      if (raw_out[i] != 0xFF)
         break;
   }
   if (raw_len - i != msg_len + 1 || raw_out[i] != 0) {
      res = 0;
      goto done;
   }
   i++;
   /* printf("eay_rsa_pkcs1_verify(): padding OK\n"); */

   res = (memcmp(raw_out + i, msg, msg_len) == 0);

done:
   OPENSSL_free(raw_out);

   return res;
}
