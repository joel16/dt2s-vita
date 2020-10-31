cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VITASDK})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
    else()
        message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
    endif()
endif()

project(dt2s)
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-q -Wall -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions")

add_executable(${PROJECT_NAME}
    source/dt2s.c
)

target_link_libraries(${PROJECT_NAME}
    SceCtrl_stub
    SceKernelThreadMgr_stub
    SceLibKernel_stub
    ScePower_stub
    SceTouch_stub
    taihen_stub
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES LINK_FLAGS "-nostdlib"
)

vita_create_self(${PROJECT_NAME}.suprx ${PROJECT_NAME} CONFIG ${PROJECT_NAME}.yml UNSAFE)

add_executable(${PROJECT_NAME}_statusbar
    source/dt2s.c
)

target_link_libraries(${PROJECT_NAME}_statusbar
    SceCtrl_stub
    SceKernelThreadMgr_stub
    SceLibKernel_stub
    ScePower_stub
    SceTouch_stub
    taihen_stub
)

set_target_properties(${PROJECT_NAME}_statusbar
    PROPERTIES LINK_FLAGS "-nostdlib"
    COMPILE_DEFINITIONS "DT2S_STATUS_BAR"
)

vita_create_self(${PROJECT_NAME}_statusbar.suprx ${PROJECT_NAME}_statusbar CONFIG ${PROJECT_NAME}.yml UNSAFE)
