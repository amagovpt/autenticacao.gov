/*-****************************************************************************

 * Copyright (C) 2018-2019 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2018 Veniamin Craciun - <veniamin.craciun@caixamagica.pt>
 * Copyright (C) 2018-2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <cstring>

#ifndef _WIN32
#include <cstdint>
#endif

#ifdef UNIT_TEST
#include <vector>
#endif

#include <openssl/hmac.h>

#ifdef _WIN32
#include <intrin.h> //For _byteswap_uint64
#include <stdint.h> //For uint8_t
#endif

#include "eidlib.h"

void Base64Decode(const char *array, unsigned int inlen, char * &decoded, unsigned int &decoded_len)
{

    BIO *bio, *b64;
    unsigned int outlen = 0;
    char *inbuf = new char[512];
    memset(inbuf, 0, 512);

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

    bio = BIO_new_mem_buf((void *)array, inlen);
    bio = BIO_push(b64, bio);

    outlen = BIO_read(bio, inbuf, 512);
    decoded = inbuf;

    decoded_len = outlen;

    BIO_free_all(bio);
}


/*   TOTP Generator according to RFC 6238   */
std::string generateTOTP(std::string secretKey, unsigned int digits, unsigned int step_time, time_t my_time) {

    std::string key_buffer;

    unsigned char hs[128];
    char output_otp[10];
    int key_len = 0;
    const int DIGITS_POWER[] =
     // 0 1  2   3    4     5      6       7        8
      { 1,10,100,1000,10000,100000,1000000,10000000,100000000 };

    const int MAX_DIGITS = sizeof(DIGITS_POWER) / sizeof(int);

    unsigned int md_len = sizeof(hs);

    memset(hs, 0, sizeof(hs));
    memset(output_otp, 0, sizeof(output_otp));

    long S = 0;

    if (digits > MAX_DIGITS) {
        eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
            "Should not ever happen, invalid number of digits for generateTOTP... Digits: %d, max_digits: %d", digits, MAX_DIGITS);
        return std::string("");
    }

    unsigned int decoded_len = 0;
    char * base64_buffer;

    Base64Decode(secretKey.c_str(), secretKey.size(), base64_buffer, decoded_len);

    if (decoded_len < 20){
        eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
        "TOTP Base64Decode error decoded_len = %d", decoded_len);
        return std::string("");
    }


    key_buffer = std::string(base64_buffer, decoded_len);

    key_len = key_buffer.size();

/* Big-endian conversion */
#ifdef _WIN32
    unsigned __int64 msg = _byteswap_uint64(my_time / step_time);
    /*fprintf(stderr, "DEBUG: Timestamp used to generate TOTP: %ld\n", _byteswap_uint64(msg) * 60);*/
#else
    uint64_t msg = __builtin_bswap64((uint64_t)my_time / step_time);
    /*fprintf(stderr, "DEBUG: Timestamp used to generate TOTP: %ld\n", __builtin_bswap64(msg) * 60);*/
#endif

    if (HMAC(EVP_sha1(), key_buffer.c_str(), key_len,
            (const unsigned char*) &msg, sizeof(msg), hs, &md_len) != NULL) {

        uint8_t offset = hs[md_len - 1] & 0x0f;

        S = (((hs[offset] & 0x7f) << 24)
            | ((hs[offset + 1] & 0xff) << 16)
            | ((hs[offset + 2] & 0xff) << 8)
                | ((hs[offset + 3] & 0xff)));

        S = S % DIGITS_POWER[digits];

#ifdef _WIN32
        sprintf_s(output_otp, digits + 1, "%.*ld", digits, S);
#else
        snprintf(output_otp, digits + 1, "%.*ld", digits, S);
#endif
        output_otp[digits] = '\0';

        return output_otp;
    }
    else {
        fprintf(stderr, "Error in HMAC-SHA1 in generateTOTP()!!\n");

        return std::string("");
    }
}

/* Build this C++ file with the following define to run the generateTOTP() unit test */

#ifdef UNIT_TEST

    char * format_timet(char * buf, size_t len, time_t *t1) {
        struct tm * time_1 = gmtime(t1);

        strftime(buf, len, "%F %T %z", time_1);

        return buf;
    }

    int main() {
        //String buffer for formatted datetime values
        char time_s[50];

        std::vector<time_t> my_times = { 59, 1111111109, 1111111111, 1234567890, 2000000000, 20000000000 };
        //Reference TOTP values in SHA-1 mode from RFC-6238
        std::vector<std::string> reference_values = {"94287082", "07081804", "14050471", "89005924", "69279037", "65353130"};

        //Base-64 encoding of the SHA-1 RFC-6238 test key
        const std::string key = "MTIzNDU2Nzg5MDEyMzQ1Njc4OTA=";
        const int step_time = 30;
        const int digits = 8;
        int i = 0;

        for (auto t: my_times) {
            format_timet(time_s, sizeof(time_s), &t);
            fprintf(stderr, "Time: %s Reference TOTP: %s My TOTP: %s\n",
                             time_s, reference_values[i].c_str(), generateTOTP(key, digits, step_time, t).c_str());
            i++;
        }


        return 0;
    }

#endif
