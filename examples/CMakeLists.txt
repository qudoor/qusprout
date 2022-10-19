#定义最新cmake的编译版本
cmake_minimum_required(VERSION 3.7)

#生成rpc接口
execute_process(COMMAND thrift -r -out ${PROJECT_SOURCE_DIR}/interface/ --gen cpp ${PROJECT_SOURCE_DIR}/../idl/ecode.thrift)
execute_process(COMMAND thrift -r -out ${PROJECT_SOURCE_DIR}/interface/ --gen cpp ${PROJECT_SOURCE_DIR}/../idl/qusproutdata.thrift)
execute_process(COMMAND thrift -r -out ${PROJECT_SOURCE_DIR}/interface/ --gen cpp ${PROJECT_SOURCE_DIR}/../idl/qusprout.thrift)
execute_process(COMMAND thrift -r -out ${PROJECT_SOURCE_DIR}/interface/ --gen cpp ${PROJECT_SOURCE_DIR}/../idl/resource.thrift)
execute_process(COMMAND thrift -r -out ${PROJECT_SOURCE_DIR}/interface/ --gen cpp ${PROJECT_SOURCE_DIR}/../idl/qurootresource.thrift)
if(EXISTS ${PROJECT_SOURCE_DIR}/interface/MasterServer_server.skeleton.cpp)
    execute_process(COMMAND rm -rf ${PROJECT_SOURCE_DIR}/interface/MasterServer_server.skeleton.cpp)
endif()
if(EXISTS ${PROJECT_SOURCE_DIR}/interface/QuSproutServer_server.skeleton.cpp)
    execute_process(COMMAND rm -rf ${PROJECT_SOURCE_DIR}/interface/QuSproutServer_server.skeleton.cpp)
endif()
if(EXISTS ${PROJECT_SOURCE_DIR}/interface/QuRootServer_server.skeleton.cpp)
    execute_process(COMMAND rm -rf ${PROJECT_SOURCE_DIR}/interface/QuRootServer_server.skeleton.cpp)
endif()

#添加子模块
add_subdirectory(grover)