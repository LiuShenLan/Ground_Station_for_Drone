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

#include "bridge.h"

// 安全模式最大速度
#define SAFE_MODE_YAW_THRESHOLD     80.0
#define SAFE_MODE_PITCH_THRESHOLD   30.0
#define SAFE_MODE_POLL_THRESHOLD    50.0

// 摄像头数据读取目标
#define CAM_LOAD_PC_CAMERA      0
#define CAM_LOAD_DRONE_CAMERA   2
#define CAM_LOAD_PC_VIDEO       "../../dataset/dronet test video/all copy.mp4"
// 摄像头读取选择
#define CAM_LOAD    CAM_LOAD_DRONE_CAMERA
// 摄像头显示选择
#define CAM_SHOW_DETECT_CAMERA_FLAG true
#define CAM_SHOW_DETECT_CAMERA      "../../dataset/hostData/detece result/show_realTimeImg.jpg"

// 摄像头数据保存路径
#define READ_FRAME_PIC_TEMP_PATH    "../../dataset/hostData/realTimeImg_temp.jpg"
#define READ_FRAME_PIC_PATH         "../../dataset/hostData/realTimeImg.jpg"

// 无人机信息Json文件保存路径
#define SAVE_JSON_TEMP_PATH     "../../dataset/hostData/uav_temp.json"
#define SAVE_JSON_RENAME_PATH   "../../dataset/hostData/uav.json"

// WayPoints文件保存路径
#define SAVE_WAYPOINTS_PATH   "../../dataset/hostData/wayPoints.json"

// Car detect保存路径
#define QFILE_CAR_DETECT_NUMBER "./Qfile/number.txt"
#define QFILE_CAR_DETECT_TRACK  "./Qfile/detect_track.txt"

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
    double fLng;  // 经度
    double fLat;   // 纬度
    double direction; // the map_direct, comes from the Baidu Map route plan
    double manul_direction; // 手动控制 顺时针标记前进角度
    bool setRollFlag = false;

    double collPred = 1.0;      // coll 预测值
    double collThreshold = 0.5; // 障碍预测阈值
    bool isCollFlag = false;    // 前方是否是障碍

protected:
    void keyPressEvent(QKeyEvent *);    // 虚拟控制 键盘输入控制
//    void keyReleaseEvent(QKeyEvent *);

private:
    Ui::MainWindow *ui;
    Ui::Server *server_;
    QDockWidget* dock_server_;
    bridge *bridgeins;

    // Init
    void InitForm();    // 窗口初始化
    void InitVirtualStickControl(); // 无人机虚拟控制初始化

    // 地图显示
    QTimer* timer_1;    // 100ms定时器 读取GPS与无人机信息并显示
    QTimer* timer_2;    // 200ms定时器 调用JAVA程序 在地图上显示导航点

    // 虚拟控制
    QTimer* virtualStickTimer;  // 100ms定时器 向无人机发送虚拟控制信息

    // TCP dronet
    QTcpServer tcpServer_for_python_controller;     // TCP python控制server
    QTcpSocket *tcpSocket_for_python_controller;    // TCP python控制socket
    void acceptConnection_for_python_controller();  // TCP 接受信息并设置虚拟控制与方向数值
    void onRecvTargetPoint(const QString& msg);     // 根据TCP接收到的信息计算目标点方位并设置方向值
    void updateCommand_from_python_controller();    // 接受python socket数据并设置虚拟控制与方向信息
    void sendWayPoint();                            // 向TCP发送所有的WayPoints信息
    void onRecvdMsg(const QString& msg);            // 接受信息并设置经纬度信息

    // TCP coll pred
    QTcpServer tcpServer_for_coll_pred;     // TCP python控制server
    QTcpSocket *tcpSocket_for_coll_pred;    // TCP python控制socket
    void acceptConnection_for_coll_pred();  // TCP 接受信息并设置虚拟控制与方向数值
    void update_coll_pred();                // 接受coll pred数据并发送
    QTimer* collPredTimer;                  // 100ms定时器 向无人机发送障碍预测信息

    // 摄像头
    QTimer *timer;  // 50ms定时器 读取摄像头信息器
    cv::VideoCapture cam;   // 从 摄像头/本地视频 读取图片
    cv::Mat frame;  // 图片帧

    int rollBias = 0, pitchBias = 0, yawBias = 0, throttleBias = 0; // 虚拟控制 键盘输入偏差

    // 不明定义
    QImage    *imag;
    QTimer m_timer; // 500ms定时器 用途不明

private slots:  // 槽声明区
    // 人工导航
    void onTurnLeftButton();    // 人工导航 左转按钮 Turn left
    void onTurnRightButton();   // 人工导航 右转按钮 Turn right
    void onGoStraightButton();  // 人工导航 直行按钮 Go straight

    // 刷新地图、GPS与无人机信息
    void on_GPSMapRefreshBtn_clicked();   // 刷新地图、GPS与无人机信息
    void timeCountsFunction();  // 读取GPS与无人机信息并显示
    void callJava();    // 调用JAVA程序在地图上显示导航点

    // WayPoints
    void onBtnLightOn();    // 将WayPoints下拉菜单中显示的nValue设置为1
    void onBtnLightOff();   // 将WayPoints下拉菜单中显示的nValue设置为0
    void onBtnAddLight();   // 根据导航点方向滑块添加WayPoints
    void onGoButton();      // 向TCP发送所有的WayPoints信息
    void onClearAllPoint(); // 移除所有WayPoints信息
    void onReleaseNavSlider();  // 设置导航点方向
    void onTakeoffButton(); // 向TCP发送起飞命令
    void onLandButton();    // 向TCP发送降落命令
    void onSaveButton();    // 保存WayPoints信息
    void onLoadButton();    // 读取WayPoints信息
    static QJsonObject wayPointsToJson(const Light_t& point); // wayPoints转换为Json
    static Light_t jsonToWayPoints(const QJsonObject& jsonWayPoints); // Json转换为wayPoints

    // 无人机虚拟控制
    void onEnableVirtualStickButton();  // 允许无人机虚拟控制
    void sendVirtualStickCommand();     // 向TCP发送无人机虚拟控制信息
    void onDisableVirtualStickButton(); // 禁止无人机虚拟控制
    void onReleaseYawSlider();          // 清除虚拟控制航向数值
    void onReleasePitchSlider();        // 清除虚拟控制俯仰数值
    void onReleaseRollSlider();         // 清除虚拟控制横滚数值
    void onSetRoll();                   // 设置Roll滑块信息
    void onReleaseThrottleSlider();     // 清除虚拟控制油门数值
    void onVirtualStickResetButton();   // 重置虚拟控制

    // 无人机障碍预测
    void onEnableCollButton();  // 允许无人机障碍预测
    void sendCollPredCommand(); // 向TCP发送无人机障碍预测信息
    void onDisableCollButton(); // 禁止无人机障碍预测
    void onSetCollThreshold();  // 设置障碍预测阈值

    // 摄像头
    void readFarme();   // 读取摄像头信息
    void closeCamara(); // 关闭摄像头，释放资源，必须释放

    // Car setection
    void on_carDetectBtn_clicked();         // 向car detect track 文件写入"detect"
    void on_carDetectTrackCarBtn_clicked(); // 向car detect track 文件写入"track"
    void on_carDetectWaitBtn_clicked();     // 向car detect track 文件写入"wait"

    // 不明函数
    void onTimeOut();   // 空定时中断函数
//    void paintEvent(QPaintEvent *event);
};

#endif // MAINWINDOW_H
