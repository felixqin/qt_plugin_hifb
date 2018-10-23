
cmake_minimum_required(VERSION 2.8.11)


set(PRODUCT_TOOLCHAIN arm-hisiv500-linux-uclibcgnueabi)
set(PRODUCT_HICHIP HI3519_V101)
set(PRODUCT_TOPDIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PRODUCT_OUTDIR ${CMAKE_CURRENT_SOURCE_DIR}/out/${PRODUCT_HICHIP})
set(PRODUCT_HIMPPDIR ${PRODUCT_TOPDIR}/mpp/${PRODUCT_HICHIP})


# path to compiler and utilities
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# specify the cross compiler
set(CMAKE_C_COMPILER ${PRODUCT_TOOLCHAIN}-gcc)
set(CMAKE_CXX_COMPILER ${PRODUCT_TOOLCHAIN}-g++)
set(CMAKE_STRIP ${PRODUCT_TOOLCHAIN}-strip)

# Name of the target platform
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Version of the system
set(CMAKE_SYSTEM_VERSION 1)


add_definitions(-Wall -O2 -Os)
add_definitions(-Wno-unused-local-typedefs)
add_definitions(-Wstrict-aliasing -Wwrite-strings)
add_definitions(-D_REENTRANT -msoft-float)


project(qt-hifb)



SET(QT_MISSING True)
set(CMAKE_PREFIX_PATH "${PRODUCT_TOPDIR}/out/qt-5.11.2-arm-hisiv500-linux-uclibcgnueabi")


add_subdirectory(hifb)
add_subdirectory(test)
add_subdirectory(qt5)
add_subdirectory(demo)

