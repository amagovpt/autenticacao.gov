#include <FreeImagePTEiD.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message) {
    printf("\n*** ");

    if(fif != FIF_UNKNOWN) {
        if (FreeImage_GetFormatFromFIF(fif))
            printf("%s Format\n", FreeImage_GetFormatFromFIF(fif));
    }
    printf(message);
    printf(" ***\n");
}


int main(int argc, char *argv[])
{
	

/*
	
	FreeImage_Initialise(TRUE);
	int file_size = 10365;

	BYTE *mem_buffer = (BYTE*)malloc(file_size* sizeof(BYTE));
      	FILE *stream = fopen("/home/agrr/Downloads/cartaodecidadao-explorer-master/cache/0000000010959658_MANUEL SILVA/photo.jp2", "rb");
      	if(stream) {
        	fread(mem_buffer, sizeof(BYTE), file_size, stream);
       		fclose(stream);

			FIMEMORY *hmem = FreeImage_OpenMemory(mem_buffer, file_size);
        	FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(hmem, 0);
        	printf("FREE_IMAGE_FORMAT = %d\n", fif);
		}
	free(mem_buffer);
	FreeImage_DeInitialise();
	return 0;
*/	
	
	int y;
	FIBITMAP *dib;
	long status = -1;
	int file_size = 10365;
	
	FreeImage_Initialise(TRUE);

	FreeImage_SetOutputMessage(FreeImageErrorHandler);

	fprintf(stderr, "Is plugin enabled PNG?: %d\n", FreeImage_IsPluginEnabled(FIF_PNG));
	//fprintf(stderr, "Is plugin enabled JPEG?: %d\n", FreeImage_IsPluginEnabled(FIF_JPEG));
	fprintf(stderr, "Is plugin enabled JP2?: %d\n", FreeImage_IsPluginEnabled(FIF_JP2));

	
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(argv[1], file_size);

	dib = FreeImage_Load(fif, argv[1], JP2_DEFAULT);

	if (dib){
		fprintf(stderr, "Loading was OK rui\n");
	}
	status = FreeImage_Save(FIF_PNG, dib, argv[2], PNG_DEFAULT);
	if (status){
		printf("-------> OK\n");
	}
	else
	{
		printf("Error saving stuff Error code: %d\n", status);
	}
	
	FreeImage_DeInitialise();
	return 0;
	
}
