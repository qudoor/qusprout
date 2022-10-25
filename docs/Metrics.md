# QuSprout的prometheus监控配置文档
## 1 概述
文档对QuSprout的监控组件的配置、安装、部署，以及QuSprout的grafana监控面板配置等进行了详细的说明。

## 2 监控的安装和配置
### 2.1 下载监控服务镜像
```
docker pull prom/prometheus
docker pull grafana/grafana
docker pull prom/node-exporter
```

### 2.2 运行镜像
1、修改prometheus配置
```
# cd /root/prometheus
# vim prometheus.yml
global:
  scrape_interval: 15s
  evaluation_interval: 15s

alerting:
  alertmanagers:
    - static_configs:
        - targets:
          # - alertmanager:9093

rule_files:
  # - "first_rules.yml"
  # - "second_rules.yml"

scrape_configs:
  - job_name: "prometheus"
    static_configs:
      - targets: ["localhost:9090"]
  - job_name: linux
    static_configs:
      - targets: ['${qusprout_ip}:9100']
  - job_name: qusprout
    static_configs:
      - targets: ['${qusprout_ip}:9093']
```

2、运行prometheus
```  
docker run -itd --name=docker_prometheus --restart=always -p 9090:9090 -v /root/prometheus/prometheus.yml:/etc/prometheus/prometheus.yml -v "/etc/localtime:/etc/localtime" prom/prometheus
```
web地址：http://${qusprout_ip}:9090

3、运行grafana
```
mkdir -p /root/grafana
chmod 777 /root/grafana
chmod 777 /root/grafana -R
docker run -itd --name=grafana --restart=always -p 3000:3000 -v /root/grafana:/var/lib/grafana -v "/etc/localtime:/etc/localtime" grafana/grafana
```
web地址：http://${qusprout_ip}:3000

登录账号：admin/admin

4、运行node-exporter
```
docker run -itd --name=node-exporter --restart=always -p 9100:9100 -v "/etc/localtime:/etc/localtime" prom/node-exporter
```
web地址：http://${qusprout_ip}:9100/metrics

5、查看运行状态
```
# docker ps -a
CONTAINER ID   IMAGE                COMMAND                  CREATED          STATUS          PORTS                                       NAMES
017c5cd71058   prom/node-exporter   "/bin/node_exporter"     5 seconds ago    Up 4 seconds    0.0.0.0:9100->9100/tcp, :::9100->9100/tcp   node-exporter
5c09bbe84816   grafana/grafana      "/run.sh"                4 minutes ago    Up 4 minutes    0.0.0.0:3000->3000/tcp, :::3000->3000/tcp   grafana
9da6ed484f1a   prom/prometheus      "/bin/prometheus --c…"   12 minutes ago   Up 12 minutes   0.0.0.0:9090->9090/tcp, :::9090->9090/tcp   docker_prometheus
```

### 2.3 配置监控
1、登录http://${qusprout_ip}:3000，在首页，选择 "Add your first data source"；

2、选择 Prometheus，填写http-->url-->http://${qusprout_ip}:9090，在选择save/test；

3、选择导入面板，选择8919；

4、选择导入面板，选择docs/QuSprout.json；