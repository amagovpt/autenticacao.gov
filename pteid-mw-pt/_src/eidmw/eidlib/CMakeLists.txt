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

target_link_libraries(${PROJECT_NAME} PRIVATE ${PTEID_COMMON} ${PTEID_APPLAYER} ${PTEID_CMDSERVICES})

install(FILES CardCallbacks.h eidlibdefines.h eidlibException.h eidlib.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
