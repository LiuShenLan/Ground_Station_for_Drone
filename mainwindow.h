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

#define CAM_LOAD_DRONE_CAMERA   0
#define CAM_LOAD_PC_CAMERA      1
#define CAM_LOAD_PC_VIDEO       "/home/lsl/data/project/UAV/dataset/video/282.mp4"

#define READ_FRAME_PIC_TEMP_PATH    "/home/lsl/data/project/UAV/dataset/QT image/014.jpg"
#define READ_FRAME_PIC_PATH         "/home/lsl/data/project/UAV/dataset/QT image/014.jpg"

#define QFILE_CAR_DETECT_NUMBER   "/home/lsl/data/project/UAV/code/Ground_Station_for_Drone/Qfile/number.txt"

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
    double   fLng;
    double	fLat;
    double direction; // the map_direct, comes from the Baidu Map route plan
    double manul_direction; // 手动控制 顺时针标记前进角度

protected:
    void keyPressEvent(QKeyEvent *);
//    void keyReleaseEvent(QKeyEvent *);

private slots:  // 槽声明区
    void onTurnLeftButton();
    void onTurnRightButton();
    void onGoStraightButton();
    void on_pushButton_clicked();
    void timeCountsFunction();
    void callJava();
    void onBtnLightOn();
    void onBtnLightOff();
    void onBtnAddLight();   // 添加WayPoints
    void onGoButton();      // 向TCP发送所有的WayPoints信息
    void onClearAllPoint(); // 移除所有WayPoints信息
    void onTimeOut();   // 空定时中断函数
    void onTakeoffButton(); // Takeoff按钮,起飞
    void onReleaseYawSlider();
//    void onReleaseThrottleSlider();
    void onReleasePitchSlider();
    void onReleaseRollSlider();
    void onReleaseThrottleSlider();
    void onEnableVirtualStickButton();  // 允许无人机虚拟控制
    void onDisableVirtualStickButton(); // 禁止无人机虚拟控制
    void sendVirtualStickCommand(); // 向TCP发送无人机虚拟控制信息
    void onSetRoll();   // 设置Roll滑块信息
    void onReleaseNavSlider();

private:
    Ui::MainWindow *ui;
    Ui::Server *server_;
    QDockWidget* dock_server_;

    QTimer* timer_1;
    QTimer* timer_2;
    void onRecvdMsg(const QString msg); // 接受信息并设置经纬度信息
    void onRecvTargetPoint(const QString msg);  // 计算目标点方位并赋值
    QTimer m_timer; // 500ms定时器
    QTimer* virtualStickTimer;  // 100ms定时器，定时中断中向无人机发送虚拟控制信息
    void InitForm();
    bridge *bridgeins;
    void sendWayPoint();    // 向TCP发送所有的WayPoints信息

    void InitVirtualStickControl(); // 无人机虚拟控制初始化
    QTcpServer tcpServer_for_python_controller;
    QTcpSocket *tcpSocket_for_python_controller;

    void acceptConnection_for_python_controller();  // 从TCP接收数据并处理
    void updateCommand_from_python_controller();    // 接受socket数据并设置信息

private slots:  // 槽声明区
    void readFarme();       // 读取当前帧信息
    void closeCamara();     // 关闭摄像头。

    void on_detect_carButton_clicked();
//    void paintEvent(QPaintEvent *event);
    void on_track_carButton_clicked();

    void on_do_nothing_clicked();

private:
    QTimer    *timer;   // 读取摄像头信息定时器
    QImage    *imag;
    cv::VideoCapture cam;   // 从 无人机摄像头/本地视频 读取图片
    cv::Mat  frame;

    int rollBias = 0, pitchBias = 0, yawBias = 0, throttleBias = 0;
};

#endif // MAINWINDOW_H
