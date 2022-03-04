# Ground_Station_for_Drone
The interface can visualize the system status and enable the user to give sparse instructions to the drone system.

Please refer to the UAVPatrol project webpage: http://www.yfan.site/UAVPatrol.html.

# environmet

use opencv-4.5.1 & Qt5.12.11

# 信息记录

## GPS标准

谷歌地球 == GPS坐标 == WGS84坐标(大疆无人机GPS坐标)

谷歌地图 == 火星坐标 == GCJ-02坐标

百度地图 == 百度坐标 == BD09坐标 (QT上位机html地图坐标)

## 无人机朝向角度定义

|无人机接收WayPoints中的rotation|说明|
|:-:|:-:|
|0|正北|
|90|正东|
|180/-180|正南|
|-90|正西|

|QT上位机百度地图html显示/QT上位机存储|说明|
|:-:|:-:|
|0|正北|
|90|正东|
|180|正南|
|270|正西|

## QT上位机发送给MyUX信息

|key|value|备注|
|:-:|:-:|:-:|
|mission|0|起飞|
|mission|1|发送WayPoints|
|mission|2|允许虚拟控制|
|mission|3|禁止虚拟控制|
|mission|4|降落|
|mission|5|障碍预测(停止)|
|mission|6|障碍预测(避障)|
|mission|7|返航|

### mission 详细
|mission|key|value|备注|
|:-:|:-:|:-:|:-:|
|mission0|mission|0|起飞|
|||||
|mission1|mission|1|发送WayPoints|
||altitude|double|高度|
||way_point_num|int|WayPoints数目|
|||||
|mission2|mission|2|允许虚拟控制|
||yaw|double [-1, 1]|左右旋转|
||roll|double [-1, 1]|前后平移|
||pitch|double [-1, 1]|左右平移|
||throttle|double [-1, 1]|上下平移|
|||||
|mission3|mission|3|禁止虚拟控制|
||yaw|double [-1, 1]|左右旋转|
||roll|double [-1, 1]|前后平移|
||pitch|double [-1, 1]|左右平移|
||throttle|double [-1, 1]|上下平移|
|||||
|mission4|mission|4|降落|
|||||
|mission5|mission|5|障碍预测(避障)|
||isCollFlag|bool|前方是否有障碍|
|||||
|mission5|mission|6|障碍预测(停止)|
||isCollFlag|bool|前方是否有障碍|
|mission1|mission|7|发送WayPoints|
||altitude|double|高度|
||way_point_num|int|WayPoints数目|


## 通信端口号

dronet -> QT: 5555

coll pred -> QT: 5556

QT -> MyUX: 6665

MyUx -> QT: 6666

## 虚拟控制

QT发送给MyUX的虚拟控制信息为double类型，范围为[-1, 1]

|Key|动作|
|:-:|:-:|
|yaw|左右旋转|
|throttle|飞行高度|
|pitch|左右平移|
|roll|前后速度|

QT上位机显示数值 = 发送数值 * 1000
