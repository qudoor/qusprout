#!/bin/bash

#代码根目录
src_root=`pwd`

#编译结果安装目录
build_path=$src_root/build

#安装目录
install_path=$src_root/install

#服务名称
service_binfile_array=('qusprout' 'quwork')

#编译方法：1:cmake, 2:makefile
service_build_method_array=(1 1)

#编译目录
service_src_path_array=( \
    "${src_root}/qusprout" \
    "${src_root}/quwork"
)

#编译单个服务
build_service() {
    DATE=$(date +"%Y-%m-%d %H:%M:%S")
    if [ $# != 2 ]; then
        echo -e "\033[31m------------build_service has bad params---------------\033[0m"
        exit -1
    fi

    service_name=$1
    service_version=$2
    service_index=-1
    for(( i=0;i<${#service_binfile_array[@]};i++)) do
        if [ ${service_binfile_array[i]} == $service_name ]; then
            service_index=$i
        fi
    done;

    if [ $service_index -lt 0 ]; then
        echo -e "\033[31m------------build_service has bad servername $service_name ---------------\033[0m"
        exit -1
    fi
    
    if [ ! -n $service_name ]; then
        echo -e "\033[31m------------bad service name $service_name---------------\033[0m"
        exit -2
    fi

    service_src_path=${service_src_path_array[$service_index]}
    service_binfile=${service_binfile_array[$service_index]}
    service_build_method=${service_build_method_array[$service_index]}

    echo -e "\n\033[33m------------service_name $service_name version $service_version------------\033[0m";

    echo "cd $build_path"
    #清空目录
    if [ -d $build_path ]; then
        rm -rf $build_path/*
    else
        mkdir -p ${build_path}
    fi
    cd $build_path

    if [ $service_build_method == 1 ]; then
        cmake -DCMAKE_INSTALL_PREFIX=$install_path/$service_name $service_src_path
        make
        make install
    else
        echo -e "\033[31m------------Build do nothing------------\033[0m";
    fi

    if [ $? -eq 0 ]; then
        echo -e "\033[32m------------Build $service_name version $service_version successed------------\033[0m";
    else
        echo -e "\033[31m------------Build $service_name version $service_version failed------------\033[0m";
        exit;
    fi

    if [ -f $install_path/$service_name/bin/$service_binfile ]
    then
        echo -e "\033[32m------------Install $service_name begin------------\033[0m";

        cd $install_path
        tar zcvf $service_name.v$service_version.tar.gz $service_name
        rm -rf $service_name

        echo -e "\033[32m------------Install $service_name end------------\033[0m";
    else
        echo -e "\033[31m------------Build $service_name successed but have no $service_sofile------------\033[0m";
    fi
}

#编译所有服务
build() {
    build_version=
    #创建安装目录
    if [ -d $install_path ]; then
        rm -rf $install_path/*
    else
        mkdir -p ${install_path}
    fi
	
    build_version=$1
    if [ $# == 1 ]; then
        for service_name in ${service_binfile_array[@]}
        do
            build_service $service_name $1
        done

    elif [ $# -ge 2 ]; then
        args=(`echo $@`)
        for((i=1;i<${#args[*]};i++)); do        
            build_service ${args[i]} $1
        done
    fi

    cd $src_root
    tar zcvf build-v${build_version}.tar.gz install
}

print_help() {
	echo "Usage Example: "
	echo "  $0 version 1.0.0 --Build all as version 1.0"
    	echo "  $0 version 1.0.0 qusprout|quwork  --Build one service as version 1.0"
}

case $1 in
	version)
		if [ $# -lt 2 ]; then
			print_help
			exit
		fi

        if [ $# == 2 ]; then
            build $2
        elif [ $# -ge 3 ]; then
            args=(`echo $@`)
            build $2 ${args[2]}
        fi
		;;
	*)

	print_help
		;;
esac
