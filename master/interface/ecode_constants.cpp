/**
 * Autogenerated by Thrift Compiler (0.16.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#include "ecode_constants.h"



const ecodeConstants g_ecode_constants;

ecodeConstants::ecodeConstants() {
  ErrMsg.insert(std::make_pair((ErrCode::type)4, "异常"));
  ErrMsg.insert(std::make_pair((ErrCode::type)2, "不合法参数"));
  ErrMsg.insert(std::make_pair((ErrCode::type)1, "未知错误"));
  ErrMsg.insert(std::make_pair((ErrCode::type)0, "成功"));
  ErrMsg.insert(std::make_pair((ErrCode::type)3, "超时"));
  ErrMsg.insert(std::make_pair((ErrCode::type)100, "内存不足"));
  ErrMsg.insert(std::make_pair((ErrCode::type)101, "未初始化"));
  ErrMsg.insert(std::make_pair((ErrCode::type)102, "机器未注册"));
  ErrMsg.insert(std::make_pair((ErrCode::type)103, "无可用的资源"));
  ErrMsg.insert(std::make_pair((ErrCode::type)104, "数据解析异常"));

}



