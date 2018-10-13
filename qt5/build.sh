

# https://blog.csdn.net/KOdeCSDN/article/details/80170325
# https://www.cnblogs.com/plmmlp09/p/4434343.html
# https://blog.csdn.net/Cherry_Lover/article/details/80323497



./configure -v -recheck-all \
-prefix $HOME/opt/qt-5.11.1-arm-hisiv500-linux-uclibcgnueabi \
-release \
-opensource -confirm-license \
-make libs \
-xplatform linux-arm-hisiv500-uclibcgnueabi-g++ \
-optimized-qmake \
-linuxfb    \
-no-pch \
-no-evdev   \
-no-accessibility   \
-no-tslib \
-no-feature-qt3d-render \
-no-feature-qt3d-input  \
-no-feature-qt3d-logic  \
-no-feature-qt3d-opengl-renderer \
-no-feature-socketcan   \
-no-avx \
-no-dbus    \
-no-sql-sqlite \
-no-opengl \
-no-sse2 \
-no-openssl \
-no-cups \
-no-glib \
-no-pkg-config \
-no-separate-debug-info


# ./configure-prefix /home/chenhui/qt-5.5.1 -release -opensource -confirm-license 
# -static-qt-zlib -qt-libpng -qt-libjpeg -qt-freetype 
# -no-pch -no-avx -no-openssl-no-cups -no-dbus
# -platform linux-g++ -xplatform linux-hisiv600-linux-g++-no-opengl -no-glib

