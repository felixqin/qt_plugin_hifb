
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../lib/${PRODUCT_HICHIP})


include_directories(
        ${PRODUCT_HIMPPDIR}/include
        common
)

aux_source_directory(./ SRC_FILES)
aux_source_directory(common SRC_FILES)
add_library(hifb ${SRC_FILES})


set(LIB_HIFB_SINGLE libhifb_single.a)
add_custom_command(OUTPUT ${LIB_HIFB_SINGLE}
        COMMAND ${CMAKE_AR} x ${PRODUCT_HIMPPDIR}/lib/libmpi.a
        COMMAND ${CMAKE_AR} x ${PRODUCT_HIMPPDIR}/lib/libupvqe.a
        COMMAND ${CMAKE_AR} x ${PRODUCT_HIMPPDIR}/lib/libdnvqe.a
        COMMAND ${CMAKE_AR} x ${PRODUCT_HIMPPDIR}/lib/libVoiceEngine.a
        COMMAND ${CMAKE_AR} cru ${LIB_HIFB_SINGLE} *.o
        COMMAND mv ${LIB_HIFB_SINGLE} ${LIBRARY_OUTPUT_PATH}
        )

add_custom_target(hifb_single ALL DEPENDS ${LIB_HIFB_SINGLE})

