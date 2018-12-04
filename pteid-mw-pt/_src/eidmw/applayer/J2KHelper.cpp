#include "J2KHelper.h"

// --------------------------------------------------------------------------
// Memory Helper Functions
unsigned _MemoryReadProc(void *buffer, unsigned size, unsigned count, void *data) {
	unsigned x;

	MEMORY_HEADER *mem_header = static_cast<MEMORY_HEADER*>(data);

	for (x = 0; x < count; x++) {
		long remaining_bytes = mem_header->file_length - mem_header->current_position;
		//if there isn't size bytes left to read, set pos to eof and return a short count
		if (remaining_bytes < static_cast<long>(size)) {
			if (remaining_bytes > 0) {
				memcpy(buffer, static_cast<char *>(mem_header->data) + mem_header->current_position, static_cast<size_t>(remaining_bytes));
			}
			mem_header->current_position = mem_header->file_length;
			break;
		}
		//copy size bytes count times
		memcpy(buffer, static_cast<char *>(mem_header->data) + mem_header->current_position, size);
		mem_header->current_position += size;
		buffer = static_cast<char *>(buffer)+size;
	}
	return x;
}

unsigned _MemoryWriteProc(void *buffer, unsigned size, unsigned count, void *data) {
	void *newdata;
	long newdatalen;

	MEMORY_HEADER *mem_header = static_cast<MEMORY_HEADER*>(data);

	//double the data block size if we need to
	while ((mem_header->current_position + static_cast<long>(size * count)) >= mem_header->data_length) {
		//if we are at or above 1G, we cant double without going negative
		if (mem_header->data_length & 0x40000000) {
			//max 2G
			if (mem_header->data_length == 0x7FFFFFFF) {
				return 0;
			}
			newdatalen = 0x7FFFFFFF;
		}
		else if (mem_header->data_length == 0) {
			//default to 4K if nothing yet
			newdatalen = 4096;
		}
		else {
			//double size
			newdatalen = mem_header->data_length << 1;
		}
		newdata = realloc(mem_header->data, static_cast<size_t>(newdatalen));
		if (!newdata) {
			return 0;
		}
		mem_header->data = newdata;
		mem_header->data_length = newdatalen;
	}
	memcpy(static_cast<char *>(mem_header->data) + mem_header->current_position, buffer, size * count);
	mem_header->current_position += size * count;
	if (mem_header->current_position > mem_header->file_length) {
		mem_header->file_length = mem_header->current_position;
	}
	return count;
}

int _MemorySeekProc(void *data, long offset, int origin) {
	MEMORY_HEADER *mem_header = static_cast<MEMORY_HEADER*>(data);

	// you can use _MemorySeekProc to reposition the pointer anywhere in a file

	switch (origin) { //0 to filelen-1 are 'inside' the file
	default:
	case SEEK_SET: //can fseek() to 0-7FFFFFFF always
		if (offset >= 0 && offset <= mem_header->file_length) {
			mem_header->current_position = offset;
			return 0;
		}
		break;

	case SEEK_CUR:
		if (mem_header->current_position + offset >= 0 && (mem_header->current_position + offset) <= mem_header->file_length) {
			mem_header->current_position += offset;
			return 0;
		}
		break;

	case SEEK_END:
		if (mem_header->file_length >= 0) {
			mem_header->current_position = mem_header->file_length;
			return 0;
		}
		break;
	}

	return -1;
}

long _MemoryTellProc(void *data) {
	MEMORY_HEADER *mem_header = static_cast<MEMORY_HEADER*>(data);
	return mem_header->current_position;
}

static OPJ_UINT64
_LengthProc(void *data) {
	long start_pos = _MemoryTellProc(data);
	_MemorySeekProc(data, 0, SEEK_END);
	unsigned file_length = static_cast<unsigned int>(_MemoryTellProc(data) - start_pos);
	_MemorySeekProc(data, start_pos, SEEK_SET);
	return static_cast<OPJ_UINT64>(file_length);
}

static OPJ_SIZE_T
_ReadProc(void *p_buffer, OPJ_SIZE_T p_nb_bytes, void *data) {
	OPJ_SIZE_T l_nb_read = _MemoryReadProc(p_buffer, 1, static_cast<unsigned>(p_nb_bytes), data);
	return l_nb_read ? l_nb_read : static_cast<OPJ_SIZE_T>(-1);
}

