#!/bin/bash

CURRDIR=$(pwd)
OS=`uname -s`

#openssl
if [ ! -f /usr/lib/libssl.a ] && [ ! -f /usr/lib64/libssl.a ] && [ ! -f /usr/local/lib/libssl.a ] && [ ! -f /usr/local/lib64/libssl.a ]; then
    echo "begin install openssl"
    rm -rf openssl-1.1.1n
    tar -zxvf openssl-1.1.1n.tar.gz
    cd ./openssl-1.1.1n
    ./config
    make
    sudo make install
    cd $CURRDIR
    echo "install openssl success"
fi

#boost安装
if [ ! -f "/usr/local/lib/libboost_system.a" ]; then
    echo "begin install libboost"
    rm -rf boost_1_79_0
    tar -zxvf boost_1_79_0.tar.gz
    cd ./boost_1_79_0
    ./bootstrap.sh
    ./b2
    sudo ./b2 install
    cd $CURRDIR
    echo "install libboost success"
fi

#libevent安装
if [ ! -f "/usr/local/lib/libevent_core.a" ]; then
    echo "begin install libevent"
    rm -rf libevent-2.1.12-stable
    tar -zxvf libevent-2.1.12-stable.tar.gz
    cd ./libevent-2.1.12-stable
    ./autogen.sh 
    ./configure
    make
    sudo make install
    cd $CURRDIR
    echo "install libevent success"
fi

#yum install flex
#bison安装
if [ ! -f "/usr/local/lib/liby.a" ]; then
    echo "begin install libbison"
    rm -rf bison-3.8.2
    tar -zxvf bison-3.8.2.tar.gz
    cd ./bison-3.8.2
    ./configure
    make
    sudo make install
    cd $CURRDIR
    echo "install libbison success"
fi

#thrift安装
if [ ! -f "/usr/local/lib/libthrift.a" ]; then
    echo "begin install libthrift"
    rm -rf thrift-0.16.0
    tar -zxvf thrift-0.16.0.tar.gz
    cd ./thrift-0.16.0
    mkdir builddir
    cd builddir
    cmake ..
    make 
    sudo make install
    cd $CURRDIR
    echo "install libthrift success"
fi

#gflags安装
if [ ! -f "/usr/local/lib/libgflags.a" ]; then
    echo "begin install libgflags"
    rm -rf gflags-master
    unzip gflags-master.zip
    cd gflags-master
    mkdir builddir
    cd builddir
    cmake ..
    make 
    sudo make install
    cd $CURRDIR
    echo "install libgflags success"
fi

#glog安装
#cd glog-0.6.0 && diff -uN src/logging.cc ../logging.cc > logging.cc.patch
#tar -zcvf glog-0.6.0-patch.tar.gz logging.cc.patch
if [ ! -f "/usr/local/lib/libglog.a" ]; then
    echo "begin install libglog"
    rm -rf glog-0.6.0
    tar -zxvf glog-0.6.0-patch.tar.gz
    tar -zxvf glog-0.6.0.tar.gz
    cd glog-0.6.0
    patch -p0 < ../logging.cc.patch 
    if [ ${OS} == "Darwin" ]; then
        sed -i '.bak' 's/option (BUILD_SHARED_LIBS "Build shared libraries" ON)/option (BUILD_SHARED_LIBS "Build shared libraries" OFF)/g' CMakeLists.txt
        sed -i '.bak' 's/option (WITH_GTEST "Use Google Test" ON)/option (WITH_GTEST "Use Google Test" OFF)/g' CMakeLists.txt
    else
        sed -i 's/option (BUILD_SHARED_LIBS "Build shared libraries" ON)/option (BUILD_SHARED_LIBS "Build shared libraries" OFF)/g' CMakeLists.txt
        sed -i 's/option (WITH_GTEST "Use Google Test" ON)/option (WITH_GTEST "Use Google Test" OFF)/g' CMakeLists.txt
    fi
    mkdir build
    cd build
    cmake ..
    make 
    sudo make install
    cd $CURRDIR
    echo "install libglog success"
