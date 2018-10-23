


add_custom_target(qt5_download
        sh ./download.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "download qt5 source code"
        SOURCES download.sh
        )


add_custom_target(qt5_sdk
        sh ./build_hisiv500.sh
        DEPENDS hifb qt5_download
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "build qt5 sdk"
        SOURCES build_hisiv500.sh
        )

