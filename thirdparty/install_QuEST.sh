#!/bin/bash

set -e
CURRDIR=$(pwd)
OS=`uname -s`
OSMachine=`uname -m`

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
