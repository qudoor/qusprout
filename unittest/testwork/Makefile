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
TARGET = testwork

#定义OBJ文件变量
COMM_OBJS = ../../common/crc32.o ../../common/qulog.o ../../common/SystemHandle.o ../../common/TimerTask.o

OBJS = ../../work/command/executor.o ../../work/config/Config.o ../../work/interface/ecode_constants.o ../../work/interface/ecode_types.o ../../work/interface/qusproutdata_types.o \
	../../work/interface/QuSproutServer.o ../../work/common/QuESTIniter.o main.o \
	testGateCNOT.o testGateCP.o testGateCRX.o testGateCRY.o testGateCRZ.o \
	testGateCX.o testGateCY.o testGateCZ.o testGateH.o testGateP.o \
	testGateRX.o testGateRXX.o testGateRY.o testGateRYY.o testGateRZ.o \
	testGateRZZ.o testGateS.o testGateSdg.o testGateSqrtSwap.o testGateSqrtX.o \
	testGateSwap.o testGateT.o testGateTdg.o testGateX.o testGateY.o \
	testGateZ.o testGateU1.o testGateR.o testGateU2.o testGateU3.o \
	testGateX1.o testGateY1.o testGateZ1.o testGateiSwap.o testGateCR.o \
	testGateCU.o testGateU.o testGateCU1.o testGateCU3.o testGateGetPauli.o testAll.o

#定义库文件路径变量	    
STATCI_LIBS = -L. -L/usr/local/lib64 -L/usr/local/lib -L/usr/lib -L/usr/lib64 -L../../lib -Wl,-Bstatic -lyaml-cpp -lthrift -lglog -lgflags -lQuEST -lmpi -lhwloc -lgtest
SHARED_LIBS = -L. -L/usr/local/lib64 -L/usr/local/lib -L/usr/lib -L/usr/lib64 -L../../lib -Wl,-Bdynamic -lpthread -lrt -ldl 

#定义头文件路径变量
INCLUDES = -I. -I../.. -I../../work -I../../include -I/usr/include -I/usr/local/include -I/usr/local/include/thrift

#定义编译选项变量
CFLAGS = -Wall -fPIC -std=c++11 -g -DGLOG_CUSTOM_PREFIX_SUPPORT -DQuEST_PREC=2 -fopenmp 

#定义连接选项变量
LFLAGS = -Wall -fPIC -std=c++11 -g -DGLOG_CUSTOM_PREFIX_SUPPORT -DQuEST_PREC=2 -fopenmp -Wl,-rpath,/usr/local/lib:/usr/lib64:/usr/local/lib64:/usr/lib:/usr/lib64
	
.PHONY: all 
all:$(TARGET) 

#“.PHONY”来显示地指明一个目标是“伪目标”
.PHONY : clean install
clean :
	-rm $(COMM_OBJS) $(OBJS) $(TARGET)

#格式：目标集合: 目标模式 : 目标依赖模式
#“$<”表示所有的依赖目标集
#“$@”表示目标模式集
#@echo "正在编译各个源文件"
$(COMM_OBJS): %.o: %.cpp 
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

$(OBJS): %.o: %.cpp 
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

#主目标依赖命令(使用了字符“@”使得make执行命令不显示在屏幕上)
$(TARGET) : $(COMM_OBJS) $(OBJS)
	$(CC) $(LFLAGS) -fPIC -o $(TARGET) $(COMM_OBJS) $(OBJS) $(STATCI_LIBS) $(SHARED_LIBS)

