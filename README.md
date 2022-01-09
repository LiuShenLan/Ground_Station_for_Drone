# Ground_Station_for_Drone
The interface can visualize the system status and enable the user to give sparse instructions to the drone system.

Please refer to the UAVPatrol project webpage: http://www.yfan.site/UAVPatrol.html.


### Note

修改了/usr/include/python3.8/object.h文件，用完记得把object_copy.h改回去

### TCP指令记录

|key|value|备注|
|:-:|:-:|:-:|
|mission|0|起飞|
|mission|1|发送WayPoints|
|mission|2|允许虚拟控制|
|mission|3|禁止虚拟控制|
|mission|4|降落|

tcpServerMyUXReceive port: 6666

tcpServerMyUXSend port: 6665

tcpServerDronet port: 5555