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
	QString strName;    // 名称
	QString strDesc;    // 描述
    double   fLng;      // 经度
    double	fLat;       // 纬度
	int		nValue;     //
    int rotation;       //
}Light_t;

class bridge : public QObject
{
Q_OBJECT
public:
    static bridge* instance();  // 返回bridge对象
private:
    bridge();
    double _fLng = 0, _fLat = 0;    // 经纬度信息
	QList<Light_t> m_lightList; // 所有WayPoints的list
	void SetLightValue(QString strName, int nValue);
signals:    // 信号声明区
    void sigUpdateLight();
    void removePoints();
	void sigLightStatus(const QString& strName, int nvalue);
    void toRecvMsg(const QString& msg); //收到数据
    void targetPointReceived(const QString& msg);
    void rotateNavPoint(int rot);
public slots:   // 槽声明区
    void onLightOn(QString strName);
	void onLightOff(QString strName);
	void onUpdateData();
    void jsSendMsg(const QString& msg);     // 接收到页面发送来的消息
    void jsSendTargetPoint(const QString& msg);
	QVariantList	GetLightData();
public:
    QList<Light_t>	returnWayPointList();   // 返回所有的WayPoints的list
    Light_t			AddLight(int rot);  // 根据导航旋转信息生成Light_t对象
	const QList<Light_t>& GetLightList(){return m_lightList;}
    void newPoint(double lng, double lat);  // 设置经纬度信息
    void removeAllPoints(); // 移除所有WayPoints信息
    void setNavPointRotate(int rot);

};

#endif // BRIDGE_H
