#!/bin/bash

set -e
CURRDIR=$(pwd)
OS=`uname -s`
OSMachine=`uname -m`

if [ ${OS} == "Darwin" ]; then
    if [ ! -f "/usr/local/bin/brew" ]; then
        /bin/zsh -c "$(curl -fsSL https://gitee.com/cunkai/HomebrewCN/raw/master/Homebrew.sh)"
    fi
    brew install help2man
    brew install xz
    brew install llvm 
    brew install libomp
fi

#openssl
if [ ! -f /usr/lib/libssl.a ] && [ ! -f /usr/lib64/libssl.a ] && [ ! -f /usr/local/lib/libssl.a ] && [ ! -f /usr/local/lib64/libssl.a ]; then
    echo "begin install openssl"
    rm -rf openssl-1.1.1n
    tar -zxvf openssl-1.1.1n.tar.gz
    cd ./openssl-1.1.1n
    ./config
    make
    if [ $? -ne "0" ]; then
        echo "make failed!!! please Check error"
        exit
    fi
    make install
    if [ $? -ne "0" ]; then
        echo "make install failed!!! please Check error"
        exit
    fi
    cd $CURRDIR
    echo "install openssl success"
fi

#cmake
if [ ! -f "/usr/bin/cmake" ] && [ ! -f "/usr/local/bin/cmake" ]; then
    echo "begin install cmake"
	rm -rf cmake-3.23.1
    tar -zxvf cmake-3.23.1.tar.gz
    cd cmake-3.23.1
    ./configure
    make
    if [ $? -ne "0" ]; then
        echo "make failed!!! please Check error"
        exit
    fi
    make install
    if [ $? -ne "0" ]; then
        echo "make install failed!!! please Check error"
        exit
    fi
    cd $CURRDIR
    echo "install cmake success"
fi

#m4
if [ ! -f "/usr/share/info/m4.info" ] && [ ! -f "/usr/local/share/info/m4.info" ]; then
    echo "begin install m4"
	rm -rf m4-1.4.19
    tar -zxvf m4-latest.tar.gz
    cd m4-1.4.19
    ./configure
    make
    if [ $? -ne "0" ]; then
        echo "make failed!!! please Check error"
        exit
    fi
    make install
    if [ $? -ne "0" ]; then
        echo "make install failed!!! please Check error"
        exit
    fi
    cd $CURRDIR
    echo "install m4 success"
fi
 
#autoconf
if [ ! -f "/usr/bin/autoconf" ] && [ ! -f "/usr/local/bin/autoconf" ]; then
    echo "begin install autoconf"
	rm -rf autoconf-2.71
    tar -zxvf autoconf-2.71.tar.gz 
    cd autoconf-2.71
    ./configure
    make
    if [ $? -ne "0" ]; then
        echo "make failed!!! please Check error"
        exit
    fi
    make install
    if [ $? -ne "0" ]; then
        echo "make install failed!!! please Check error"
        exit
    fi
    cd $CURRDIR
    echo "install autoconf success"
fi

#automake
if [ ! -f "/usr/bin/automake" ] && [ ! -f "/usr/local/bin/automake" ]; then
    echo "begin install automake"
	rm -rf automake-1.16.5
    tar -zxvf automake-1.16.5.tar.gz  
    cd automake-1.16.5
    ./configure
    make
    if [ $? -ne "0" ]; then
        echo "make failed!!! please Check error"
        exit
    fi
    make install
    if [ $? -ne "0" ]; then
        echo "make install failed!!! please Check error"
        exit
    fi
    cd $CURRDIR
    echo "install automake success"
fi 

#libtool
if [ ! -f "/usr/bin/libtoolize" ] && [ ! -f "/usr/local/bin/libtoolize" ]; then
    echo "begin install libtool"
	rm -rf libtool-2.4.7
    tar -zxvf libtool-2.4.7.tar.gz 
    cd libtool-2.4.7
    ./configure
    make
    if [ $? -ne "0" ]; then
        echo "make failed!!! please Check error"
        exit
    fi
    make install
    if [ $? -ne "0" ]; then
        echo "make install failed!!! please Check error"
        exit
    fi
    cd $CURRDIR
    echo "install libtool success"
