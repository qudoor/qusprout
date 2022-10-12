#include "errorcode.h"

std::map<int, std::string> g_errormsg = {
    { COMMON_SUCCESS, "成功" }, 
    { COMMON_OHER, "未知错误" }, 
    { COMMON_INVALID_PARAM, "不合法参数" }, 
    { COMMON_TIMEOUT, "超时" }, 
    { COMMON_EXCEPTION, "异常" }, 
    { COMMON_MEM_NOT_ENOUGH, "内存不足" }, 
    { COMMON_NOT_INIT, "未初始化" },
    { COMMON_NOT_RESOURCE, "无可用的资源" },
    { COMMON_PRASE_ERROR, "数据解析异常" },
    { COMMON_NOT_EXIST, "不存在" }
};
