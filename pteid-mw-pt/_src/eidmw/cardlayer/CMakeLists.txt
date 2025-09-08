project(${PTEID_CARDLAYER} VERSION 1.0 LANGUAGES CXX)

if(WIN32)
	set(CMAKE_PREFIX_PATH "${LIB_ROOT_FOLDER}/openssl/${ARCH}" "${LIB_ROOT_FOLDER}/openpace/${ARCH}/lib")
	link_directories("${LIB_ROOT_FOLDER}/openpace/${ARCH}/lib")
    elseif(APPLE)
	set(CMAKE_PREFIX_PATH "${LIB_ROOT_FOLDER}/openpace/lib")
	link_directories("${LIB_ROOT_FOLDER}/openpace/lib")   
    else()
    find_package(PkgConfig)
    pkg_search_module(libeac REQUIRED libeac)
    pkg_search_module(libpcsclite REQUIRED libpcsclite)
    find_package(OpenSSL REQUIRED)
endif()
include_directories(../common)
include_directories(${OPENSSL_INCLUDE_DIR})
include_directories(${LIB_ROOT_FOLDER}/openpace/${ARCH}/include)

set(sources APDU.cpp Cache.cpp Card.cpp CardFactory.cpp CardLayer.cpp CardReaderInfo.cpp Context.cpp 
    PCSC.cpp PaceAuthentication.cpp Pinpad.cpp GenericPinpad.cpp PKCS15.cpp PKCS15Parser.cpp PkiCard.cpp 
    Reader.cpp ReadersInfo.cpp ThreadPool.cpp GempcPinpad.cpp ACR83Pinpad.cpp PteidCard.cpp UnknownCard.cpp
    IcaoCard.cpp SecureMessaging.cpp BacAuthentication.cpp ChipAuthentication.cpp FciData.cpp
    APDU.h Cache.h Card.h CardFactory.h CardLayer.h CardLayerConst.h Context.h InternalConst.h 
	 P15Objects.h PCSC.h GenericPinpad.h PaceAuthentication.h Pinpad.h PKCS15.h PKCS15Parser.h
    PkiCard.h Reader.h ReadersInfo.h ThreadPool.h UnknownCard.h pinpad2.h GempcPinpad.h ACR83Pinpad.h PteidCard.h CardReaderInfo.h
    IcaoCard.h SecureMessaging.h BacAuthentication.h ChipAuthentication.h FciData.h)

if(WIN32)
	set(sources ${sources} Win32ReaderInfo.cpp)
	add_compile_definitions(WIN32;NDEBUG;_WINDOWS;_USRDLL;EIDMW_CAL_EXPORT;)
elseif(LINUX)
    add_compile_definitions(__UNIX__)
endif()

add_library(${PROJECT_NAME} SHARED ${sources} ${PROJECT_NAME}.rc)

target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENSSL_CRYPTO_LIBRARY} ${PTEID_COMMON})
if(WIN32)
        target_link_libraries(${PROJECT_NAME} PRIVATE WS2_32;Crypt32;WinsCard;SetupApi;libeacMT ${PTEID_DLGSWIN32})
    elseif(APPLE)
        target_include_directories(${PROJECT_NAME} PRIVATE /usr/include/PCSC)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${PTEID_DIALOGSQT} libeac.a)
        target_link_libraries(${PROJECT_NAME} PRIVATE "-framework PCSC")
    elseif(LINUX)
        target_include_directories(${PROJECT_NAME} PRIVATE /usr/include/PCSC)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${PTEID_DIALOGSQT} eac pcsclite)
endif()
