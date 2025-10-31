#每个创建的文件夹下需创建CMAKE文件
cmake_minimum_required(VERSION 3.13)
target_sources(${build_target} PRIVATE wsd_datacall.c wsd_factorypara.c wsd_fota_update.c wsd_mqtt.c wsd_tcp.c wsd_uart_servies.c timefunc.c cJSON.c wsd_dlt645.c wsd_flag.c wsd_meter_update.c wsd_dlt698.c wsd_modbus.c  wsd_sysevent.c ReadMeter.c wsd_ep212.c)
#target_sources(${build_target} PRIVATE wsd_datacall.c timefunc.c wsd_tcp.c wsd_dlt698.c  wsd_flag.c wsd_factorypara.c cJSON.c wsd_uart_servies.c  wsd_modbus.c wsd_sysevent.c wsd_mqtt.c wsd_dlt645.c)