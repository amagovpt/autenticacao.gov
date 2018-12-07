#include <string.h>
#include <string>

#include <zlib.h>
#include <png.h>
#include <openjpeg.h>

#include "PNGConverter.h"

#define PNG_MAGIC "\x89PNG\x0d\x0a\x1a\x0a"
#define MAGIC_SIZE 8

#define JP2_DEFAULT			0		//! save with a 16:1 rate

#if (defined(BYTE_ORDER) && BYTE_ORDER==BIG_ENDIAN) || \
    (defined(__BYTE_ORDER) && __BYTE_ORDER==__BIG_ENDIAN) || \
    defined(__BIG_ENDIAN__)
#define IS_BIG_ENDIAN
#endif


struct PNG_MEM_ENCODE
{
	unsigned char *buffer;
	unsigned long size;
};

struct PHOTO_STREAM {
	void *data;
};

struct MEMORY_HEADER {
	/**
	Flag used to remember to delete the 'data' buffer.
	When the buffer is a wrapped buffer, it is read-only, no need to delete it.
	When the buffer is a read/write buffer, it is allocated dynamically and must be deleted when no longer needed.
	*/
	long delete_me;
	/**
	file_length is equal to the input buffer size when the buffer is a wrapped buffer, i.e. file_length == data_length.
	file_length is the amount of the written bytes when the buffer is a read/write buffer.
	*/
	long file_length;
	/**
	When using read-only input buffers, data_length is equal to the input buffer size, i.e. the file_length.
	When using read/write buffers, data_length is the size of the allocated buffer,
	whose size is greater than or equal to file_length.
	*/
	long data_length;
	/**
	start buffer address
	*/
	void *data;
	/**
	Current position into the memory stream
	*/
	long current_position;
};

enum IMAGE_FORMAT{
	UNKNOWN = -1,

	PNG = 0,
	J2K = 1,
	JP2 = 2
};

void convert_to_png(unsigned char *data, unsigned long size, unsigned char **buffer, unsigned long *buffer_size);
