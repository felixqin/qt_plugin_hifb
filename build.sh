

cd `dirname $0`
mkdir -p cmake-build-shell
cd cmake-build-shell
cmake ..
make -j`nproc` qt5_sdk