static OPJ_SIZE_T
_WriteProc(void *p_buffer, OPJ_SIZE_T p_nb_bytes, void *data) {
	return _MemoryWriteProc(p_buffer, 1, static_cast<unsigned>(p_nb_bytes), data);
}

static OPJ_OFF_T
_SkipProc(OPJ_OFF_T p_nb_bytes, void *data) {
	if (_MemorySeekProc(data, static_cast<long>(p_nb_bytes), SEEK_CUR)) {
		return -1;
	}
	return p_nb_bytes;
}

static OPJ_BOOL
_SeekProc(OPJ_OFF_T p_nb_bytes, FILE * p_user_data) {
	if (_MemorySeekProc(p_user_data, static_cast<long>(p_nb_bytes), SEEK_SET)) {
		return OPJ_FALSE;
	}
	return OPJ_TRUE;
}

opj_stream_t * opj_image_stream_create(void *data) {

	opj_stream_t *l_stream = opj_stream_create(OPJ_J2K_STREAM_CHUNK_SIZE, OPJ_TRUE);
	if (l_stream) {
		opj_stream_set_user_data(l_stream, data, nullptr);
		opj_stream_set_user_data_length(l_stream, _LengthProc(data));
		opj_stream_set_read_function(l_stream, static_cast<opj_stream_read_fn>(_ReadProc));
		opj_stream_set_write_function(l_stream, static_cast<opj_stream_write_fn>(_WriteProc));
		opj_stream_set_skip_function(l_stream, static_cast<opj_stream_skip_fn>(_SkipProc));
		opj_stream_set_seek_function(l_stream, reinterpret_cast<opj_stream_seek_fn>(_SeekProc));
	}
	return l_stream;
}

// --------------------------------------------------------------------------
// PNG Conversion Functions
/* PNG allows bits per sample: 1, 2, 4, 8, 16 */

void convert_16u32s_C1R(const OPJ_BYTE* pSrc, OPJ_INT32* pDst,
	OPJ_SIZE_T length)
{
	OPJ_SIZE_T i;
	for (i = 0; i < length; i++) {
		OPJ_INT32 val0 = *pSrc++;
		OPJ_INT32 val1 = *pSrc++;
		pDst[i] = val0 << 8 | val1;
	}
}

static void convert_32s16u_C1R(const OPJ_INT32* pSrc, OPJ_BYTE* pDst,
	OPJ_SIZE_T length)
{
	OPJ_SIZE_T i;
	for (i = 0; i < length; i++) {
		OPJ_UINT32 val = static_cast<OPJ_UINT32>(pSrc[i]);
		*pDst++ = static_cast<OPJ_BYTE>(val >> 8);
		*pDst++ = static_cast<OPJ_BYTE>(val);
	}
}


// --------------------------------------------------------------------------

