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
project(QUSPROUT)

#添加子模块
add_subdirectory(quwork)
add_subdirectory(qusprout)
add_subdirectory(unittest)
add_subdirectory(examples)