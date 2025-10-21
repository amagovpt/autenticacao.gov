project(${PTEID_CMDSERVICES} VERSION 1.0 LANGUAGES CXX)

if(WIN32)
	set(CMAKE_PREFIX_PATH "${LIB_ROOT_FOLDER}/openssl/${ARCH}")
elseif(LINUX)
	find_package(OpenSSL REQUIRED)
endif()


include_directories(${OPENSSL_INCLUDE_DIR})
include_directories(../../common)
include_directories(../../applayer)
include_directories(../../cardlayer)
include_directories(../../dialogs)
include_directories(../../eidlib)

set(sources cmdCertificates.cpp cmdSignatureClient.cpp cmdServices.cpp CMDSignature.cpp credentials.cpp soapC.cpp 
                soapBasicHttpBinding_USCORECCMovelSignatureProxy.cpp stdsoap2.cpp
			cmdCertificates.h cmdSignatureClient.h cmdErrors.h cmdServices.h CMDSignature.h credentials.h soapH.h
                        soapStub.h soapBasicHttpBinding_USCORECCMovelSignatureProxy.h stdsoap2.h)

if(WIN32)
	add_compile_definitions(WIN32;NDEBUG;_WINDOWS;WITH_OPENSSL;EIDMW_CMDSERVICES_EXPORTS;_USRDLL;
		OPENSSL_SUPPRESS_DEPRECATED;WIN32_LEAN_AND_MEAN;_WINDLL;_UNICODE;UNICODE)
elseif(UNIX)
    add_compile_definitions(__UNIX__ WITH_OPENSSL OPENSSL_SUPPRESS_DEPRECATED)
endif()

add_library(${PROJECT_NAME} SHARED ${sources} cmdServices.rc)

target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY} ${PTEID_COMMON} ${PTEID_APPLAYER})

if(WIN32)
	target_link_libraries(${PROJECT_NAME} PRIVATE Crypt32;WS2_32 ${PTEID_DLGSWIN32})
    else()
	target_link_libraries(${PROJECT_NAME} PRIVATE ${PTEID_DIALOGSQT})
endif()
