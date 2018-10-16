
cd `dirname $0`


SRCPACKAGE=qt-everywhere-src-5.11.2.tar.xz
SRCDIR=qt-everywhere-src-5.11.2


if [ ! -f ${SRCPACKAGE} ]; then
    wget http://download.qt.io/archive/qt/5.11/5.11.2/single/qt-everywhere-src-5.11.2.tar.xz
fi

if [ ! -d ${SRCDIR} ]; then
    tar xvf ${SRCPACKAGE}
fi

