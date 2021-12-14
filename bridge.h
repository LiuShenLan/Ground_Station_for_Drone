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
    double fLng;    // 经度
    double fLat;    // 纬度
	int nValue;     //
    int rotation;   //
}Light_t;

class bridge : public QObject
{
    friend class MainWindow;
    Q_OBJECT
public:
    // Init
    static bridge* instance();  // 返回bridge对象
    // WayPoints
    Light_t AddLight(int rot);              // 根据导航旋转信息生成Light_t对象
    void newPoint(double lng, double lat);  // 设置经纬度信息
    QList<Light_t> returnWayPointList();    // 返回所有的WayPoints的list
    const QList<Light_t>& GetLightList(){return m_lightList;}   // 返回所有的WayPoints的list的引用
    void removeAllPoints();                 // 移除所有WayPoints信息
    void setNavPointRotate(int rot);        // 设置导航点方向
private:
    double _fLng = 0, _fLat = 0;    // 经纬度信息
	QList<Light_t> m_lightList;     // 所有WayPoints的list
    bridge();   // 空构造函数
	void SetLightValue(const QString& strName, int nValue);    // 将WayPoints中strName的nValue设置为给定值
signals:    // 信号声明区
    void sigUpdateLight();
    void removePoints();
	void sigLightStatus(const QString& strName, int nvalue);
    void toRecvMsg(const QString& msg);         //收到数据
    void targetPointReceived(const QString& msg);   // TCP 接收到目标点信息
    void rotateNavPoint(int rot);                   // 旋转导航点方向
public slots:   // 槽声明区
    // WayPoints
    void onLightOn(const QString& strName);    // 将WayPoints中的strName的nValue设置为1
	void onLightOff(const QString& strName);   // 将WayPoints中的strName的nValue设置为0
    QVariantList GetLightData();        // 返回WayPoints list的map形式
	void onUpdateData();                // WayPoints更新数据
    // 发射信号
    void jsSendMsg(const QString& msg); // 发射toRecvMsg信号
    void jsSendTargetPoint(const QString& msg); // 发射targetPointReceived信号
};

#endif // BRIDGE_H