fi 

#boost安装
if [ ! -f "/usr/local/lib/libboost_system.a" ]; then
    echo "begin install libboost"
    rm -rf boost_1_79_0
    tar -zxvf boost_1_79_0.tar.gz
    cd ./boost_1_79_0
    ./bootstrap.sh
    ./b2
    ./b2 install
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
    if [ $? -ne "0" ]; then
        echo "make failed!!! please Check error"
        exit
    fi
    make install
    if [ $? -ne "0" ]; then
        echo "make install failed!!! please Check error"
        exit
    fi
    cd $CURRDIR
    echo "install libevent success"
fi

#bison安装
if [ ! -f "/usr/local/lib/liby.a" ]; then
    echo "begin install libbison"
    rm -rf bison-3.8.2
    tar -zxvf bison-3.8.2.tar.gz
    cd ./bison-3.8.2
    ./configure
    make
    if [ $? -ne "0" ]; then
        echo "make failed!!! please Check error"
        exit
    fi
    make install
    if [ $? -ne "0" ]; then
        echo "make install failed!!! please Check error"
        exit
    fi
    cd $CURRDIR
    echo "install libbison success"
fi

#flex安装
if [ ! -f "/usr/local/lib/libfl.a" ]; then
    echo "begin install flex"
    rm -rf flex-2.6.4
    tar -zxvf flex-2.6.4.tar.gz 
    cd flex-2.6.4/
    ./configure
    make
    if [ $? -ne "0" ]; then
        echo "make failed!!! please Check error"
        exit
    fi
    make install
    if [ $? -ne "0" ]; then
        echo "make install failed!!! please Check error"
        exit
    fi
    cd $CURRDIR
    echo "install flex success"
fi

#thrift安装
if [ ! -f "/usr/local/lib/libthrift.a" ]; then
    echo "begin install libthrift"
    rm -rf thrift-0.16.0
    tar -zxvf thrift-0.16.0.tar.gz
    cd ./thrift-0.16.0
    if [ ${OS} == "Darwin" ]; then
        sed -i '' 's/if(BUILD_JAVA)/if(false)/g' CMakeLists.txt
        sed -i '' 's/if(BUILD_JAVASCRIPT)/if(false)/g' CMakeLists.txt
        sed -i '' 's/if(BUILD_NODEJS)/if(false)/g' CMakeLists.txt
    else
        sed -i 's/if(BUILD_JAVA)/if(false)/g' CMakeLists.txt
        sed -i 's/if(BUILD_JAVASCRIPT)/if(false)/g' CMakeLists.txt
        sed -i 's/if(BUILD_NODEJS)/if(false)/g' CMakeLists.txt
    fi
    mkdir builddir
    cd builddir
    cmake ..
    make
    if [ $? -ne "0" ]; then
        echo "make failed!!! please Check error"
        exit
    fi
    make install
    if [ $? -ne "0" ]; then
        echo "make install failed!!! please Check error"
        exit
    fi
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
    if [ $? -ne "0" ]; then
        echo "make failed!!! please Check error"
        exit
    fi
    make install
    if [ $? -ne "0" ]; then
        echo "make install failed!!! please Check error"
        exit
    fi
    cd $CURRDIR
    echo "install libgflags success"
fi

