project(${PTEID_LIB} VERSION 1.0 LANGUAGES CXX)

find_package(OpenSSL REQUIRED)

include_directories(.)
include_directories(../CMD/services)
include_directories(../common)
include_directories(../applayer)
include_directories(../cardlayer)
include_directories(../dialogs)

set(sources eidlibCmdClient.cpp eidlibCard.cpp eidlibCrypto.cpp eidlibDoc.cpp eidlibException.cpp 
            eidlibReader.cpp eidlibSignDevFactory.cpp eidlibxades.cpp InternalUtil.cpp 
            eidlibdefines.h eidlibException.h InternalUtil.h)


add_library(${PROJECT_NAME} SHARED ${sources} ${PTEID_LIB}.rc)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PTEID_COMMON} ${PTEID_APPLAYER} ${PTEID_CMDSERVICES})
