#ifndef BRIDGE_H
#define BRIDGE_H

#include <QObject>
#include <QJSValue>
#include <functional>
#include <QVariant>
#include <QString>
#include <QJsonArray>
#include <QJsonObject>
#include <QVariantList>


typedef struct
{
	QString strName;	// 名称
	QString strDesc;	// 描述
	double fLng;	// 经度
	double fLat;	// 纬度
	int nValue;		// 0:显示箭头   1:显示绿色圆形
	int rotation;	// 旋转角度(朝向)
}wayPoint;

class bridge : public QObject
{
	friend class MainWindow;
	Q_OBJECT
public:
	// Init
	static bridge* instance();	// 返回bridge对象

	// WayPoints
	int wayPointsNum = 1;	// WayPoints数目
	wayPoint AddLight(int rot);	// 根据临时WayPoint经纬度信息生成wayPoint对象并添加到wayPointsAllList中
	void newPoint(double lng, double lat);	// 设置临时WayPoint经纬度信息
	QList<wayPoint> returnWayPointList();	// 返回wayPointsAllList
	const QList<wayPoint>& GetLightList(){return wayPointsAllList;}	// 返回所有的WayPoints的list的引用
	void removeAllPoints();	// 移除所有WayPoints信息
	void setNavPointRotate(int rot);	// 设置html地图中的选择WayPoints时显示的水滴的朝向
private:
	double _fLng = 0, _fLat = 0;	// 临时WayPoint经纬度信息
	QList<wayPoint> wayPointsAllList;	// 所有WayPoints的list
	bridge();	// 空构造函数
	void SetLightValue(const QString& strName, int nValue);	// 设置strName的wayPoints的nValue
signals:	// 信号声明区
	void sigUpdateLight();	// 调用js程序，在地图中显示WayPoints
	void removePoints();	// 调用js程序，清除地图中显示的WayPoints
	void sigLightStatus(const QString& strName, int nvalue);	// 设置strName的wayPoints的nValue
	void toRecvMsg(const QString& msg);	// 调用MainWindow::onRecvdMsg函数
	void targetPointReceived(const QString& msg);	// 接收到js信息，调用MainWindow::onRecvTargetPoint函数
	void rotateNavPoint(int rot);	// 旋转导航点水滴方向
public slots:	// 槽声明区
	// WayPoints
	void onLightOn(const QString& strName);	// 将WayPoints中的strName的nValue设置为1
	void onLightOff(const QString& strName);// 将WayPoints中的strName的nValue设置为0
	QVariantList GetLightData();	// 返回WayPoints list的map形式
	void onUpdateData();	// 调用js程序，在地图中显示WayPoints
	// 发射信号
	void jsSendMsg(const QString& msg);	// 接收到js信息，调用MainWindow::onRecvdMsg函数，其中msg为在html中点击确定的经纬度信息
	void jsSendTargetPoint(const QString& msg);	// 接收到js信息，调用MainWindow::onRecvTargetPoint函数
};

#endif // BRIDGE_H