fi

#yaml-cpp安装
if [ ! -f "/usr/local/lib/libyaml-cpp.a" ]; then
    echo "begin install libyaml-cpp"
    rm -rf yaml-cpp-yaml-cpp-0.7.0
    tar -zxvf yaml-cpp-yaml-cpp-0.7.0.tar.gz
    cd yaml-cpp-yaml-cpp-0.7.0
    mkdir build
    cd build
    cmake ..
    make 
    sudo make install
    cd $CURRDIR
    echo "install libyaml-cpp success"
fi

#mpich安装
#yum remove libibverbs-devel libibverbs-utils librdmacm librdmacm-devel librdmacm-utils libibverbs
#apt-get remove libibverbs-dev libibverbs-utils librdmacm librdmacm-dev librdmacm-utils libibverbs
if [ ! -f "/usr/local/lib/libmpi.a" ]; then
    echo "begin install libmpi"
    rm -rf mpich-4.0
    tar -zxvf mpich-4.0.tar.gz
    cd ./mpich-4.0 
    cd ./modules/hwloc
    ./autogen.sh
    ./configure --disable-libudev --disable-libxml2 --enable-shared=yes --enable-static=yes
    make -j4
    sudo make install
    cd ../../
    ./autogen.sh 
    ./configure --disable-fortran --enable-shared=yes --enable-static=yes --with-hwloc-include=/usr/local/include --with-hwloc-lib=/usr/local/lib --with-hwloc=/usr/local
    make -j4
    sudo make install
    cd $CURRDIR
    echo "install libmpi success"
fi

#QuEST安装
echo "begin install QuEST"
rm -rf QuEST-3.5.0
tar -zxvf QuEST-3.5.0.tar.gz
cd QuEST-3.5.0/QuEST
if [ ${OS} == "Darwin" ]; then
    sed -i '.bak' 's/option(DISTRIBUTED "Whether the program will be run in distributed mode using MPI. Set to 1 to enable" 0)/option(DISTRIBUTED "Whether the program will be run in distributed mode using MPI. Set to 1 to enable" 1)/g' CMakeLists.txt
    sed -i '.bak' 's/set(BUILD_SHARED_LIBS TRUE CACHE BOOL "Whether to build a dynamic library")/set(BUILD_SHARED_LIBS FALSE CACHE BOOL "Whether to build a dynamic library")/g' CMakeLists.txt
    sed -i 's/find_package(MPI REQUIRED)/#find_package(MPI REQUIRED)/g' CMakeLists.txt
else
    sed -i 's/option(DISTRIBUTED "Whether the program will be run in distributed mode using MPI. Set to 1 to enable" 0)/option(DISTRIBUTED "Whether the program will be run in distributed mode using MPI. Set to 1 to enable" 1)/g' CMakeLists.txt
    sed -i 's/set(BUILD_SHARED_LIBS TRUE CACHE BOOL "Whether to build a dynamic library")/set(BUILD_SHARED_LIBS FALSE CACHE BOOL "Whether to build a dynamic library")/g' CMakeLists.txt
    sed -i 's/find_package(MPI REQUIRED)/#find_package(MPI REQUIRED)/g' CMakeLists.txt
fi
echo 'install(FILES include/QuEST_complex.h include/QuEST.h include/QuEST_precision.h DESTINATION include)' >> CMakeLists.txt
echo 'install(TARGETS QuEST LIBRARY DESTINATION lib)' >> CMakeLists.txt
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=../../../../ ..
make
sudo make install
cd $CURRDIR
echo "install QuEST success"

#googletest安装
if [ ! -f "/usr/local/lib/libgtest.a" ]; then
    echo "begin install libgtest"
    rm -rf googletest-release-1.10.0
    tar -zxvf googletest-release-1.10.0.tar.gz
    cd ./googletest-release-1.10.0
    mkdir build
    cd build
    cmake ..
    make 
    sudo make install
    cd $CURRDIR
    echo "install libgtest success"
fi
