project(${PTEID_COMMON} VERSION 1.0 LANGUAGES CXX)

find_package(OpenSSL REQUIRED)

include_directories(${OPENSSL_INCLUDE_DIR})

add_library(${PROJECT_NAME} SHARED ${PROJECT_NAME}.rc)

target_sources(${PTEID_COMMON}
        PUBLIC Config.cpp ByteArray.cpp ConfigCommon.cpp datafile.cpp Hash.cpp Log.cpp
        LogBase.cpp Mutex.cpp MWException.cpp Thread.cpp Util.cpp
        StringOps.cpp MyriadFontGlyphWidths.cpp libtomcrypt/sha1.c
        libtomcrypt/sha256.c libtomcrypt/sha384.c libtomcrypt/sha512.c Crypto.cpp

        ByteArray.h Config.h datafile.h ThreadDefines.h Export.h Hash.h Log.h
        LogBase.h Mutex.h MWException.h Thread.h Util.h win_macros.h
        prefix.h StringOps.h MyriadFontGlyphWidths.h libtomcrypt/tomcrypt_argchk.h
        libtomcrypt/tomcrypt_cfg.h libtomcrypt/tomcrypt_hash.h libtomcrypt/tomcrypt_macros.h Crypto.h

        PUBLIC FILE_SET HEADERS
        BASE_DIRS FILES eidErrors.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENSSL_CRYPTO_LIBRARY})
