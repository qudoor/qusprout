# **********************************************************************
# 
#     说		明： 
#     作		者：
#     最后修改日期： 
#     版		本： 
# **********************************************************************
#定义编译连接指令变量（可以是cc、gcc、g++等）
CC = g++ 

#定义主目标文件变量, 动态库的名字应符合“lib*.so*”这种格式
TARGET_CLIENT = client

#定义OBJ文件变量
OBJ_THRIFT_DEF = .thrift/ecode.cpp .thrift/qusproutdata.cpp .thrift/qusprout.cpp .thrift/resource.cpp
OBJS_THRIFT = .thrift/ecode_constants.o .thrift/ecode_types.o .thrift/qusproutdata_types.o .thrift/QuSproutServer.o .thrift/resource_types.o .thrift/MasterServer.o

OBJS_CLIENT = client.o

#定义库文件路径变量	    
STATCI_LIBS = -L. -L/usr/local/lib64 -L/usr/local/lib -L/usr/lib -L/usr/lib64 -Wl,-Bstatic -lthrift
SHARED_LIBS = -L. -L/usr/local/lib64 -L/usr/local/lib -L/usr/lib -L/usr/lib64 -Wl,-Bdynamic -lm -lpthread -lrt

#定义头文件路径变量
INCLUDES = -I. -I.thrift  -I/usr/include -I/usr/local/include -I/usr/local/include/thrift
				   
#定义编译选项变量
CFLAGS = -Wall -fPIC -std=c++11

#定义连接选项变量
LFLAGS = -Wall -fPIC -std=c++11 -Wl,-rpath,/usr/local/lib:/usr/lib64:/usr/local/lib64:/usr/lib:/usr/lib64 
	
.PHONY: all 
all:$(TARGET_CLIENT)

#“.PHONY”来显示地指明一个目标是“伪目标”
.PHONY : clean install
clean :
	-rm $(TARGET_CLIENT)  $(OBJS) $(OBJ_THRIFT) 
	-rm -rf .thrift

#格式：目标集合: 目标模式 : 目标依赖模式
#“$<”表示所有的依赖目标集
#“$@”表示目标模式集
#@echo "正在编译各个源文件"
$(OBJ_THRIFT_DEF): .thrift/%.cpp: ../../idl/%.thrift 
	@if [ ! -d .thrift ]; then mkdir -p .thrift; fi; 
	thrift -r -out .thrift/ --gen cpp $< 

$(OBJS_THRIFT): %.o: %.cpp 
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

$(OBJS_CLIENT): %.o: %.cpp 
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

#主目标依赖命令(使用了字符“@”使得make执行命令不显示在屏幕上)
$(TARGET_CLIENT) : $(OBJ_THRIFT_DEF) $(OBJS_THRIFT) $(OBJS_CLIENT) 
	$(CC)  $(LFLAGS) -fPIC -o  $(TARGET_CLIENT) $(OBJS_CLIENT) $(OBJS_THRIFT) $(STATCI_LIBS) $(SHARED_LIBS)