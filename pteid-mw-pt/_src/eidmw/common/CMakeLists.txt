project(${PTEID_COMMON} VERSION 1.0 LANGUAGES CXX)

find_package(OpenSSL REQUIRED)

include_directories(${OPENSSL_INCLUDE_DIR})

add_library(${PROJECT_NAME} SHARED
        ${PROJECT_NAME}.rc
        Config.cpp
        ByteArray.cpp
        ConfigCommon.cpp
        datafile.cpp
        Log.cpp
        LogBase.cpp
        Mutex.cpp
        MWException.cpp
        Thread.cpp
        Util.cpp
        StringOps.cpp
        MyriadFontGlyphWidths.cpp
        Crypto.cpp
        ByteArray.h
        Config.h
        datafile.h
        ThreadDefines.h
        Export.h
        Log.h
        LogBase.h
        Mutex.h
        MWException.h
        Thread.h
        Util.h
        win_macros.h
        prefix.h
        StringOps.h
        MyriadFontGlyphWidths.h
        Crypto.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENSSL_CRYPTO_LIBRARY})

install(FILES eidErrors.h
        DESTINATION include
)
