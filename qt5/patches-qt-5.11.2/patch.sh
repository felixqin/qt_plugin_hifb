

SRCDIR=qt-everywhere-src-5.11.2


cd `dirname $0`

cp -av linux-arm-hisiv500-uclibcgnueabi-g++ ../${SRCDIR}/qtbase/mkspecs
cp -v ExecutableAllocator.h ../${SRCDIR}/qtdeclarative/src/3rdparty/masm/stubs/ExecutableAllocator.h
cp -v qhash.cpp ../${SRCDIR}/qtbase/src/corelib/tools/qhash.cpp

