#include <FreeImagePTEiD.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	

	// initialize the FreeImage library
	FreeImage_Initialise(TRUE);

	BYTE_FREEIMAGE *mem_buffer = (BYTE_FREEIMAGE*)malloc(12931 * sizeof(BYTE_FREEIMAGE));
      	FILE *stream = fopen("imagem.jp2", "rb");
      	if(stream) {
        	fread(mem_buffer, sizeof(BYTE_FREEIMAGE), 12931, stream);
       		fclose(stream);
		FIMEMORY *hmem = FreeImage_OpenMemory(mem_buffer, 12931);
        	FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(hmem, 0);
        	printf("-----------> %d\n",fif);
	}
	free(mem_buffer);
	FreeImage_DeInitialise();
	
	
	/*
	int y;
	FIBITMAP *dib;
	long status = -1;
	
	FreeImage_Initialise(TRUE);

	dib = FreeImage_Load(FIF_JP2, "rui.bin", JP2_DEFAULT);	
	if (dib){
		printf("---------> OK rui\n");
	} 
	status = FreeImage_Save(FIF_JPEG, dib, "rui.jpg", JPEG_DEFAULT);
	if (status){
		printf("-------> OK rui\n");
	}
	
	FreeImage_DeInitialise();
	return 0;
	*/
	return 0;
}
