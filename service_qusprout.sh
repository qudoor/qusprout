#!/bin/bash

install_path=/usr/local/qusprout
temp_path=$install_path/temp
service_path=/usr/local

install() {
    now=`date +%Y%m%d%H%M%S`
    release_path=$install_path/release/v$1
    mkdir -p $release_path

    if [ -d $temp_path ]; then
        rm -rf $temp_path 
    fi
    mkdir -p $temp_path

    cp build-v$1.tar.gz $temp_path
    cd $temp_path

    tar zxvf build-v$1.tar.gz -C .
    mv install/* .
    rm -rf build-v$1.tar.gz
    rm -rf install

    echo 'Tar the archive file'
    for file in `ls`
    do
        echo $file
        tar zxvf $file
    done

    cd $temp_path
    echo 'Delete the total archive file'
    rm -rf *.tar.gz

    if [ $# -eq 1 ]; then
        for service_item in `ls`
        do
            if [ -d $service_item ]; then
                echo "Deply the $service_item service!!"
                if [ -d $release_path/$service_item ]; then
                    rm -rf $release_path/$service_item
                fi
                mv $service_item $release_path/

				if [ ! -d $service_path/bin ]; then
					mkdir -p $service_path/bin
				fi
				if [ -L $service_path/bin/$service_item ]; then
					rm -rf $service_path/bin/$service_item
				fi
				if [ -f $service_path/bin/$service_item ]; then
					rm -rf $service_path/bin/$service_item
				fi
                ln -s $release_path/$service_item/bin/$service_item $service_path/bin/$service_item

				if [ ! -d $service_path/etc/qusprout ]; then
					mkdir -p $service_path/etc/qusprout
				fi
				if [ -L $service_path/etc/qusprout/$service_item.yaml ]; then
					rm -rf $service_path/etc/qusprout/$service_item.yaml
				fi
				if [ -f $service_path/etc/qusprout/$service_item.yaml ]; then
					rm -rf $service_path/etc/qusprout/$service_item.yaml
				fi
				ln -s $release_path/$service_item/etc/qusprout/$service_item.yaml $service_path/etc/qusprout/$service_item.yaml
            fi
        done
    elif [ $# -eq 2 ]; then
        if [ -d $2 ]; then
            echo "Deply the $2 service!!"
            if [ -d $release_path/$2 ]; then
                rm -rf $release_path/$2
            fi
            mv $2 $release_path/

			if [ ! -d $service_path/bin ]; then
				mkdir -p $service_path/bin
			fi
			if [ -L $service_path/bin/$2 ]; then
				rm -rf $service_path/bin/$2
			fi
			if [ -f $service_path/bin/$2 ]; then
				rm -rf $service_path/bin/$2
			fi
			ln -s $release_path/$2/bin/$2 $service_path/bin/$2

			if [ ! -d $service_path/etc/qusprout ]; then
				mkdir -p $service_path/etc/qusprout
			fi
			if [ -L $service_path/etc/qusprout/$2.yaml ]; then
				rm -rf $service_path/etc/qusprout/$2.yaml
			fi
			if [ -f $service_path/etc/qusprout/$2.yaml ]; then
				rm -rf $service_path/etc/qusprout/$2.yaml
			fi
			ln -s $release_path/$2/etc/qusprout/$2.yaml $service_path/etc/qusprout/$2.yaml
        fi
    fi
}

print_help() {
	echo "Usage Example: "
	echo "$0 version 1.0.0 qusprout-master|qusprout-slave|qusprout-work -- install the .so file"
	echo "$0 restart qusprout-master|qusprout-slave|qusprout-work -- restrat server"
	echo "$0 start qusprout-master|qusprout-slave|qusprout-work -- start server"
	echo "$0 stop qusprout-master|qusprout-slave|qusprout-work -- stop server"
}

case $1 in
	version)
		if [ $# -lt 2 ];then
			echo "Bad param, please check the usage example"
			print_help
			exit
		fi

		if [ $# -gt 4 ];then
			echo "Bad param, please check the usage example"
			print_help
			exit
		fi

		release_file=build-v$2.tar.gz
		if [ ! -f $release_file ]; then
			echo "There is no release file [$release_file], you should build it first use: ./build.sh version $2"
			print_help
			exit
		fi
		
		if [ $# -eq 2 ]; then
		    	echo "Install all service!!!"
			install $2
		elif [ $# -eq 3 ]; then
		    	echo "Install $3 service!!!"
			install $2 $3
		fi

		;;
	restart)
		if [ $# != 2 ];then
			echo "Bad param, please check the usage example"
			exit
		fi

		echo "Now restart $service_path/bin/$2 ..."
		ps -lef | grep "$service_path/bin/$2" | grep -v "grep" | awk '{print $4}' | xargs kill
		sleep 5s
		echo "restart $service_path/bin/$2 -c $service_path/etc/qusprout/$2.yaml"
		nohup $service_path/bin/$2 -c $service_path/etc/qusprout/$2.yaml &

		;;
	stop)
		if [ $# != 2 ];then
			echo "Bad param, please check the usage example"
			exit
		fi

		echo "Now stop $service_path/bin/$2 ..."
		ps -lef | grep "$service_path/bin/$2" | grep -v "grep" | awk '{print $4}' | xargs kill

		;;
	start)
		if [ $# != 2 ];then
			echo "Bad param, please check the usage example"
			exit
		fi

		echo "start $service_path/bin/$2 -c $service_path/etc/qusprout/$2.yaml"
		nohup $service_path/bin/$2 -c $service_path/etc/qusprout/$2.yaml &

		;;
	*)

	print_help
esac
