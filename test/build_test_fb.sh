
cd `dirname $0`

mkdir -p ../out

arm-hisiv500-linux-uclibcgnueabi-gcc -o ../out/test_fb test_fb.cpp
sudo xcp -v ../out/test_fb /share/charles/test/