static bool validate_jp2(void *data) {
	unsigned char jp2_signature[] = { 0x00, 0x00, 0x00, 0x0C, 0x6A, 0x50, 0x20, 0x20, 0x0D, 0x0A, 0x87, 0x0A };
	unsigned char signature[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	long tell = _MemoryTellProc(data);

	_MemoryReadProc(signature, 1, sizeof(jp2_signature), data);
	_MemorySeekProc(data, tell, SEEK_SET);

	return (memcmp(jp2_signature, signature, sizeof(jp2_signature)) == 0);
}

bool validate(IMAGE_FORMAT image_format, PHOTO_STREAM *p_stream) {
	bool valid = false;

	if (image_format == JP2)
	{
		long tell = _MemoryTellProc(p_stream->data);
		valid = validate_jp2(p_stream->data);
		_MemorySeekProc(p_stream->data, tell, SEEK_SET);
	}

	return valid;
}

void write_callback(png_structp png_ptr, png_bytep data, png_size_t length)
{
	/* with libpng15 next line causes pointer deference error; use libpng12 */
	struct PNG_MEM_ENCODE* s_pointer = static_cast<struct PNG_MEM_ENCODE*>(png_get_io_ptr(png_ptr));
	unsigned long nsize = s_pointer->size + length;

	/* allocate or grow buffer */
	if (s_pointer->buffer){
		s_pointer->buffer = static_cast<unsigned char *>(realloc(s_pointer->buffer, nsize));
	}
	else {
		s_pointer->buffer = static_cast<unsigned char *>(malloc(nsize));
	}

	if (!s_pointer->buffer){
		png_error(png_ptr, "Write Error");
	}

	/* copy new bytes to end of buffer */
	memcpy(s_pointer->buffer + s_pointer->size, data, length);
	s_pointer->size += length;
}

void flush_callback(png_structp png_ptr)
{
	//this callback does nothing, otherwise libpng will attemp to flush it before saving the values
	//the structure is free'd after it has been copied to the buffer
}

int jp2_to_png(opj_image_t * image, unsigned char ** buffer, unsigned long* image_len)
{
	png_structp png = nullptr;
	png_infop info = nullptr;
	png_bytep volatile row_buf = nullptr;
	png_bytepp row_pointers = nullptr;

	int nr_comp, color_type;
	volatile int prec;
	png_color_8 sig_bit;
	OPJ_INT32 const* planes[4];
	int i;
	OPJ_INT32* volatile buffer32s = nullptr;

	volatile int fails = 1;

	memset(&sig_bit, 0, sizeof(sig_bit));
	prec = static_cast<int>(image->comps[0].prec);
	planes[0] = image->comps[0].data;
	nr_comp = static_cast<int>(image->numcomps);

	if (nr_comp > 4) {
		nr_comp = 4;
	}
	for (i = 1; i < nr_comp; ++i) {
		if (image->comps[0].dx != image->comps[i].dx) {
			break;
		}
		if (image->comps[0].dy != image->comps[i].dy) {
			break;
		}
		if (image->comps[0].prec != image->comps[i].prec) {
			break;
		}
		if (image->comps[0].sgnd != image->comps[i].sgnd) {
			break;
		}
		planes[i] = image->comps[i].data;
	}
	if (i != nr_comp) {
		fprintf(stderr,
			"imagetopng: All components shall have the same subsampling, same bit depth, same sign.\n");
		fprintf(stderr, "\tAborting\n");
		return 1;
	}
	for (i = 0; i < nr_comp; ++i) {
		clip_component(&(image->comps[i]), image->comps[0].prec);
	}
	if (prec > 8 && prec < 16) {
		for (i = 0; i < nr_comp; ++i) {
			scale_component(&(image->comps[i]), 16);
		}
		prec = 16;
	}
	else if (prec < 8 && nr_comp > 1) { /* GRAY_ALPHA, RGB, RGB_ALPHA */
		for (i = 0; i < nr_comp; ++i) {
			scale_component(&(image->comps[i]), 8);
		}
		prec = 8;
	}
	else if ((prec > 1) && (prec < 8) && ((prec == 6) ||
		((prec & 1) == 1))) { /* GRAY with non native precision */
		if ((prec == 5) || (prec == 6)) {
			prec = 8;
		}
		else {
			prec++;
		}
		for (i = 0; i < nr_comp; ++i) {
			scale_component(&(image->comps[i]), static_cast<OPJ_UINT32>(prec));
		}
	}

	if (prec != 1 && prec != 2 && prec != 4 && prec != 8 && prec != 16) {
		fprintf(stderr, "imagetopng: can not create\n\twrong bit_depth %d\n", prec);
		return fails;
	}

	/* Create and initialize the png_struct with the desired error handler
	 * functions.  If you want to use the default stderr and longjump method,
	 * you can supply NULL for the last three parameters.  We also check that
	 * the library version is compatible with the one used at compile time,
	 * in case we are using dynamically linked libraries.  REQUIRED.
	 */
	png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (!png) {
		goto clean_up;
	}

	/* Allocate/initialize the image information data.  REQUIRED
	 */
	info = png_create_info_struct(png);

	if (!info) {
		goto clean_up;
	}

	/* Set error handling.  REQUIRED if you are not supplying your own
	 * error handling functions in the png_create_write_struct() call.
	 */
	if (setjmp(png_jmpbuf(png))) {
		goto clean_up;
	}

	/* I/O initialization functions is REQUIRED
	 */

	struct PNG_MEM_ENCODE state;
	state.buffer = nullptr;
	state.size = 0;

	png_set_write_fn(png, &state, write_callback, flush_callback);

	/* Set the image information here.  Width and height are up to 2^31,
	 * bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
	 * the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
	 * PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
	 * or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
	 * PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
	 * currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE.
	 * REQUIRED
	 *
	 * ERRORS:
	 *
	 * color_type == PNG_COLOR_TYPE_PALETTE && bit_depth > 8
	 * color_type == PNG_COLOR_TYPE_RGB && bit_depth < 8
	 * color_type == PNG_COLOR_TYPE_GRAY_ALPHA && bit_depth < 8
	 * color_type == PNG_COLOR_TYPE_RGB_ALPHA) && bit_depth < 8
	 *
	 */
	png_set_compression_level(png, Z_BEST_COMPRESSION);

	if (nr_comp >= 3) { /* RGB(A) */
		color_type = PNG_COLOR_TYPE_RGB;
		sig_bit.red = sig_bit.green = sig_bit.blue = static_cast<png_byte>(prec);
	}
	else { /* GRAY(A) */
		color_type = PNG_COLOR_TYPE_GRAY;
		sig_bit.gray = static_cast<png_byte>(prec);
	}
	if ((nr_comp & 1) == 0) { /* ALPHA */
		color_type |= PNG_COLOR_MASK_ALPHA;
		sig_bit.alpha = static_cast<png_byte>(prec);
	}

	png_set_IHDR(png, info, image->comps[0].w, image->comps[0].h, prec, color_type,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_set_sBIT(png, info, &sig_bit);

	png_write_info(png, info);

	/* setup conversion */
	{
		OPJ_SIZE_T row_stride;
		png_size_t png_row_size;

		png_row_size = png_get_rowbytes(png, info);
		row_stride = static_cast<OPJ_SIZE_T>((image->comps[0].w * static_cast<OPJ_SIZE_T> (nr_comp)*
			static_cast<OPJ_SIZE_T> (prec)+7U) / 8U);
		if (row_stride != static_cast<OPJ_SIZE_T>(png_row_size)) {
			fprintf(stderr, "Invalid PNG row size\n");
			goto clean_up;
		}

		row_buf = static_cast<png_bytep>(malloc(png_row_size));
		if (!row_buf) {
			fprintf(stderr, "Can't allocate memory for PNG row\n");
			goto clean_up;
		}

		buffer32s = static_cast<OPJ_INT32*>(malloc((static_cast<OPJ_SIZE_T>(image->comps[0].w * static_cast<OPJ_SIZE_T> (nr_comp)* sizeof(OPJ_INT32)))));
		if (!buffer32s) {
			fprintf(stderr, "Can't allocate memory for interleaved 32s row\n");
			goto clean_up;
		}
	}

	/* convert */
	{
		OPJ_SIZE_T width = image->comps[0].w;
		OPJ_UINT32 y;
		convert_32s_PXCX cvtPxToCx = convert_32s_PXCX_LUT[nr_comp];
		convert_32sXXx_C1R cvt32sToPack = nullptr;
		OPJ_INT32 adjust = image->comps[0].sgnd ? 1 << (prec - 1) : 0;
		png_bytep row_buf_cpy = row_buf;
		OPJ_INT32* buffer32s_cpy = buffer32s;

		switch (prec) {
		case 1:
		case 2:
		case 4:
		case 8:
			cvt32sToPack = convert_32sXXu_C1R_LUT[prec];
			break;
		case 16:
			cvt32sToPack = convert_32s16u_C1R;
			break;
		default:
			break;
		}

		row_pointers = static_cast<png_bytepp>(png_malloc(png, image->comps[0].h * sizeof(png_uint_16 *)));

		for (y = 0; y < image->comps[0].h; ++y) {
			cvtPxToCx(planes, buffer32s_cpy, width, adjust);
			cvt32sToPack(buffer32s_cpy, row_buf_cpy, width * static_cast<OPJ_SIZE_T>(nr_comp));
			png_write_row(png, row_buf_cpy);
			row_pointers[y] = row_buf_cpy;
			planes[0] += width;
			planes[1] += width;
			planes[2] += width;
			planes[3] += width;
		}
	}

	png_set_rows(png, info, row_pointers);

#ifdef IS_BIG_ENDIAN
	png_write_png(png, info, PNG_TRANSFORM_SWAP_ENDIAN, nullptr);
#else
	png_write_png(png, info, PNG_TRANSFORM_IDENTITY, nullptr);
#endif
	*image_len = state.size;
	*buffer = static_cast<unsigned char *>(malloc(((*image_len) * sizeof(char))));
	memcpy(*buffer, state.buffer, *image_len);
	fails = 0;

clean_up:
	if (png) {
		png_destroy_write_struct(&png, &info);
	}
	if (png)
		if (row_buf) {
			free(row_buf);
		}
	if (row_pointers) {
		free(row_pointers);
	}
	if (buffer32s) {
		free(buffer32s);
	}
	if (state.buffer){
		free(state.buffer);
	}

	return fails;
}

int load_jp2(PHOTO_STREAM *p_stream, opj_stream_t *stream, unsigned char **buffer, unsigned long *image_len) {
	if (p_stream && stream) {
		opj_codec_t *d_codec = nullptr;	// handle to a decompressor
		opj_dparameters_t parameters;	// decompression parameters
		opj_image_t *image = nullptr;		// decoded image

		// check the file format
		if (!validate(JP2, p_stream)) {
			return -1;
		}

		// get the OpenJPEG stream
		opj_stream_t *d_stream = stream;

		// set decoding parameters to default values
		opj_set_default_decoder_parameters(&parameters);

		try {
			// decode the JPEG-2000 file
			// get a decoder handle
			d_codec = opj_create_decompress(OPJ_CODEC_JP2);

			// configure the event callbacks
			// catch events using our callbacks (no local context needed here)
			opj_set_info_handler(d_codec, nullptr, nullptr);
			opj_set_warning_handler(d_codec, nullptr, nullptr);
			opj_set_error_handler(d_codec, nullptr, nullptr);

			// setup the decoder decoding parameters using user parameters
			if (!opj_setup_decoder(d_codec, &parameters)) {
				throw "Failed to setup the decoder\n";
			}

			// read the main header of the codestream and if necessary the JP2 boxes
			if (!opj_read_header(d_stream, d_codec, &image)) {
				throw "Failed to read the header\n";
			}

			// decode the stream and fill the image structure
			if (!(opj_decode(d_codec, d_stream, image) && opj_end_decompress(d_codec, d_stream))) {
				throw "Failed to decode image!\n";
			}

			int i = jp2_to_png(image, buffer, image_len);

			// free the codec context
			if (d_codec){
				opj_destroy_codec(d_codec);
				d_codec = nullptr;
			}

			// free image data structure
			if (image){
				opj_image_destroy(image);
				image = nullptr;
			}

			return i;

		}
		catch (const char *text) {
			// free remaining structures
			fprintf(stderr, "Failed to convert to png with error: %s", text);
			if (image){
				opj_image_destroy(image);
			}

			if (d_codec){
				opj_destroy_codec(d_codec);
			}

			return -1;
		}
	}

	return -1;
}

void close_jp2(PHOTO_STREAM *p_stream, opj_stream_t *stream) {

	if (p_stream){
		if (p_stream->data){
			free(p_stream->data);
		}
		free(p_stream);
	}
	if (stream){
		opj_stream_destroy(stream);
	}
}

// --------------------------------------------------------------------------

PHOTO_STREAM* load_memory(unsigned char* data, unsigned long size_in_bytes){
	PHOTO_STREAM *stream = static_cast<PHOTO_STREAM*>(malloc(sizeof(PHOTO_STREAM)));
	if (stream) {
		stream->data = static_cast<unsigned char *>(malloc(sizeof(MEMORY_HEADER)));

		if (stream->data) {
			MEMORY_HEADER *mem_header = static_cast<MEMORY_HEADER*>((stream->data));

			// initialize the memory header
			memset(mem_header, 0, sizeof(MEMORY_HEADER));

			if (data && size_in_bytes) {
				mem_header->delete_me = 0;
				mem_header->data = static_cast<unsigned char*>(data);
				mem_header->data_length = mem_header->file_length = static_cast<long>(size_in_bytes);
			}
			else {
				mem_header->delete_me = 1;
			}

			return stream;
		}
		free(stream);
	}

	return nullptr;
}

void convert_to_png(unsigned char* data, unsigned long data_size, unsigned char **mem_buffer, unsigned long * buffer_size) {
	PHOTO_STREAM *source = load_memory(data, data_size);
	if (source && source->data) {
		opj_stream_t * stream = opj_image_stream_create(source->data);
		int i = load_jp2(source, stream, mem_buffer, buffer_size);
		if (i < 0) {
			fprintf(stderr, "Conversion between jp2 and png failed\n");
		}
		close_jp2(source, stream);
	}
	return;
}
