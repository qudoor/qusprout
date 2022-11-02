#!/bin/bash

qusprout_path=/usr/local/bin/qusprout

qusprout_yaml=/usr/local/etc/qusprout/qusprout.yaml

nohup $qusprout_path -c $qusprout_yaml &

/bin/bash
