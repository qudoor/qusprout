--机器状态表
CREATE TABLE machineStatus (
	id integer PRIMARY KEY AUTOINCREMENT,			--自增id
	hostName varchar(50) DEFAULT '',				--主机名
	allCpuNum integer DEFAULT 0,					--cpu数量
	allMemory integer DEFAULT 0,					--内存
	useMemory integer DEFAULT 0,					--已用内存
	gpuType int8 DEFAULT 0,							--gpu类型 1：nvidia
	allGpuMemory integer DEFAULT 0,					--gpu内存
	useGpuMemory integer DEFAULT 0,					--已用gpu内存
	currTime DATETIME DEFAULT CURRENT_TIMESTAMP		--当前时间
);

--当前任务统计表
CREATE TABLE SisCurrTaskCount (
	id integer PRIMARY KEY AUTOINCREMENT,			--自增id
	hostName varchar(50) DEFAULT '',				--主机名
	execType int8 DEFAULT 0,						--执行类型 1：cpu 2：cpu+mpi 3：gpu
	qubitNum integer DEFAULT 0,						--qubit数量
	currTaskNum integer DEFAULT 0,					--当前任务数
	currTimeoutTaskNum integer DEFAULT 0,			--当前超过1分钟没有更新的任务数
	currTime DATETIME DEFAULT CURRENT_TIMESTAMP		--当前时间
);

--任务统计表
CREATE TABLE SisTaskCount (
	id integer PRIMARY KEY AUTOINCREMENT,			--自增id
	hostName varchar(50) DEFAULT '',				--主机名
	execType int8 DEFAULT 0,						--执行类型 1：cpu 2：cpu+mpi 3：gpu
	qubitNum integer DEFAULT 0,						--qubit数量
	num integer DEFAULT 0,							--任务数
	startTime DATETIME DEFAULT CURRENT_TIMESTAMP,	--开始时间
	endTime DATETIME DEFAULT CURRENT_TIMESTAMP		--结束时间
);

--接口调用次数统计表
CREATE TABLE SisInterfaceCount (
	id integer PRIMARY KEY AUTOINCREMENT,			--自增id
	hostName varchar(50) DEFAULT '',				--主机名
	execType int8 DEFAULT 0,						--执行类型 1：cpu 2：cpu+mpi 3：gpu
	qubitNum integer DEFAULT 0,						--qubit数量
	interfaceName varchar(50) DEFAULT '',			--接口名称
	count integer DEFAULT 0,						--次数
	startTime DATETIME DEFAULT CURRENT_TIMESTAMP,	--开始时间
	endTime DATETIME DEFAULT CURRENT_TIMESTAMP		--结束时间
);

--接口调用耗时统计表
CREATE TABLE SisInterfaceElapsed (
	id integer PRIMARY KEY AUTOINCREMENT,			--自增id
	hostName varchar(50) DEFAULT '',				--主机名
	execType int8 DEFAULT 0,						--执行类型 1：cpu 2：cpu+mpi 3：gpu
	qubitNum integer DEFAULT 0,						--qubit数量
	interfaceName varchar(50) DEFAULT '',			--接口名称
	elapsed integer DEFAULT 0,						--耗时，单位：ms
	count integer DEFAULT 0,						--次数
	startTime DATETIME DEFAULT CURRENT_TIMESTAMP,	--开始时间
	endTime DATETIME DEFAULT CURRENT_TIMESTAMP		--结束时间
);

--接口调用返回码统计表
CREATE TABLE SisInterfaceCode (
	id integer PRIMARY KEY AUTOINCREMENT,			--自增id
	hostName varchar(50) DEFAULT '',				--主机名
	execType int8 DEFAULT 0,						--执行类型 1：cpu 2：cpu+mpi 3：gpu
	qubitNum integer DEFAULT 0,						--qubit数量
	interfaceName varchar(50) DEFAULT '',			--接口名称
	code integer DEFAULT 0,							--返回码
	count integer DEFAULT 0,						--次数
	startTime DATETIME DEFAULT CURRENT_TIMESTAMP,	--开始时间
	endTime DATETIME DEFAULT CURRENT_TIMESTAMP		--结束时间
);


SELECT count(1) FROM sqlite_master WHERE type='table' AND name='machineStatus' COLLATE NOCASE;
