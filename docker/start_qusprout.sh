#!/bin/bash

master_path=/usr/local/bin/qusprout-master
slave_path=/usr/local/bin/qusprout-slave

master_yaml=/usr/local/etc/qusprout/qusprout-master.yaml
slave_yaml=/usr/local/etc/qusprout/qusprout-slave.yaml

nohup $master_path -c $master_yaml &
nohup $slave_path -c $slave_yaml &

/bin/bash
