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
TARGET = testglog
TARGET1 = testglog1

#定义OBJ文件变量
OBJS = testglog.o
OBJS1 = testglog1.o

#定义库文件路径变量	    
STATCI_LIBS = -L. -L/usr/local/lib64 -L/usr/local/lib -L/usr/lib -L/usr/lib64 -Wl,-Bstatic -lpthread
SHARED_LIBS = -L. -L/usr/local/lib64 -L/usr/local/lib -L/usr/lib -L/usr/lib64 -Wl,-Bdynamic -lglog

#定义头文件路径变量
INCLUDES = -I. -I/usr/include -I/usr/local/include
				   
#定义编译选项变量
CFLAGS = -Wall -fPIC -std=c++11 -g -DGLOG_CUSTOM_PREFIX_SUPPORT

#定义连接选项变量
LFLAGS = -Wall -fPIC -std=c++11 -g -DGLOG_CUSTOM_PREFIX_SUPPORT -Wl,-rpath,/usr/local/lib:/usr/lib64:/usr/local/lib64:/usr/lib:/usr/lib64
	
.PHONY: all 
all:$(TARGET) $(TARGET1)

#“.PHONY”来显示地指明一个目标是“伪目标”
.PHONY : clean install
clean :
	-rm $(OBJS) $(TARGET) $(OBJS1) $(TARGET1)

#格式：目标集合: 目标模式 : 目标依赖模式
#“$<”表示所有的依赖目标集
#“$@”表示目标模式集
#@echo "正在编译各个源文件"
$(OBJS): %.o: %.cpp 
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

$(OBJS1): %.o: %.cpp 
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

#主目标依赖命令(使用了字符“@”使得make执行命令不显示在屏幕上)
$(TARGET) : $(OBJS)
	$(CC) $(LFLAGS) -fPIC -o $(TARGET) $(OBJS) $(STATCI_LIBS) $(SHARED_LIBS)

$(TARGET1) : $(OBJS1)
	$(CC) $(LFLAGS) -fPIC -o $(TARGET1) $(OBJS1) $(STATCI_LIBS) $(SHARED_LIBS)