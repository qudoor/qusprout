/****************************************************************************
*
* This code is licensed under the Apache License, Version 2.0. You may
* obtain a copy of this license in the LICENSE.txt file in the root directory
* of this source tree or at http://www.apache.org/licenses/LICENSE-2.0.
*
* Any modifications or derivative works of this code must retain this
* copyright notice, and modified files need to carry a notice indicating
* that they have been altered from the originals.
*
****************************************************************************/

#ifndef __H__ERRORCODE__H__
#define __H__ERRORCODE__H__

#include <map>
#include <string>

const int COMMON_SUCCESS = 0;               //成功
const int COMMON_OHER = 1001;               //未知错误
const int COMMON_INVALID_PARAM = 1002;      //不合法参数
const int COMMON_TIMEOUT = 1003;            //超时
const int COMMON_EXCEPTION = 1004;          //异常
const int COMMON_MEM_NOT_ENOUGH = 1005;     //内存不足
const int COMMON_NOT_INIT = 1006;           //未初始化
const int COMMON_NOT_RESOURCE = 1007;       //无可用的资源
const int COMMON_PRASE_ERROR = 1008;        //数据解析异常
const int COMMON_NOT_EXIST = 1009;          //不存在
const int COMMON_IS_EXIST = 1010;          //存在
const int COMMON_IS_QUEUE = 1011;          //缓存中

extern std::map<int, std::string> g_errormsg;

#endif