#glog安装
#cd glog-0.6.0 && diff -uN src/logging.cc ../logging.cc > logging.cc.patch
#tar -zcvf glog-0.6.0-patch.tar.gz logging.cc.patch
if [ ! -f "/usr/local/lib/libglog.a" ] && [ ! -f "/usr/local/lib64/libglog.a" ]; then
    echo "begin install libglog"
    rm -rf glog-0.6.0
    tar -zxvf glog-0.6.0-patch.tar.gz
    tar -zxvf glog-0.6.0.tar.gz
    cd glog-0.6.0
    patch -p0 < ../logging.cc.patch 
    if [ ${OS} == "Darwin" ]; then
        sed -i '' 's/option (BUILD_SHARED_LIBS "Build shared libraries" ON)/option (BUILD_SHARED_LIBS "Build shared libraries" OFF)/g' CMakeLists.txt
        sed -i '' 's/option (WITH_GTEST "Use Google Test" ON)/option (WITH_GTEST "Use Google Test" OFF)/g' CMakeLists.txt
    else
        sed -i 's/option (BUILD_SHARED_LIBS "Build shared libraries" ON)/option (BUILD_SHARED_LIBS "Build shared libraries" OFF)/g' CMakeLists.txt
        sed -i 's/option (WITH_GTEST "Use Google Test" ON)/option (WITH_GTEST "Use Google Test" OFF)/g' CMakeLists.txt
    fi
    mkdir build
    cd build
    cmake ..
    make
    if [ $? -ne "0" ]; then
        echo "make failed!!! please Check error"
        exit
    fi
    make install
    if [ $? -ne "0" ]; then
        echo "make install failed!!! please Check error"
        exit
    fi
    cd $CURRDIR
    echo "install libglog success"
fi

#yaml-cpp安装
if [ ! -f "/usr/local/lib/libyaml-cpp.a" ] && [ ! -f "/usr/local/lib64/libyaml-cpp.a" ]; then
    echo "begin install libyaml-cpp"
    rm -rf yaml-cpp-yaml-cpp-0.7.0
    tar -zxvf yaml-cpp-yaml-cpp-0.7.0.tar.gz
    cd yaml-cpp-yaml-cpp-0.7.0
    if [ ${OS} == "Darwin" ]; then
        sed -i '' 's/include(CTest)/#include(CTest)/g' CMakeLists.txt
    else
        sed -i 's/include(CTest)/#include(CTest)/g' CMakeLists.txt
    fi
    mkdir build
    cd build
    cmake ..
    make
    if [ $? -ne "0" ]; then
        echo "make failed!!! please Check error"
        exit
    fi
    make install
    if [ $? -ne "0" ]; then
        echo "make install failed!!! please Check error"
        exit
    fi
    cd $CURRDIR
    echo "install libyaml-cpp success"
fi

#mpich安装
#if [ -f "/usr/bin/yum" ]; then
#    yum remove -y libibverbs-devel libibverbs-utils librdmacm librdmacm-devel librdmacm-utils libibverbs
#elif [ -f "/usr/bin/apt-get" ]; then
#    apt-get remove -y libibverbs-dev libibverbs-utils librdmacm librdmacm-dev librdmacm-utils libibverbs
#fi
if [ ! -f "/usr/local/lib/libhwloc.a" ]; then
    echo "begin install libhwloc"
    rm -rf mpich-4.0
    tar -zxvf mpich-4.0.tar.gz
    cd ./mpich-4.0 
    cd ./modules/hwloc
    ./autogen.sh
    ./configure --libdir=/usr/local/lib --includedir=/usr/local/include --disable-libudev --disable-libxml2 --enable-shared=yes --enable-static=yes
    make -j4
    if [ $? -ne "0" ]; then
        echo "make failed!!! please Check error"
        exit
    fi
    make install
    if [ $? -ne "0" ]; then
        echo "make install failed!!! please Check error"
        exit
    fi
    cd $CURRDIR
    echo "install libhwloc success"
fi

