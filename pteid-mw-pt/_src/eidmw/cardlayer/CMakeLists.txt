project(${PTEID_CARDLAYER} VERSION 1.0 LANGUAGES CXX)

add_compile_definitions(__UNIX__)

find_package(OpenSSL REQUIRED)
include_directories(../common)
include_directories(${OPENSSL_INCLUDE_DIR})
include_directories(${LIB_ROOT_FOLDER}/openpace/${ARCH}/include)

set(sources APDU.cpp Cache.cpp Card.cpp CardFactory.cpp CardLayer.cpp CardReaderInfo.cpp Context.cpp
    PCSC.cpp PaceAuthentication.cpp PKCS15.cpp PKCS15Parser.cpp PkiCard.cpp
    Reader.cpp ReadersInfo.cpp ThreadPool.cpp PteidCard.cpp UnknownCard.cpp
    IcaoCard.cpp SecureMessaging.cpp BacAuthentication.cpp ChipAuthentication.cpp FciData.cpp
    APDU.h Cache.h Card.h CardFactory.h CardLayer.h CardLayerConst.h Context.h InternalConst.h
    P15Objects.h PCSC.h PaceAuthentication.h PKCS15.h PKCS15Parser.h
    PkiCard.h Reader.h ReadersInfo.h ThreadPool.h UnknownCard.h PteidCard.h CardReaderInfo.h
    IcaoCard.h SecureMessaging.h BacAuthentication.h ChipAuthentication.h FciData.h PinpadInterface.h Pinpad.h Pinpad.cpp)

if(USE_PCSC)
    message(STATUS "PCSC support: ENABLED")
    add_compile_definitions(${PROJECT_NAME} PRIVATE __USE_PCSC__=1)

    # add pinpad related files
    list(APPEND sources
        GenericPinpad.h pinpad2.h GempcPinpad.h ACR83Pinpad.h
        GenericPinpad.cpp GempcPinpad.cpp ACR83Pinpad.cpp)
else()
    message(STATUS "PCSC support: DISABLED")
    message(WARNING "Note: Pinpad and minidriver features will not be available")
endif()

add_library(${PROJECT_NAME} SHARED ${sources} ${PROJECT_NAME}.rc)

target_link_libraries(${PROJECT_NAME} PUBLIC ${OPENSSL_CRYPTO_LIBRARY} ${PTEID_COMMON} eac)
if(USE_PCSC)
    target_include_directories(${PROJECT_NAME} PRIVATE /usr/include/PCSC)
    target_link_libraries(${PROJECT_NAME} PRIVATE pcsclite)
endif()
