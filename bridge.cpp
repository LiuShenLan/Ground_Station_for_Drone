#include "bridge.h"
#include <QMessageBox>
#include <QJSEngine>
#include <QDebug>
#include "mainwindow.h"

// Init
bridge* bridge::instance() {
	static bridge s_obj;
	return &s_obj;
}
bridge::bridge(){}

// WayPoints
wayPoint bridge::AddLight(int rot) {
	wayPoint tLight;
	tLight.strName = QString("Light%1").arg(wayPointsNum);
	tLight.strDesc = QString("Light Description %1").arg(wayPointsNum);
	tLight.fLng = _fLng;
	tLight.fLat = _fLat;
	tLight.nValue = 0;
	tLight.rotation = rot;	// Nav point 导航点方向滑块数据
	wayPointsAllList.append(tLight);
	wayPointsNum++;
	return tLight;
}
void bridge::newPoint(double lng, double lat) {
	_fLng = lng;
	_fLat = lat;
}
QList<wayPoint> bridge::returnWayPointList() {
	return wayPointsAllList;
}
void bridge::removeAllPoints() {
	wayPointsAllList.clear();
	wayPointsNum = 1;
	emit removePoints();	// 发射removePoints信号，调用js程序，清除地图中显示的WayPoints
}
void bridge::setNavPointRotate(int rot) {
	emit rotateNavPoint(rot);	// 旋转导航点水滴方向
}
void bridge::SetLightValue(const QString& strName, int nValue) {
	int nCount = wayPointsAllList.count();
	for(int i=0; i<nCount; i++) {
		wayPoint tLight = wayPointsAllList[i];
		if(wayPointsAllList[i].strName == strName) {
			wayPointsAllList[i].nValue = nValue;
			break;
		}
	}
	emit sigLightStatus(strName, nValue);
}
void bridge::onLightOn(const QString& strName) {
	SetLightValue(strName, 1);
}
void bridge::onLightOff(const QString& strName) {
	SetLightValue(strName, 0);
}
QVariantList bridge::GetLightData() {
	int nCount = wayPointsAllList.count();
	QVariantList markerList;

	for(int i=0; i<nCount; i++) {
		wayPoint tLight = wayPointsAllList[i];
		QVariantMap map;
		QVector<double> gps = MainWindow::WGS84ToBD09(tLight.fLng, tLight.fLat);

		map.insert("name", tLight.strName);
		map.insert("description", tLight.strDesc);
		map.insert("point", QString("%1,%2,%3")
			.arg(gps[0], 0, 'f', 14)
			.arg(gps[1], 0, 'f', 14)
			.arg(tLight.rotation));
		map.insert("value", tLight.nValue);
		markerList << map;
	}
	return markerList;
}
void bridge::onUpdateData(){
	emit sigUpdateLight();  // 调用js程序，在地图中显示WayPoints
}

// 发射信号
void bridge::jsSendMsg(const QString& msg){
	emit toRecvMsg(msg);	// 调用MainWindow::onRecvdMsg函数
}
void bridge::jsSendTargetPoint(const QString &msg) {
	emit targetPointReceived(msg);	// 接收到js信息，调用MainWindow::onRecvTargetPoint函数
}