if [ ! -f "/usr/local/lib/libmpi.a" ]; then
    echo "begin install libmpi"
    if [ ! -d "mpich-4.0" ]; then
        tar -zxvf mpich-4.0.tar.gz
    fi
    cd ./mpich-4.0 
    ./autogen.sh 
    ./configure --libdir=/usr/local/lib --includedir=/usr/local/include --disable-fortran --enable-shared=yes --enable-static=yes --with-hwloc-include=/usr/local/include --with-hwloc-lib=/usr/local/lib --with-hwloc=/usr/local
    make -j4
    if [ $? -ne "0" ]; then
        echo "make failed!!! please Check error"
        exit
    fi
    make install
    if [ $? -ne "0" ]; then
        echo "make install failed!!! please Check error"
        exit
    fi
    cd $CURRDIR
    echo "install libmpi success"
fi
#if [ -f "/usr/bin/yum" ]; then
#    yum install -y buildah cockpit-podman firewalld iproute-tc iptables-nft nmap-ncat podman tcpdump libibverbs-devel libibverbs-utils librdmacm librdmacm-devel librdmacm-utils libibverbs
#elif [ -f "/usr/bin/apt-get" ]; then
#    apt-get install -y libibverbs-dev libibverbs-utils librdmacm librdmacm-dev librdmacm-utils libibverbs
#fi
#

#QuEST安装
echo "begin install QuEST"
rm -rf QuEST-3.5.0
tar -zxvf QuEST-3.5.0.tar.gz
cd QuEST-3.5.0/QuEST
if [ ${OS} == "Darwin" ]; then
    sed -i '' 's/option(DISTRIBUTED "Whether the program will be run in distributed mode using MPI. Set to 1 to enable" 0)/option(DISTRIBUTED "Whether the program will be run in distributed mode using MPI. Set to 1 to enable" 1)/g' CMakeLists.txt
    sed -i '' 's/set(BUILD_SHARED_LIBS TRUE CACHE BOOL "Whether to build a dynamic library")/set(BUILD_SHARED_LIBS FALSE CACHE BOOL "Whether to build a dynamic library")/g' CMakeLists.txt
    sed -i '' 's/find_package(MPI REQUIRED)/#find_package(MPI REQUIRED)/g' CMakeLists.txt
else
    sed -i 's/option(DISTRIBUTED "Whether the program will be run in distributed mode using MPI. Set to 1 to enable" 0)/option(DISTRIBUTED "Whether the program will be run in distributed mode using MPI. Set to 1 to enable" 1)/g' CMakeLists.txt
    sed -i 's/set(BUILD_SHARED_LIBS TRUE CACHE BOOL "Whether to build a dynamic library")/set(BUILD_SHARED_LIBS FALSE CACHE BOOL "Whether to build a dynamic library")/g' CMakeLists.txt
    sed -i 's/find_package(MPI REQUIRED)/#find_package(MPI REQUIRED)/g' CMakeLists.txt
    if [ ${OSMachine} == "aarch64" ]; then
        sed -i 's/-mavx//g' CMakeLists.txt
    fi
fi
echo 'install(FILES include/QuEST_complex.h include/QuEST.h include/QuEST_precision.h DESTINATION include)' >> CMakeLists.txt
echo 'install(TARGETS QuEST LIBRARY DESTINATION lib)' >> CMakeLists.txt
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=../../../../ ..
make
if [ $? -ne "0" ]; then
	echo "make failed!!! please Check error"
	exit
fi
make install
if [ $? -ne "0" ]; then
	echo "make install failed!!! please Check error"
	exit
fi
cd $CURRDIR
echo "install QuEST success"

#googletest安装
if [ ! -f "/usr/local/lib/libgtest.a" ] && [ ! -f "/usr/local/lib64/libgtest.a" ]; then
    echo "begin install libgtest"
    rm -rf googletest-release-1.10.0
    tar -zxvf googletest-release-1.10.0.tar.gz
    cd ./googletest-release-1.10.0
    if [ ${OS} == "Darwin" ]; then
        sed -i '' 's/int dummy;/int dummy = 0;/g' googletest/src/gtest-death-test.cc
    else
        sed -i 's/int dummy;/int dummy = 0;/g' googletest/src/gtest-death-test.cc
    fi
    mkdir build
    cd build
    cmake ..
    make
    if [ $? -ne "0" ]; then
        echo "make failed!!! please Check error"
        exit
    fi
    make install
    if [ $? -ne "0" ]; then
        echo "make install failed!!! please Check error"
        exit
    fi
    cd $CURRDIR
    echo "install libgtest success"
