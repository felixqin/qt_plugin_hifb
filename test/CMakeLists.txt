
set(EXECUTABLE_OUTPUT_PATH ${PRODUCT_OUTDIR})


include_directories(
        ../hifb/include
)

link_directories(
        ${PRODUCT_HIMPPDIR}/lib
)

add_executable(test_hifb test_hifb.cpp)

target_link_libraries(test_hifb
        hifb
        pthread rt dl
        )
