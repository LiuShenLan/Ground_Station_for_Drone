#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebChannel>
#include "server.h"
#include <QLabel>

#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>

#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>

#include <cmath>

#include "bridge.h"

// 安全模式最大速度
#define SAFE_MODE_YAW_THRESHOLD		80.0
#define SAFE_MODE_PITCH_THRESHOLD	30.0
#define SAFE_MODE_POLL_THRESHOLD	50.0

// 障碍预测由前进到停止所需要的连续flag跳变阈值
#define PRE_COLL_FLAG_IS_TRUE_COUNT_THRESHOLD	30

// 摄像头数据读取目标
#define CAM_LOAD_PC_CAMERA		0
#define CAM_LOAD_DRONE_CAMERA	2
#define CAM_LOAD_PC_VIDEO		"../../dataset/video/all copy.mp4"
// 摄像头读取选择
#define CAM_LOAD	CAM_LOAD_DRONE_CAMERA
// 摄像头显示选择
#define CAM_SHOW_DETECT_CAMERA_FLAG	true
#define CAM_SHOW_DETECT_CAMERA		"../../dataset/hostData/detectResult/show_realTimeImg.jpg"

// 摄像头数据保存路径
#define READ_FRAME_PIC_TEMP_PATH	"../../dataset/hostData/realTimeImg/realTimeImg_temp.jpg"
#define READ_FRAME_PIC_PATH			"../../dataset/hostData/realTimeImg/realTimeImg.jpg"

// 无人机信息Json文件保存路径
#define SAVE_JSON_TEMP_PATH		"../../dataset/hostData/uav_temp.json"
#define SAVE_JSON_RENAME_PATH	"../../dataset/hostData/uav.json"

// WayPoints文件保存路径
#define SAVE_WAYPOINTS_PATH	"../../dataset/hostData/wayPoints.json"

QT_BEGIN_NAMESPACE
namespace Ui {
	class MainWindow;
	class Server;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	// GPS坐标系相互转换
	// 无人机使用WGS84坐标，上位机html地图使用BD09坐标
	static QVector<double> WGS84ToBD09(double lng, double lat);	// WGS84坐标转BD09坐标
	static QVector<double> BD09ToWGS84(double lng, double lat);	// BD09坐标转WGS84坐标
	static QVector<double> BD09ToGCJ02(double lng, double lat);	// BD09坐标转GCJ02坐标
	static QVector<double> GCJ02ToBD09(double lng, double lat);	// GCJ02坐标转BD09坐标
	static QVector<double> WGS84ToGCJ02(double lng, double lat);// WGS84坐标转GCJ02坐标
	static QVector<double> GCJ02ToWGS84(double lng, double lat);// GCJ02坐标转WGS84坐标
	static double transformLat(double lng, double lat);	// 坐标转换辅助函数
	static double transformLng(double lng, double lat);	// 坐标转换辅助函数
	static constexpr double transformMagicNumberX_PI = 3.14159265358979324 * 3000.0 / 180.0;
	static constexpr double transformMagicNumberPI = 3.1415926535897932384626;
	static constexpr double transformMagicNumberA = 6378245.0;
	static constexpr double transformMagicNumberEE = 0.00669342162296594323;

private:
	// Init
	Ui::MainWindow *ui;
	Ui::Server *server_;
	QDockWidget* dock_server_;
	bridge *bridgeins;
	void InitForm();	// 窗口初始化
	void InitVirtualStickControl();	// 无人机虚拟控制初始化

	// 地图显示
	QTimer* timer_1;	// 100ms定时器 读取GPS与无人机信息并显示
	QTimer* timer_2;	// 200ms定时器 调用JAVA程序 在地图上显示导航点

	// WayPoints
	void sendWayPoint();	// 向MyUX发送所有的WayPoints信息
	void onRecvdMsg(const QString& msg);	// 接收html确定的经纬度信息并设置WayPoints临时经纬度信息(将wayPoints坐标由BD09转换为WGS84)

	// TCP coll pred
	QTcpServer tcpServerCollPred;	// coll pred TCP server
	QTcpSocket *tcpSocketCollPred;	// coll pred TCP socket
	qint64 coll_pre_receive_length = 12;	// TCP socket收到的coll pre数据长度(防止数据接受出错)
	double collPred = 1.0;		// coll 预测值
	double collThreshold = 0.97;// 障碍预测阈值
	bool isCollFlag = false;	// 前方是否是障碍
	bool isCollFlagPre = false;	// 前方是否是障碍上次预测值
	bool collIsSending = false;	// 是否正在发送障碍预测
	int preCollFlagIsTrueCount = 0;	// 障碍预测之前连续全部都是False的次数
	void acceptConnectionCollPred();	// TCP 接受信息并设置虚拟控制与方向数值
	void update_coll_pred();	// 接受coll pred数据并更新flag
	QTimer* collPredTimer;		// 定时器 向无人机发送障碍预测信息

