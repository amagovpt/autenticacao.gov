project(${PTEID_APPLAYER} VERSION 1.0 LANGUAGES CXX)

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(XercesC REQUIRED)
find_package(OpenJPEG REQUIRED)
find_package(PNG REQUIRED)
find_package(CURL REQUIRED) 
find_package(libzip REQUIRED)

include_directories(../common;../cardlayer;../dialogs;../eidlib;../pteid-poppler)

set(sources APLCard.cpp APLCardPteid.cpp APLCertif.cpp APLConfig.cpp APLCrypto.cpp APLPublicKey.cpp APLReader.cpp 
	asn1_idfile.cpp CardFile.cpp CardPteid.cpp CertStatusCache.cpp cJSON.c CurlUtil.cpp PAdESExtender.cpp PNGConverter.cpp
        cryptoFramework.cpp cryptoFwkPteid.cpp J2KHelper.cpp MiscUtil.cpp PDFSignature.cpp PhotoPteid.cpp proxyinfo.cpp
	MutualAuthentication.cpp SigContainer.cpp sign-pkcs7.cpp SODParser.cpp SSLConnection.cpp TSAClient.cpp
        XercesUtils.cpp RemoteAddress.cpp RemoteAddressRequest.cpp IcaoDg1.cpp IcaoDg2.cpp IcaoDg3.cpp IcaoDg11.cpp
        IcaoDg14.cpp PKIFetcher.cpp
	APLCard.h APLCardFile.h APLCardPteid.h APLCCXmlDoc.h APLCertif.h APLConfig.h APLCrypto.h APLPublicKey.h
	APLReader.h CardFile.h CardPteid.h CardPteidDef.h CertStatusCache.h cJSON.h PAdESExtender.h PNGConverter.h
        cryptoFramework.h cryptoFwkPteid.h J2KHelper.h MiscUtil.h PDFSignature.h PhotoPteid.h MutualAuthentication.h
        SigContainer.h sign-pkcs7.h SODParser.h SSLConnection.h RemoteAddress.h RemoteAddressRequest.h TSAClient.h XadesSignature.h
        IcaoDg1.h IcaoDg2.h IcaoDg3.h IcaoDg11.h IcaoDg14.h PKIFetcher.h)

add_library(${PROJECT_NAME} SHARED ${sources} ${PROJECT_NAME}.rc)


if (PTEID_MULTIPASS_SDK)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENSSL_CRYPTO_LIBRARY} CURL::libcurl ${PTEID_COMMON} ${PTEID_CARDLAYER} )
    # Some quirkiness on openjpeg's cmake stuff, we need to explicitly add the INCLUDE_DIRS variable
    target_include_directories(${PROJECT_NAME} PRIVATE ${OPENJPEG_INCLUDE_DIRS})
    add_compile_definitions(${PROJECT_NAME} PTEID_MULTIPASS_SDK)
else()
    target_sources(${PROJECT_NAME} PRIVATE XadesSignature.cpp)

    target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY} ${PNG_LIBRARIES} ZLIB::ZLIB ${PTEID_COMMON} ${PTEID_CARDLAYER}
    CURL::libcurl ${OPENJPEG_LIBRARIES} XercesC::XercesC libzip::zip ${PTEID_POPPLER} )
    target_link_libraries(${PROJECT_NAME} PRIVATE xml-security-c)
endif()

target_compile_options(${PROJECT_NAME} PRIVATE -fvisibility=hidden -ffunction-sections -fdata-sections)
target_link_options(${PROJECT_NAME} PRIVATE -Wl,--gc-sections)

if(USE_PCSC)
   target_include_directories(${PROJECT_NAME} PRIVATE /usr/include/PCSC)
endif()


