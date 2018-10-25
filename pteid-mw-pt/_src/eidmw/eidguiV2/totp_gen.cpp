#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <cstring>

#ifndef _WIN32
#include <cstdint>
#endif

#include <openssl/hmac.h>

#ifdef _WIN32
#include <intrin.h> //For _byteswap_uint64
#include <stdint.h> //For uint8_t
#endif

//Borrowed from https://stackoverflow.com/a/3764426/9906
//TODO: this decoder needs better testing, e.g. with inputs with "=" padding chars
void Base32Decode(const std::string & input, std::string & output)
{

    // The "Base 32 Encoding" section of http://tools.ietf.org/html/rfc4648#page-8
    // shows that every 8 bytes of base32 encoded data must be translated back into 5 bytes
    // of original data during a decoding process. The following code does this.

    int input_len = input.length();

    const char * input_str = input.c_str();
    int output_len = (input_len*5)/8;

    unsigned char *output_str = new unsigned char[output_len];

    char curr_char, temp_char;
    long long temp_buffer = 0; //formerly: __int64 temp_buffer = 0;
    for ( int i=0; i<input_len; i++ )
    {
        curr_char = input_str[i];
        if ( curr_char >= 'A' && curr_char <= 'Z' )
            temp_char = curr_char - 'A';
        if ( curr_char >= '2' && curr_char <= '7' )
            temp_char = curr_char - '2' + 26;

        if( temp_buffer )
            temp_buffer <<= 5; //temp_buffer = (temp_buffer << 5);
        temp_buffer |= temp_char;

        // If 8 encoded characters have been decoded into the temp location,
        // then copy them to the appropriate section of the final decoded location
        if((i>0) && !((i+1) % 8))
        {
            unsigned char * source = reinterpret_cast<unsigned char*>(&temp_buffer);
            //strncpy(output_str+(5*(((i+1)/8)-1)), source, 5);
            int start_index = 5*(((i+1)/8)-1);
            int copy_index = 4;
            for (int x=start_index; x<(start_index+5); x++, copy_index-- )
                output_str[x] = source[copy_index];
            temp_buffer = 0;
        }
    }

    output.append((char *)output_str, output_len);

    delete [] output_str;
}

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
std::string generateTOTP(std::string secretKey) {

    std::string key_buffer;

    unsigned char hs[128];
    char output_otp[10];
    int key_len = 0;

    unsigned int md_len = sizeof(hs);

    memset(hs, 0, sizeof(hs));
    memset(output_otp, 0, sizeof(output_otp));

    long S = 0;

    unsigned int decoded_len = 0;
    char * base64_buffer;

    Base64Decode(secretKey.c_str(), secretKey.size(), base64_buffer, decoded_len);

    //key_buffer = std::string(base64_buffer, decoded_len);
    key_buffer = std::string(base64_buffer, decoded_len);
    //Base32Decode(secretKey, key_buffer);

    key_len = key_buffer.size();

    const int digits = 6;

/* Big-endian conversion */
#ifdef _WIN32
    unsigned __int64 msg = _byteswap_uint64(time(NULL) / 60);
    fprintf(stderr, "DEBUG: Timestamp used to generate TOTP: %ld\n", _byteswap_uint64(msg) * 60);
#else
    uint64_t msg = __builtin_bswap64((uint64_t)time(NULL) / 60);
    fprintf(stderr, "DEBUG: Timestamp used to generate TOTP: %ld\n", __builtin_bswap64(msg) * 60);
#endif

    if (HMAC(EVP_sha1(), key_buffer.c_str(), key_len,
            (const unsigned char*) &msg, sizeof(msg), hs, &md_len) != NULL) {

        uint8_t offset = hs[md_len - 1] & 0x0f;

        S = (((hs[offset] & 0x7f) << 24)
            | ((hs[offset + 1] & 0xff) << 16)
            | ((hs[offset + 2] & 0xff) << 8)
                | ((hs[offset + 3] & 0xff)));

        //Digits = 6
        S = S % 1000000;
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
