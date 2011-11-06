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
#ifndef EAY_RSA_H
#define EAY_RSA_H

#ifdef	__cplusplus
extern "C" {
#endif

/* RAW RSA operation: out = in ^ exp % mod
 * - in, exp, mod out should be MSB first, positive numbers.
 * - in_len, exp_len <= mod_len <= out_len
 * Returns 0 upon success, or an error value otherwise.
 */
//__declspec(dllexport)
int eay_rsa_raw(const unsigned char *in, int in_len,
                const unsigned char *exp, int exp_len,
                const unsigned char *mod, int mod_len,
                unsigned char *out, int *out_len);

/*
 * RSA encryption of data that will first be RSA PKCS#1 v1.5 padded.
 * Returns 0 upon success, or an error value otherwise.
 */
int eay_rsa_pkcs1_encrypt(const unsigned char *in, int in_len,
                          const unsigned char *exp, int exp_len,
                          const unsigned char *mod, int mod_len,
                          unsigned char *out, int *out_len);

int eay_rsa_pkcs1_decrypt(const unsigned char *in, int in_len,
                          const unsigned char *exp, int exp_len,
                          const unsigned char *mod, int mod_len,
                          unsigned char *out, int *out_len);

int eay_rsa_pkcs1_sign(const unsigned char *in, int in_len,
            const unsigned char *exp, int exp_len,
            const unsigned char *mod, int mod_len,
            unsigned char *sig, int *sig_len);

/*
 * RSA verify of data that will first be RSA PKCS#1 v1.5 padded.
 * Returns 1 (valid) or 0 (invalid), or a negative error value upon failure.
 */
int eay_rsa_pkcs1_verify(const unsigned char *sig, int sig_len,
            const unsigned char *exp, int exp_len,
            const unsigned char *mod, int mod_len,
            unsigned char *msg, int msg_len);

#ifdef	__cplusplus
}
#endif

#endif
