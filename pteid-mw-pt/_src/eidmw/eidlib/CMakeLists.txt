project(${PTEID_LIB} VERSION 1.0 LANGUAGES CXX)

find_package(OpenSSL REQUIRED)

include_directories(.)
include_directories(../CMD/services)
include_directories(../common)
include_directories(../applayer)
include_directories(../cardlayer)
include_directories(../dialogs)

add_library(${PROJECT_NAME} SHARED
    ${PTEID_LIB}.rc
    eidlibCmdClient.cpp
    eidlibCard.cpp
    eidlibCrypto.cpp
    eidlibDoc.cpp
    eidlibException.cpp
    eidlibReader.cpp
    eidlibSignDevFactory.cpp
    eidlibxades.cpp
    InternalUtil.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${PTEID_COMMON} ${PTEID_APPLAYER})

target_include_directories(${PROJECT_NAME} PRIVATE ${OPENSSL_INCLUDE_DIR})

if (PTEID_MULTIPASS_SDK)
    add_compile_definitions(${PROJECT_NAME} PTEID_MULTIPASS_SDK)
    target_compile_options(${PROJECT_NAME} PRIVATE -fvisibility=hidden -ffunction-sections -fdata-sections)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE ${PTEID_COMMON} ${PTEID_CMDSERVICES})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${PTEID_COMMON} ${PTEID_APPLAYER})

target_link_options(${PROJECT_NAME} PRIVATE -Wl,--gc-sections)

install(FILES CardCallbacks.h eidlibdefines.h eidlibException.h eidlib.h
        DESTINATION include)