	// 摄像头
	QTimer *timer;	// 50ms定时器 读取摄像头信息器
	cv::VideoCapture cam;	// 从 摄像头/本地视频 读取图片
	cv::Mat frame;	// 图片帧

	// 虚拟控制
	QTimer* virtualStickTimer;	// 100ms定时器 向无人机发送虚拟控制信息
	void keyPressEvent(QKeyEvent *);	// 无人机虚拟控制，键盘输入控制dronet
	int rollBias = 0, pitchBias = 0, yawBias = 0, throttleBias = 0;	// 虚拟控制 键盘输入偏差

	// 手动控制
	double direction;	// QT上位机显示前进方向无人机前进方向，从html地图地区或手动设置
	double manul_direction;	// 手动设置无人机前进方向，顺时针标记前进角度

	// TCP dronet
	bool setRollFlag = false;	// 手动设置dronet最大前进速度
	QTcpServer tcpServerDronet;	// dronet TCP server
	QTcpSocket *tcpSocketDronet;	// dronet TCP socket
	void acceptConnectionDrenet();	// TCP 接受信息并设置虚拟控制与方向数值
	void onRecvTargetPoint(const QString& msg);	// 接收html发送的WayPoints信息，计算前进方向并在QT上位机上显示
	void updateCommand_from_python_controller();	// 接受python socket数据并设置虚拟控制与方向信息

private slots:  // 槽声明区
	// 刷新地图、GPS与无人机信息
	void onGPSMapRefreshBtn();	// 刷新地图、GPS与无人机信息
	void timeCountsFunction();	// 读取GPS与无人机信息并显示
	void callJava();	// 调用JAVA程序在地图上显示导航点(将无人机坐标由WGS84转换为BD09)

	// WayPoints
	void onBtnAddLight();	// 根据临时WayPoint经纬度信息添加WayPoints
	void onBtnAddCurLight();// 根据无人机当前位置添加WayPoints
	void onGoButton();		// 向TCP发送所有的WayPoints信息
	void onClearAllPoint();	// 移除所有WayPoints信息
	void onReleaseNavSlider();	// 设置导航点方向
	void onSaveButton();	// 保存WayPoints信息
	void onLoadButton();	// 读取WayPoints信息
	void onContinueBtn();	// wayPoints断点续航
	void onBackBtn();		// 返航按钮
	static QJsonObject wayPointsToJson(const wayPoint& point);	// wayPoints转换为Json
	static wayPoint jsonToWayPoints(const QJsonObject& jsonWayPoints);	// Json转换为wayPoints

	// 无人机障碍预测
	void onEnableCollButton();	// 允许无人机障碍预测
	void sendCollPredCommand();	// 向TCP发送无人机障碍预测信息
	void onDisableCollButton();	// 禁止无人机障碍预测
	void onSetCollThreshold();	// 设置障碍预测阈值
	void onCollSendTrueButton();	// 发送无人机障碍true预测
	void onCollSendFalseButton();	// 允许无人机障碍false预测
	void collHoverCheckBoxStateChange(int);	// 悬停选择框

	// 摄像头
	void readFarme();	// 读取摄像头信息
	void closeCamara();	// 关闭摄像头，释放资源，必须释放

	// 无人机起降控制
	void onTakeoffButton();	// 向TCP发送起飞命令
	void onLandButton();	// 向TCP发送降落命令

	// 无人机虚拟控制
	void onEnableVirtualStickButton();	// 允许无人机虚拟控制
	void sendVirtualStickCommand();		// 向TCP发送无人机虚拟控制信息
	void onDisableVirtualStickButton();	// 禁止无人机虚拟控制
	void onReleaseYawSlider();	// 清除虚拟控制航向数值
	void onReleasePitchSlider();// 清除虚拟控制俯仰数值
	void onReleaseRollSlider();	// 清除虚拟控制横滚数值
	void onSetRoll();			// 设置Roll滑块信息
	void onReleaseThrottleSlider();	// 清除虚拟控制油门数值
	void onVirtualStickResetButton();	// 重置虚拟控制

	// 无人机人工导航控制
	void onTurnLeftButton();	// 人工导航 左转按钮 Turn left
	void onTurnRightButton();	// 人工导航 右转按钮 Turn right
	void onGoStraightButton();	// 人工导航 直行按钮 Go straight
};

#endif // MAINWINDOW_H
