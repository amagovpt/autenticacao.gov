
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <iostream>
#include <ctype.h>
#include <string.h>

#ifndef _WIN32
#include <iconv.h>
#else
#include "win_iconv.h"
#endif

/* Iconv Wrapper */

#ifdef __APPLE__
const char * SINGLE_BYTE_CHARSET_CP1252 = "WINDOWS-1252";
#else
const char * SINGLE_BYTE_CHARSET_CP1252 = "WINDOWS-1252//IGNORE";
#endif
const char * CHARSET_UTF16BE = "UTF-16BE";

const char * INPUT_CHARSET = "UTF-8";

iconv_t
iconv_init(const char *out_charset)
{
    iconv_t conv_desc;
    conv_desc = iconv_open (out_charset, INPUT_CHARSET);
    if (conv_desc == (iconv_t)-1) {
	/* Initialization failure. */
	if (errno == EINVAL) {
	    fprintf (stderr,
		     "Conversion from '%s' to '%s' is not supported.\n",
		     INPUT_CHARSET, out_charset);
	} else {
	    fprintf (stderr, "Initialization failure: %s\n",
		     strerror (errno));
	}
    }
    return conv_desc;
}


/* Convert UTF-8 into LATIN-1 using the iconv library. */

char * iconv_convert(iconv_t conv_desc, const char * utf8_input, bool double_alloc, int *outlen)
{
    size_t iconv_value;
    char * out_buf;
    size_t len;
    size_t out_len;
    /* The variables with "start" in their name are solely for display
       of what the function is doing. As iconv runs, it alters the
       values of the variables, so these are for keeping track of the
       start points and start lengths. */
    char * utf8start;
    const char * euc_start;
    int len_start;

    len = strlen (utf8_input);
    if (!len) {
		return (0);
    }
    /* Alloc enough space to put the output string. */
    out_len = double_alloc ? len*2: len+1;
    out_buf = (char *)calloc (out_len, 1);
    /* Keep track of the variables. */
    len_start = len;
    size_t outbuffer_len = out_len;
    utf8start = out_buf;
    euc_start = utf8_input;

#ifdef __APPLE__
    //MacOS specific API from libiconv
    int discard_value = 1;
    iconvctl(conv_desc, ICONV_SET_DISCARD_ILSEQ, &discard_value);
#endif
#ifdef _WIN32
    iconv_value = iconv(conv_desc, (const char **)&utf8_input, &len, &out_buf, &out_len);
#else
	iconv_value = iconv(conv_desc, (char **)&utf8_input, &len, &out_buf, &out_len);
#endif
    /* Handle failures. */
    if (iconv_value == (size_t) -1) {
		fprintf(stderr, "iconv failed: in string '%s', length %d, "
			"out string '%s', length %d\n",
			 utf8_input, (int)len, utf8start, (int)out_len);
		switch (errno) {
		    /* See "man 3 iconv" for an explanation. */
		case EILSEQ:
		    fprintf(stderr, "Invalid multibyte sequence.\n");
		    break;
		case EINVAL:
		    fprintf(stderr, "Incomplete multibyte sequence.\n");
		    break;
		case E2BIG:
		    fprintf(stderr, "No more room.\n");
		    break;
		default:
		    fprintf(stderr, "Error: %s.\n", strerror (errno));
		}
    }
	if (outlen != NULL)
		*outlen = outbuffer_len-out_len;
    return utf8start;
}

void iconv_finalize (iconv_t conv_desc)
{
    int v;
    v = iconv_close (conv_desc);
    if (v != 0) {
	fprintf (stderr, "iconv_close failed: %s\n", strerror (errno));
    }
}

/* Externally used API */
char * utf8_to_latin1(const char * in)
{
	iconv_t conv_desc;
	conv_desc = iconv_init(SINGLE_BYTE_CHARSET_CP1252);
	bool double_alloc = false;
	char *out_string = iconv_convert(conv_desc, in, double_alloc, NULL);
	iconv_finalize (conv_desc);

	return out_string;
}

char * utf8_to_utf16be(const char * in, int *out_len) {
	iconv_t conv_desc;
	conv_desc = iconv_init(CHARSET_UTF16BE);
	bool double_alloc = true;
	char *out_string = iconv_convert(conv_desc, in, double_alloc, out_len);
	iconv_finalize(conv_desc);

	return out_string;
}



#ifdef TESTING_ICONV

void write_hex_string(char *str)
{


	char c = 0;

	for (int i=0; i!=strlen(str); i++)
	{
		c = str[i];
		if(isascii(c))
			std::cout << c;
		else
			std::cout << "\\x" << std::hex << (static_cast<int>(c) & 0xFF);

	}

	std::cout << ".\n";
}

int main(int argc, char **argv)
{

	const char *t[] = {
		"Hello, my friend, this is the sanctuary of ...",
		"Euro: €pá, pé, Chulé",
		"O presidente do CNECV que já há implícito Há médicos serviço",
		"mamã gugu-dódá",
		"Great British Pound: £",

	};

	for (int i=0; i!=sizeof(t)/sizeof(char*); i++)
	{
		std::cout << "UTF-8 str: " << t[i] << '\n';

		std::cout << "Latin-1 str: ";

		char * latin1_str = utf8_to_latin1(t[i]);
		write_hex_string(latin1_str);

	}

}
#endif