fi

#libhv安装
if [ ! -f "/usr/local/lib/libhv_static.a" ]; then
    echo "begin install libhv"
    rm -rf libhv-v1.2.6
    unzip libhv-v1.2.6.zip
    cd libhv-v1.2.6
    mkdir build
    cd build
    cmake ..
    make
    if [ $? -ne "0" ]; then
        echo "make failed!!! please Check error"
        exit
    fi
    make install
    if [ $? -ne "0" ]; then
        echo "make install failed!!! please Check error"
        exit
    fi
    cd $CURRDIR
    echo "install libgtest libhv"
fi

#prometheus-cpp安装
if [ ! -f "/usr/local/lib/libprometheus-cpp-core.a" ] && [ ! -f "/usr/local/lib64/libprometheus-cpp-core.a" ]; then
    echo "begin install prometheus-cpp"
    rm -rf prometheus-cpp-1.0.1
    tar -zxvf prometheus-cpp-1.0.1.tar.gz 
    cd prometheus-cpp-1.0.1
    if [ ${OS} == "Darwin" ]; then
        sed -i '' 's/option(ENABLE_PULL "Build prometheus-cpp pull library" ON)/option(ENABLE_PULL "Build prometheus-cpp pull library" OFF)/g' CMakeLists.txt
        sed -i '' 's/option(ENABLE_PUSH "Build prometheus-cpp push library" ON)/option(ENABLE_PUSH "Build prometheus-cpp push library" OFF)/g' CMakeLists.txt
        sed -i '' 's/option(ENABLE_COMPRESSION "Enable gzip compression" ON)/option(ENABLE_COMPRESSION "Enable gzip compression" OFF)/g' CMakeLists.txt
        sed -i '' 's/option(ENABLE_TESTING "Build tests" ON)/option(ENABLE_TESTING "Build tests" OFF)/g' CMakeLists.txt
    else
        sed -i 's/option(ENABLE_PULL "Build prometheus-cpp pull library" ON)/option(ENABLE_PULL "Build prometheus-cpp pull library" OFF)/g' CMakeLists.txt
        sed -i 's/option(ENABLE_PUSH "Build prometheus-cpp push library" ON)/option(ENABLE_PUSH "Build prometheus-cpp push library" OFF)/g' CMakeLists.txt
        sed -i 's/option(ENABLE_COMPRESSION "Enable gzip compression" ON)/option(ENABLE_COMPRESSION "Enable gzip compression" OFF)/g' CMakeLists.txt
        sed -i 's/option(ENABLE_TESTING "Build tests" ON)/option(ENABLE_TESTING "Build tests" OFF)/g' CMakeLists.txt
    fi
    mkdir build
    cd build
    cmake ..
    make
    if [ $? -ne "0" ]; then
        echo "make failed!!! please Check error"
        exit
    fi
    make install
    if [ $? -ne "0" ]; then
        echo "make install failed!!! please Check error"
        exit
    fi
    cd $CURRDIR
    echo "install libgtest prometheus-cpp"
fi

#rapidjson安装
if [ ! -f "/usr/local/lib/pkgconfig/RapidJSON.pc" ]; then
    echo "begin install rapidjson"
    rm -rf rapidjson-master
    unzip rapidjson-master.zip
    cd rapidjson-master
    sed -i 's/-Werror//g' CMakeLists.txt
    mkdir build
    cd build
    cmake ..
    make
    if [ $? -ne "0" ]; then
        echo "make failed!!! please Check error"
        exit
    fi
    make install
    if [ $? -ne "0" ]; then
        echo "make install failed!!! please Check error"
        exit
    fi
    cd $CURRDIR
    echo "install libgtest rapidjson"
fi