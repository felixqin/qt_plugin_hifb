

# https://blog.csdn.net/KOdeCSDN/article/details/80170325
# https://www.cnblogs.com/plmmlp09/p/4434343.html
# https://blog.csdn.net/Cherry_Lover/article/details/80323497


cd `dirname $0`
THISDIR=`pwd`
TOPDIR=${THISDIR}/../
OUTDIR=${TOPDIR}/out/


SRCDIR=qt-everywhere-src-5.11.2
PATCHDIR=patches-qt-5.11.2
PLATFORM=linux-arm-hisiv500-uclibcgnueabi-g++
DISTNAME=qt-5.11.2-arm-hisiv500-linux-uclibcgnueabi
HICHIP=HI3519_V101


if [ ! -d ${SRCDIR} ]; then
    echo "please download qt5 source code!"
    exit 1
fi


echo "patch ${PATCHDIR} ..."
${PATCHDIR}/patch.sh || exit 1


cd ${SRCDIR}

./configure -v -recheck-all \
-prefix ${OUTDIR}/${DISTNAME} \
-release \
-opensource -confirm-license \
-make libs \
-xplatform ${PLATFORM} \
-optimized-qmake \
-qpa linuxfb    \
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
-no-separate-debug-info \
-I${TOPDIR}/hifb/include -L${TOPDIR}/hifb/lib/${HICHIP} \
|| exit 2


# ./configure -prefix /home/chenhui/qt-5.5.1 -release -opensource -confirm-license 
# -static-qt-zlib -qt-libpng -qt-libjpeg -qt-freetype 
# -no-pch -no-avx -no-openssl-no-cups -no-dbus
# -platform linux-g++ -xplatform linux-hisiv600-linux-g++-no-opengl -no-glib


gmake -j`nproc` || exit 3
gmake install || exit 4

