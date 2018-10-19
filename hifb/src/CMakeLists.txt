
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../lib/${PRODUCT_HICHIP})


include_directories(
        ${PRODUCT_HIMPPDIR}/include
        common
)

aux_source_directory(./ SRC_FILES)
aux_source_directory(common SRC_FILES)
add_library(hifb ${SRC_FILES})

