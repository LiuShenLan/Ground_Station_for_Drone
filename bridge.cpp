#include "bridge.h"
#include <QMessageBox>
#include <QJSEngine>
#include <QDebug>
#include "mainwindow.h"

// Init
bridge* bridge::instance()
{
    static bridge s_obj;
    return &s_obj;
}
bridge::bridge()
{
}

// WayPoints
Light_t	bridge::AddLight(int rot)
{
    Light_t tLight;
    tLight.strName = QString("Light%1").arg(s_count);
    tLight.strDesc = QString("Light Description %1").arg(s_count);
    tLight.fLng = _fLng;
    tLight.fLat = _fLat;
    tLight.nValue = 0;
    tLight.rotation = rot;  // Nav point 导航点方向滑块数据
    m_lightList.append(tLight);
    s_count++;
    return tLight;
}
void bridge::newPoint(double lng, double lat)
{
    _fLng = lng;
    _fLat = lat;
}
QList<Light_t> bridge::returnWayPointList()
{
//    AddLight();
    return m_lightList;
}
void bridge::removeAllPoints()
{
    m_lightList.clear();
    s_count = 1;
    emit removePoints();    // 发射removePoints信号
}
void bridge::setNavPointRotate(int rot)
{
    emit rotateNavPoint(rot);
}
void bridge::SetLightValue(const QString& strName, int nValue)
{
    int nCount = m_lightList.count();
    for(int i=0; i<nCount; i++)
    {
        Light_t tLight = m_lightList[i];
        if( m_lightList[i].strName == strName)
        {
            m_lightList[i].nValue = nValue;
            break;
        }
    }
    emit sigLightStatus(strName, nValue);
}
void bridge::onLightOn(const QString& strName)
{
    SetLightValue(strName, 1);
}
void bridge::onLightOff(const QString& strName)
{
    SetLightValue(strName, 0);
}
QVariantList bridge::GetLightData()
{
	int nCount = m_lightList.count();
    qDebug()<<nCount;
	QVariantList markerList;
	
	for(int i=0; i<nCount; i++)
	{
		Light_t tLight = m_lightList[i];
		QVariantMap map;
		map.insert("name", tLight.strName);
		map.insert("description", tLight.strDesc);
        map.insert("point", QString("%1,%2,%3").arg(tLight.fLng, 0, 10, 10).arg(tLight.fLat, 0, 10, 10).arg(tLight.rotation));
		map.insert("value", tLight.nValue);
		markerList << map;
	}
	
	return markerList;
}
void bridge::onUpdateData()
{
    emit sigUpdateLight();  // 发射sigUpdateLight信号
}

// 发射信号
void bridge::jsSendMsg(const QString& msg)
{
    emit toRecvMsg(msg);    // 用于mainwindow.cpp line:93，调用MainWindow::onRecvdMsg函数
}
void bridge::jsSendTargetPoint(const QString &msg)
{
    emit targetPointReceived(msg);
}





