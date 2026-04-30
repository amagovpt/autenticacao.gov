cmake_minimum_required(VERSION 3.16)
project(cc_reader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../pteid-mw-pt/_src/eidmw" CACHE PATH "Path to middleware SDK root")
set(SDK_INCLUDE "${SDK_ROOT}/eidlib" CACHE PATH "Path to middleware public headers")
set(SDK_LIB "${SDK_ROOT}/lib" CACHE PATH "Path to middleware shared libraries")

add_executable(cc_reader
    main.cpp
    public_info.cpp
    address_info.cpp
)

add_executable(doc_signer
    sign_documents.cpp
)

target_include_directories(cc_reader PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${SDK_INCLUDE}"
    "${SDK_ROOT}/common"
    "${SDK_ROOT}/cardlayer"
    "${SDK_ROOT}/applayer"
    "${SDK_ROOT}/CMD/services"
    "${SDK_ROOT}/dialogs"
)

target_include_directories(doc_signer PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${SDK_INCLUDE}"
    "${SDK_ROOT}/common"
    "${SDK_ROOT}/cardlayer"
    "${SDK_ROOT}/applayer"
    "${SDK_ROOT}/CMD/services"
    "${SDK_ROOT}/dialogs"
)

target_link_directories(cc_reader PRIVATE "${SDK_LIB}")
target_link_directories(doc_signer PRIVATE "${SDK_LIB}")

target_link_libraries(cc_reader PRIVATE
    pteidlib
    pteidapplayer
    pteidcardlayer
    pteidcommon
    pteiddialogsQT
    CMDServices
    pthread
    dl
)

target_link_libraries(doc_signer PRIVATE
    pteidlib
    pteidapplayer
    pteidcardlayer
    pteidcommon
    pteiddialogsQT
    CMDServices
    pthread
    dl
)

# Keep middleware dependencies in NEEDED and use executable-relative RPATH.
target_link_options(cc_reader PRIVATE "-Wl,--no-as-needed" "-Wl,--disable-new-dtags")
target_link_options(doc_signer PRIVATE "-Wl,--no-as-needed" "-Wl,--disable-new-dtags")
set_target_properties(cc_reader PROPERTIES
    BUILD_RPATH "\$ORIGIN/../pteid-mw-pt/_src/eidmw/lib"
    INSTALL_RPATH "\$ORIGIN/../pteid-mw-pt/_src/eidmw/lib"
)
set_target_properties(doc_signer PROPERTIES
    BUILD_RPATH "\$ORIGIN/../pteid-mw-pt/_src/eidmw/lib"
    INSTALL_RPATH "\$ORIGIN/../pteid-mw-pt/_src/eidmw/lib"
)
