#定义最新cmake的编译版本
cmake_minimum_required(VERSION 3.7)

#指定项目的名称
project(qusprout-work)

#生成rpc接口
execute_process(COMMAND thrift -r -out ${PROJECT_SOURCE_DIR}/interface/ --gen cpp ${PROJECT_SOURCE_DIR}/../idl/ecode.thrift)
execute_process(COMMAND thrift -r -out ${PROJECT_SOURCE_DIR}/interface/ --gen cpp ${PROJECT_SOURCE_DIR}/../idl/qusproutdata.thrift)
execute_process(COMMAND thrift -r -out ${PROJECT_SOURCE_DIR}/interface/ --gen cpp ${PROJECT_SOURCE_DIR}/../idl/work.thrift)
if(EXISTS ${PROJECT_SOURCE_DIR}/interface/WorkServer_server.skeleton.cpp)
    execute_process(COMMAND rm -rf ${PROJECT_SOURCE_DIR}/interface/WorkServer_server.skeleton.cpp)
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
	"${PROJECT_SOURCE_DIR}/../common/qulog.cpp"
	"*.cpp"
	"command/*.cpp"
	"config/*.cpp"
	"interface/*.cpp"
	"server/*.cpp"
	"node/*.cpp"
	"common/*.cpp"
)

#头文件搜索路径，路径之间用空格分隔，相当于指定-I参数
include_directories(${PROJECT_SOURCE_DIR}/../include ${PROJECT_SOURCE_DIR}/.. ${PROJECT_SOURCE_DIR} /usr/include /usr/local/include /usr/local/include/thrift)

#动态链接库或静态链接库的搜索路径，相当于-L参数
IF (APPLE)
	link_directories(. ${PROJECT_SOURCE_DIR}/../lib /usr/local/lib)
ELSE ()
	link_directories(. ${PROJECT_SOURCE_DIR}/../lib /usr/local/lib /usr/local/lib64)
ENDIF ()
#设置可执行程序
add_executable(${PROJECT_NAME} ${SRC_LIST})

#添加链接库,相同于指定-l参数
IF (APPLE)
	target_link_libraries(${PROJECT_NAME} libglog.a;libyaml-cpp.a;libthrift.a;libgflags.a;libQuEST.a;libmpi.a;libhwloc.a;dl;pthread)
ELSE ()
	target_link_libraries(${PROJECT_NAME} libglog.a;libyaml-cpp.a;libthrift.a;libgflags.a;libQuEST.a;libmpi.a;libhwloc.a;rt;dl;pthread)
ENDIF ()

#安装
INSTALL(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
)
INSTALL(FILES ${PROJECT_SOURCE_DIR}/../config/qusprout-work.yaml
    DESTINATION etc/qusprout/
)
