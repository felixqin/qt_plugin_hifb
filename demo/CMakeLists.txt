
set(EXECUTABLE_OUTPUT_PATH ${PRODUCT_OUTDIR}/bin)


set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt5Widgets)


qt5_wrap_ui(UI_FILES mainwindow.ui)

file(GLOB_RECURSE HEADER_FILES "*.h")
AUX_SOURCE_DIRECTORY(. SRC_FILES)
add_executable(demo ${UI_FILES} ${SRC_FILES} ${HEADER_FILES})

target_link_libraries(demo
        Qt5::Widgets
        )


file(COPY
        ${_qt5Core_install_prefix}/lib/libQt5Widgets.so.5
        ${_qt5Core_install_prefix}/lib/libQt5Widgets.so.5.11.2
        ${_qt5Core_install_prefix}/lib/libQt5Gui.so.5
        ${_qt5Core_install_prefix}/lib/libQt5Gui.so.5.11.2
        ${_qt5Core_install_prefix}/lib/libQt5Core.so.5
        ${_qt5Core_install_prefix}/lib/libQt5Core.so.5.11.2
        DESTINATION
        ${PRODUCT_OUTDIR}/bin
        NO_SOURCE_PERMISSIONS
        )

file(COPY
        ${_qt5Core_install_prefix}/plugins/platforms/libqlinuxfb.so
        DESTINATION
        ${PRODUCT_OUTDIR}/bin/platforms
        NO_SOURCE_PERMISSIONS
        )

