
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#ifndef _WIN32
#include <iconv.h>
#else
#include "win_iconv.h"
#endif

/* Iconv Wrapper for UNIX */

const char * OUTSET = "WINDOWS-1252";
const char * INSET = "UTF-8";

iconv_t
iconv_init(void)
{
    iconv_t conv_desc;
    conv_desc = iconv_open (OUTSET, INSET);
    if (conv_desc == (iconv_t)-1) {
	/* Initialization failure. */
	if (errno == EINVAL) {
	    fprintf (stderr,
		     "Conversion from '%s' to '%s' is not supported.\n",
		     INSET, OUTSET);
	} else {
	    fprintf (stderr, "Initialization failure: %s\n",
		     strerror (errno));
	}
    }
    return conv_desc;
}


/* Convert UTF-8 into LATIN-1 using the iconv library. */

char *
utf82latin1(iconv_t conv_desc, const char * euc)
{
    size_t iconv_value;
    char * utf8;
    size_t len;
    size_t utf8len;
    /* The variables with "start" in their name are solely for display
       of what the function is doing. As iconv runs, it alters the
       values of the variables, so these are for keeping track of the
       start points and start lengths. */
    char * utf8start;
    const char * euc_start;
    int len_start;
    int utf8len_start;

    len = strlen (euc);
    if (!len) {
	fprintf (stderr, "Input string is empty.\n");
	return (0);
    }
    /* Assign enough space to put the Latin1. */
    utf8len = len;
    utf8 = (char *)calloc (utf8len+1, 1);
    /* Keep track of the variables. */
    len_start = len;
    utf8len_start = utf8len;
    utf8start = utf8;
    euc_start = euc;
#ifdef _WIN32
    iconv_value = iconv (conv_desc, (const char **)&euc, & len, & utf8, & utf8len);
#else
	iconv_value = iconv (conv_desc, (char **)&euc, & len, & utf8, & utf8len);
#endif
    /* Handle failures. */
    if (iconv_value == (size_t) -1) {
	fprintf (stderr, "iconv failed: in string '%s', length %d, "
		"out string '%s', length %d\n",
		 euc, (int)len, utf8start, (int)utf8len);
	switch (errno) {
	    /* See "man 3 iconv" for an explanation. */
	case EILSEQ:
	    fprintf (stderr, "Invalid multibyte sequence.\n");
	    break;
	case EINVAL:
	    fprintf (stderr, "Incomplete multibyte sequence.\n");
	    break;
	case E2BIG:
	    fprintf (stderr, "No more room.\n");
	    break;
	default:
	    fprintf (stderr, "Error: %s.\n", strerror (errno));
	}
    }
    return utf8start;
}

/* Close the connection with the library. */

void
iconv_finalize (iconv_t conv_desc)
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
	conv_desc = iconv_init();
	char *out_string = utf82latin1(conv_desc, in);
	iconv_finalize (conv_desc);

	return out_string;

}



#ifdef TESTING_ICONV

void write_hex_string(char *str)
{

	printf("%s", "Hex: ");

	for(int i=0; i!=strlen(str); i++)
	{
		if(isascii(str[i]))
			printf("%c ", (int)str[i]);
		else
			printf("%02x ", str[i]);

	}

	puts(".");

}

int main(int argc, char **argv)
{

	char *t[] = 
	{
		"Hello, my friend, this is the sanctuary of ...",
		"Euro: €pá, pé, Chulé",
		"O presidente do CNECV que já há implícito Há médicos serviço",
		"mamã gugu-dádá",
		"GBP: £",

	};

	for (int i=0; i!=sizeof(t)/sizeof(char*); i++)
	{
		printf("UTF-8 str: %s\n", t[i]);
		puts("Latin1 str: ");
		write_hex_string(utf8_to_latin1(t[i]));

	}

}
#endif


