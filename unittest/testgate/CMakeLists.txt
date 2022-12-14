#****************************************************************************
#
# This code is licensed under the Apache License, Version 2.0. You may
# obtain a copy of this license in the LICENSE.txt file in the root directory
# of this source tree or at http://www.apache.org/licenses/LICENSE-2.0.
#
# Any modifications or derivative works of this code must retain this
# copyright notice, and modified files need to carry a notice indicating
# that they have been altered from the originals.
#
#****************************************************************************/

#定义最新cmake的编译版本
cmake_minimum_required(VERSION 3.7)

#指定项目的名称
project(testgate)

#生成rpc接口
execute_process(COMMAND thrift -r -out ${PROJECT_SOURCE_DIR}/../../quwork/interface/ --gen cpp ${PROJECT_SOURCE_DIR}/../../idl/ecode.thrift)
execute_process(COMMAND thrift -r -out ${PROJECT_SOURCE_DIR}/../../quwork/interface/ --gen cpp ${PROJECT_SOURCE_DIR}/../../idl/qusproutdata.thrift)
execute_process(COMMAND thrift -r -out ${PROJECT_SOURCE_DIR}/../../quwork/interface/ --gen cpp ${PROJECT_SOURCE_DIR}/../../idl/qusprout.thrift)
if(EXISTS ${PROJECT_SOURCE_DIR}/../../quwork/interface/QuSproutServer_server.skeleton.cpp)
    execute_process(COMMAND rm -rf ${PROJECT_SOURCE_DIR}/../../quwork/interface/QuSproutServer_server.skeleton.cpp)
endif()

#set设置普通变量，缓存变量或环境变量
#指定的编译器路径
IF (APPLE)
	IF(EXISTS "/usr/local/opt/llvm/bin/clang++")
		set(CMAKE_CXX_COMPILER "/usr/local/opt/llvm/bin/clang++")
	ELSE()
		set(CMAKE_CXX_COMPILER "clang++")
	ENDIF()
ELSE ()
	set(CMAKE_CXX_COMPILER "g++")
ENDIF ()
#显式要求指明支持C++标准
set(CMAKE_CXX_STANDARD_REQUIRED True)
#启动对C++11标准的支持
set(CMAKE_CXX_STANDARD 11)

find_package(CUDA)
if(CUDA_FOUND)
	set(CMAKE_EXE_LINKER_FLAGS "-DNVIDIA_CUDA")
	add_compile_options(-DNVIDIA_CUDA)
endif(CUDA_FOUND)

#添加编译选项
add_compile_options(-g -Wall -fPIC -DGLOG_CUSTOM_PREFIX_SUPPORT)

#判断存在openmp才添加编译参数
find_package(OpenMP)
if (OPENMP_FOUND)
	#链接参数
	set(CMAKE_EXE_LINKER_FLAGS "-fopenmp -DGLOG_CUSTOM_PREFIX_SUPPORT")
	add_compile_options(-fopenmp)
else()
	#链接参数
	set(CMAKE_EXE_LINKER_FLAGS "-DGLOG_CUSTOM_PREFIX_SUPPORT")
endif ()

#使用 GLOB 命令使用通配符模式匹配查找文件
file(GLOB SRC_LIST
	"${PROJECT_SOURCE_DIR}/../../common/*.cpp"
	"${PROJECT_SOURCE_DIR}/../../quwork/common/*.cpp"
	"${PROJECT_SOURCE_DIR}/../../quwork/command/*.cpp"
	"${PROJECT_SOURCE_DIR}/../../quwork/config/*.cpp"
	"${PROJECT_SOURCE_DIR}/../../quwork/interface/*.cpp"
	"*.cpp"
)

#头文件搜索路径，路径之间用空格分隔，相当于指定-I参数
include_directories(${PROJECT_SOURCE_DIR}/../../include ${PROJECT_SOURCE_DIR}/../.. ${PROJECT_SOURCE_DIR}/../../quwork ${PROJECT_SOURCE_DIR} /usr/include /usr/local/include /usr/local/include/thrift ${CUDA_INCLUDE_DIRS})

#动态链接库或静态链接库的搜索路径，相当于-L参数
IF (APPLE)
	link_directories(. ${PROJECT_SOURCE_DIR}/../../lib /usr/local/lib)
ELSE ()
	link_directories(. ${PROJECT_SOURCE_DIR}/../../lib /usr/local/lib /usr/local/lib64)
ENDIF ()
#设置可执行程序
add_executable(${PROJECT_NAME} ${SRC_LIST})

#添加链接库,相同于指定-l参数
target_link_libraries(${PROJECT_NAME} libglog.a;libyaml-cpp.a;libthrift.a;libgflags.a;libQuEST.a;libmpi.a;libhwloc.a;libgtest.a;dl;pthread)
IF (APPLE)
ELSE ()
	target_link_libraries(${PROJECT_NAME} rt)
ENDIF ()
if(CUDA_FOUND)
	target_link_libraries(${PROJECT_NAME} ${CUDA_LIBRARIES})
endif(CUDA_FOUND)