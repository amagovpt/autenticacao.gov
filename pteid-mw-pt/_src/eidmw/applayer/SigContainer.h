#ifndef SIGCONTAINER_H
#define SIGCONTAINER_H

#define SIG_INTERNAL_PATH "META-INF/signature.xml"
#define TS_INTERNAL_PATH "META-INF/ts_resp.bin"

#include <cstdio>
#include "ByteArray.h"
#include "MiscUtil.h"
#define MINIZ_HEADER_FILE_ONLY
#include "miniz.c"

#ifdef WIN32
#define NL "\r\n"
#else
#define NL "\n"
#endif

namespace eIDMW
{

static const char *README = 
"############################################################" NL
"LEIA-ME" NL
"" NL
"Este ficheiro zip contém informação assinada com a respectiva assinatura em META-INF/signature.xml" NL
"Esta assinatura foi criada através da Aplicação Oficial do Cartão de Cidadão." NL
"" NL
"Para verificar a validade da assinatura, deverá na Aplicação Oficial do Cartão de Cidadão aceder ao seguinte menu:" NL
"	\"Opções\" -> \"Verificar Assinatura\" -> Introduzir o ficheiro zip a validar." NL
"" NL
"Mais Informação:" NL
"" NL
"Download da Aplicação Oficial do Cartão de Cidadão:" NL
"http://svn.gov.pt/projects/ccidadao/browser/middleware-offline/tags/builds" NL
"" NL
"Especificação Técnica da Assinatura Digital:" NL
"Xades / Xades-T" NL
"http://www.w3.org/TR/XAdES/" NL
"" NL
"############################################################" NL
"README" NL
"" NL
"This zip file includes signed information. The signature file can be found in META-INF/signature.xml" NL
"This signature was produced through Portuguese ID Card Management application." NL
"" NL
"To verify this signature, use the Portuguese ID Card Management application, following these instructions:" NL
"	\"Tools\" -> \"Verify Signature\" -> Introduce file to be verified." NL
"" NL
"More Info:" NL
"" NL
"Download Portuguese ID Card Management application:" NL
"http://svn.gov.pt/projects/ccidadao/browser/middleware-offline/tags/builds" NL
"" NL
"Signature technical specification:" NL
"Xades / Xades-T" NL
"http://www.w3.org/TR/XAdES" NL;

	class Container
	{
		public:
			Container(const char *zip_path);
			~Container();
			CByteArray &ExtractFile(const char *entry);
			CByteArray &ExtractSignature();
			CByteArray &ExtractTimestamp();
			//Array of pointers to tHashedFiles
			tHashedFile **getHashes(int *);
	
		private:
			mz_zip_archive zip_archive;


	};


	void StoreSignatureToDisk(CByteArray& sig, CByteArray* ts_data, const char **paths, int num_paths, const char *output_file);

}

#endif
