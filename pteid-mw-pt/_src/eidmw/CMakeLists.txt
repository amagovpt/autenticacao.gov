cmake_minimum_required(VERSION 3.16)

set(PTEID_PREFIX "pteid")

set(CMAKE_CXX_STANDARD 17)

set(LIB_ROOT_FOLDER "$ENV{HOME}/mw-thirdparty-libs" CACHE PATH "Root folder for third-party libraries")
set(LIB_POPPLER_ROOT "${LIB_ROOT_FOLDER}/poppler-24.10")
set(CURL_ROOT "${LIB_ROOT_FOLDER}/libcurl")
set(PKG_CONFIG_PATH "/usr/lib/x86_64-linux-gnu/pkgconfig")

message("Dependencies path is: ${LIB_ROOT_FOLDER}")

set(project_name_target ${PTEID_PREFIX}-mw)
project(${project_name_target} VERSION 3.12.0)

execute_process(
	    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
		COMMAND git rev-list --count HEAD
		OUTPUT_VARIABLE REVISION_NUM
		OUTPUT_STRIP_TRAILING_WHITESPACE
)

message("Using git Revision count: ${REVISION_NUM}") #check and looks further on the versioning in _build pteidversions to delete some unused vars and define others here!!!

set(SDK_PTEID_PRODUCT_NAME_STRING "Portuguese eID Middleware SDK")
set(SDK_PTEID_PRODUCT_VERSION_STRING "Version: ${CMAKE_PROJECT_VERSION}")

set(PTEID_COMMON "${PTEID_PREFIX}common")
set(PTEID_CARDLAYER "${PTEID_PREFIX}cardlayer")
set(PTEID_APPLAYER "${PTEID_PREFIX}applayer")
set(PTEID_LIB "${PTEID_PREFIX}lib")
set(PTEID_POPPLER "${PTEID_PREFIX}-poppler")
set(PTEID_CMDSERVICES "cmdServices")
set(PTEID_DIALOGSQT "${PTEID_PREFIX}dialogsQT")

option(PTEID_MULTIPASS_SDK "Enable Multipass mini SDK" OFF)

add_subdirectory(common)
add_subdirectory(cardlayer)
add_subdirectory(applayer)
add_subdirectory(eidlib)
add_subdirectory(dialogs/dialogsQT)


install(TARGETS ${PTEID_COMMON} DESTINATION lib FILE_SET HEADERS DESTINATION include)
install(TARGETS ${PTEID_CARDLAYER} DESTINATION lib)
install(TARGETS ${PTEID_APPLAYER} DESTINATION lib)
install(TARGETS ${PTEID_LIB} DESTINATION lib FILE_SET HEADERS DESTINATION include)
install(TARGETS ${PTEID_DIALOGSQT} DESTINATION lib)

if (NOT PTEID_MULTIPASS_SDK)
	add_subdirectory(pteid-poppler)
	add_subdirectory(CMD/services)
	install(TARGETS ${PTEID_POPPLER} DESTINATION lib)
	install(TARGETS ${PTEID_CMDSERVICES} DESTINATION lib)
endif()


install(DIRECTORY ${CMAKE_SOURCE_DIR}/misc/certs/ DESTINATION /usr/local/share/certs)
