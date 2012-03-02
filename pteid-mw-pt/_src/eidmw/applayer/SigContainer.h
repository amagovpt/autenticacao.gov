#ifndef SIGCONTAINER_H
#define SIGCONTAINER_H

#define SIG_INTERNAL_PATH "META-INF/signature.xml"
#include <cstdio>
#include "ByteArray.h"
#include "MiscUtil.h"
#define MINIZ_HEADER_FILE_ONLY
#include "miniz.c"

namespace eIDMW
{

static const char *README = 
"############################################################"
"LEIA-ME\n"
"\n"
"Este ficheiro zip contém informação assinada com a respectiva assinatura em META-INF/signature.xml\n"
"Esta assinatura foi criada através da Aplicação Oficial do Cartão de Cidadão.\n"
"\n"
"Para verificar a validade da assinatura, deverá na Aplicação Oficial do Cartão de Cidadão aceder ao seguinte menu:\n"
"	\"Opções\" -> \"Verificar Assinatura\" -> Introduzir o ficheiro zip a validar.  \n"
"\n"
"Mais Informação:\n"
"\n"
"Download da Aplicação Oficial do Cartão de Cidadão:\n"
"http://svn.gov.pt/projects/ccidadao/browser/middleware-offline/tags/builds\n"
"\n"
"Especificação Técnica da Assinatura Digital:\n"
"Xades / Xades-T\n"
"http://www.w3.org/TR/XAdES/\n"
"\n"
"############################################################\n"
"README\n"
"\n"
"This zip file includes signed information. The signature file can be found in META-INF/signature.xml\n"
"This signature was produced through Portuguese ID Card Management application.\n"
"\n"
"To verify this signature, use the Portuguese ID Card Management application, following these instructions:\n"
"	\"Tools\" -> \"Verify Signature\" -> Introduce file to be verified.\n"
"\n"
"More Info:\n"
"\n"
"Download Portuguese ID Card Management application:\n"
"http://svn.gov.pt/projects/ccidadao/browser/middleware-offline/tags/builds\n"
"\n"
"Signature technical specification:\n"
"Xades / Xades-T\n"
"http://www.w3.org/TR/XAdES/\n";

	class Container
	{
		public:
			Container(const char *zip_path);
			~Container();
			CByteArray *ExtractFile(const char *entry);
			CByteArray *ExtractSignature();
			//Array of pointers to tHashedFiles
			tHashedFile **getHashes(int *);
	
		private:
			mz_zip_archive zip_archive;


	};


	void StoreSignatureToDisk(CByteArray& sig, const char **paths, int num_paths, const char *output_file);
}

#endif
