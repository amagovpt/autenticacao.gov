project(${PTEID_DIALOGSQT} VERSION 1.0 LANGUAGES CXX)

include_directories(../../common)

add_compile_definitions(fno-strict-aliasing;)
set(sources ../dialogs.h ../langUtil.h ../language.h SharedMem.h dlgs.cpp ../langUtil.cpp SharedMem.cpp)

add_library(${PROJECT_NAME} SHARED ${sources})

target_link_libraries(${PROJECT_NAME} PRIVATE ${PTEID_COMMON})
