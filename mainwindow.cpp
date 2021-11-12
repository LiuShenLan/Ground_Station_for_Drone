#include "mainwindow.h"
#include "ui_mainwindow.h"  // Qt编译生成的与UI文件mainwindow.ui对应的类定义文件

#include <QDebug>
#include <math.h>

//#inMainWindowclude<cv.h>
#include<opencv2/opencv.hpp>

#include <QCoreApplication>
#include <Python.h>

class MySlider : public QSlider {
public:
  explicit MySlider(QWidget *parent = 0);

protected:
  void mousePressEvent(QMouseEvent *event) {
    //获取点击触发前的值
    const int value = this->value();
    //调用父类的鼠标点击处理事件
    QSlider::mousePressEvent(event);
    setValue(value);
    //向父窗口发送自定义事件event type，这样就可以在父窗口中捕获这个事件进行处理
    QEvent evEvent(static_cast<QEvent::Type>(QEvent::User + 1));
    QCoreApplication::sendEvent(parentWidget(), &evEvent);
  }
};

// Init
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),    // 执行父类QMainWindow的构造函数
    ui(new Ui::MainWindow)  // 创建一个Ui::MainWindow类的对象
{
    ui->setupUi(this);  // 执行Ui::MainWindow类的setupUi()函数，实现窗口的生成与各种属性的设置、信号与槽的关联
    InitForm();
    InitVirtualStickControl();

    // 左侧listen栏
    server_ = new Ui::Server(this);
    dock_server_ = new QDockWidget("Info", this);
    dock_server_->setWidget(server_);
    addDockWidget(Qt::LeftDockWidgetArea, dock_server_);

    //dock_server_->setFloating(1);
    manul_direction = 0;

    // 加载摄像头或本地视频
    cam = cv::VideoCapture(CAM_LOAD);
    if(!cam.isOpened())
        std::cerr << "Can't open camera!" <<std::endl;

    timer = new QTimer(this);
    timer->start(50);              // 开始计时，超时则发出timeout()信号，读取摄像头信息

    // Zoom与Nav point滑动块初始化
    ui->camZoomSlider->setMaximum(1000);
    ui->camZoomSlider->setMinimum(0);
    ui->camZoomSlider->setValue(1000);
    ui->navigationPointDirectSlider->setMaximum(365);
    ui->navigationPointDirectSlider->setMinimum(0);
    ui->navigationPointDirectSlider->setValue(0);

    connect(timer, SIGNAL(timeout()), this, SLOT(readFarme()));  // 时间到，读取当前摄像头信息
}
void MainWindow::InitForm()
{
    QWebChannel *channel = new QWebChannel(this);
    bridgeins = bridge::instance(); // 返回bridge对象
    channel->registerObject("bridge", (QObject*)bridgeins); // 将bridgeins对象注册到channel
    ui->mapShow->page()->setWebChannel(channel);
    ui->mapShow->page()->load(QUrl::fromLocalFile(qApp->applicationDirPath() + "/bin/index.html"));

    // GPS Data初始化
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));

    // WayPoints初始化
    const QList<Light_t>& list = bridgeins->GetLightList();
    int nCount = list.count();
    for(int i=0; i<nCount; i++)
    {
        Light_t tLight = list[i];
        ui->wayPointsComboBox->addItem(tLight.strDesc, tLight.strName);   // 在WayPoints下拉菜单中添加信息
    }
    ui->wayPointsComboBox->setCurrentIndex(0);
    connect(ui->wayPointsAddBtn, SIGNAL(clicked()), this, SLOT(onBtnAddLight()));
//    connect(ui->btnLightOn, SIGNAL(clicked()), this, SLOT(onBtnLightOn()));
//    connect(ui->btnLightOff, SIGNAL(clicked()), this, SLOT(onBtnLightOff()));
    connect(ui->wayPointsTakeOffBtn, SIGNAL(clicked()), this, SLOT(onTakeoffButton()));
    connect(ui->wayPointsGoBtn, SIGNAL(clicked()), this, SLOT(onGoButton()));
    connect(ui->wayPointsClearBtn, SIGNAL(clicked()), this, SLOT(onClearAllPoint()));

    // GPS信息初始化
    ui->GPSLatitudeLineEdit->setMinimumWidth(10);
    ui->GPSLongitudeLineEdit->setMinimumWidth(10);
    connect(bridgeins, &bridge::toRecvMsg, this, &MainWindow::onRecvdMsg);

    // 无人机方向控制
    connect(ui->manualDirectTurnLeftBtn,SIGNAL(clicked()), this, SLOT(onTurnLeftButton()));
    connect(ui->manualDirectTurnRightBtn,SIGNAL(clicked()), this, SLOT(onTurnRightButton()));
    connect(ui->manualDirectGoStraightBtn,SIGNAL(clicked()), this, SLOT(onGoStraightButton()));

    // 不明定时器
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimeOut()));
    m_timer.start(500);

    //this->showMaximized();
}
void MainWindow::InitVirtualStickControl()
{
    tcpSocket_for_python_controller = new QTcpSocket(this);
    // 无人机虚拟控制
    connect(ui->virtualStickEnableBtn, SIGNAL(clicked()), this, SLOT(onEnableVirtualStickButton()));
    connect(ui->virtualStickDisableBtn, SIGNAL(clicked()), this, SLOT(onDisableVirtualStickButton()));
    connect(ui->virtualStickRollSetBtn, SIGNAL(clicked()), this, SLOT(onSetRoll()));
    virtualStickTimer = new QTimer(this);   // 实例化定时器
    ui->virtualStickDisableBtn->setEnabled(false);
    // 虚拟控制滑块
    ui->virtualStickYawSlider->setMaximum(1000);
    ui->virtualStickYawSlider->setMinimum(-1000);
    ui->virtualStickYawSlider->setValue(0);

    ui->virtualStickPitchSlider->setMaximum(1000);
    ui->virtualStickPitchSlider->setMinimum(-1000);
    ui->virtualStickPitchSlider->setValue(0);

    ui->virtualStickRollSlider->setMaximum(1000);
    ui->virtualStickRollSlider->setMinimum(-1000);
    ui->virtualStickRollSlider->setValue(0);

    ui->virtualStickThrottleSlider->setMaximum(1000);
    ui->virtualStickThrottleSlider->setMinimum(-1000);
    ui->virtualStickThrottleSlider->setValue(0);

    // 释放滑块，设置信息
    connect(ui->virtualStickYawSlider, SIGNAL(sliderReleased()), this, SLOT(onReleaseYawSlider()));
    connect(ui->virtualStickPitchSlider, SIGNAL(sliderReleased()), this, SLOT(onReleasePitchSlider()));
    connect(ui->virtualStickRollSlider, SIGNAL(sliderReleased()), this, SLOT(onReleaseRollSlider()));
    connect(ui->virtualStickThrottleSlider, SIGNAL(sliderReleased()), this, SLOT(onReleaseThrottleSlider()));

    // WayPoints
    connect(ui->navigationPointDirectSlider, SIGNAL(sliderMoved(int)), this, SLOT(onReleaseNavSlider()));

    // TCP
    if(!tcpServer_for_python_controller.isListening() && !tcpServer_for_python_controller.listen(QHostAddress::LocalHost, 5555))
    {
        qDebug() <<"error_in_sever_for_receive"<< tcpServer_for_python_controller.errorString();
        close();
        return;
    }
    if(tcpServer_for_python_controller.isListening())
    {
        qDebug()<<"=============start listening to python controller==========";
    }
    connect(&tcpServer_for_python_controller, &QTcpServer::newConnection, this, &MainWindow::acceptConnection_for_python_controller);

}
MainWindow::~MainWindow()
{
    delete ui;
}

// 无人机人工导航控制按钮
void MainWindow::onTurnLeftButton()
{
    manul_direction = 300;
}
void MainWindow::onTurnRightButton()
{
    manul_direction = 60;
}
void MainWindow::onGoStraightButton()
{
    manul_direction = 0;
}

// 刷新地图、GPS与无人机信息
void MainWindow::on_pushButton_clicked()
{
    timer_1 = new QTimer(this);
    timer_1->start(100);

    timer_2 = new QTimer(this);
    timer_2->start(200);

    bridgeins->setNavPointRotate(int(ui->navigationPointDirectSlider->value()));

    connect(timer_1,SIGNAL(timeout()),this,SLOT(timeCountsFunction())); // disable if you want to test the map point.
    connect(timer_2,SIGNAL(timeout()),this,SLOT(callJava()));
}
void MainWindow::timeCountsFunction()
{
    // 设置GPS经纬度信息
    ui->GPSLongitudeLineEdit->setText(QString::number(server_->jsonGPS["longitude"].toDouble(), 10, 8));
    //qDebug() <<"\n->"<< server_->jsonGPS["altitude"].toString();
    ui->GPSLatitudeLineEdit->setText(QString::number(server_->jsonGPS["latitude"].toDouble(), 10, 8));
    ui->GPSHeight->setText(QString::number(server_->jsonGPS["altitude"].toDouble()));
    // 设置无人机速度信息
    ui->uavStatusVelocityH->setText(QString::number(sqrt(pow(server_->jsonGPS["velocityY"].toDouble(),2)+pow(server_->jsonGPS["velocityX"].toDouble(), 2))));
    ui->uavStatusVelocityV->setText(QString::number(server_->jsonGPS["velocityZ"].toDouble()));
    // 设置无人机电池信息
    ui->uavStatusBattery->setText(QString::number(server_->jsonBattery["BatteryEnergyRemainingPercent"].toDouble()));
}
void MainWindow::callJava()
{
    QString strJs_ = "myFunction(%1, %2, ";
    strJs_ += QString::number(server_->jsonGPS["yaw"].toDouble(), 10, 1);
//    strJs_ += QString::number(10.5, 10, 1);
    strJs_ += ")";
//    QString strJs = strJs_
//            .arg(ui->GPSLongitudeLineEdit->text().toDouble()*0.01+109.03525)
//            .arg(ui->GPSLatitudeLineEdit->text().toDouble()*0.01+26.6564);
    QString strJs = strJs_
            .arg(ui->GPSLongitudeLineEdit->text().toDouble()+0.0126,0,10,8)
            .arg(ui->GPSLatitudeLineEdit->text().toDouble()+0.0062,0,10,8);
    ui->mapShow->page()->runJavaScript(strJs);
    //qDebug()<<strJs;
}

// WayPoints
void MainWindow::onBtnLightOn()
{
    QString strName = ui->wayPointsComboBox->currentData().toString();
    bridgeins->onLightOn(strName);
}
void MainWindow::onBtnLightOff()
{
    QString strName = ui->wayPointsComboBox->currentData().toString();
    bridgeins->onLightOff(strName);
}
void MainWindow::onBtnAddLight()
{
    // 根据导航点方向滑块生成Light_t对象
    Light_t tLight = bridgeins->AddLight(int(ui->navigationPointDirectSlider->value()));
    // 向WayPoints下拉菜单中写入信息
    ui->wayPointsComboBox->addItem(tLight.strDesc, tLight.strName);
    ui->wayPointsComboBox->setCurrentIndex(ui->wayPointsComboBox->count()-1);
    bridgeins->onUpdateData();
}
void MainWindow::onGoButton()
{
    sendWayPoint();
}
void MainWindow::sendWayPoint()
{
    QList<Light_t> wayPointList = bridgeins->returnWayPointList();  // 获取所有的WayPoints信息
    QJsonObject jsonToSend;
    jsonToSend.insert("mission", 1);            //takeoff = 0, waypoint = 1
    jsonToSend.insert("altitude", ui->wayPointsHeightLineEdit->text().toDouble());
    jsonToSend.insert("way_point_num", wayPointList.size());
    for(int i = 0; i < wayPointList.size(); i++)
    {
        jsonToSend.insert(QString::number(i)+"Lng", wayPointList.at(i).fLng - 0.0126);
        jsonToSend.insert(QString::number(i)+"Lat", wayPointList.at(i).fLat - 0.0062);
    }
    QString str = QString(QJsonDocument(jsonToSend).toJson());
    //qDebug()<<str;
    server_->sendMessage(str);
}
void MainWindow::onClearAllPoint()
{
    if(ui->wayPointsComboBox->count()>=1)
    {
        ui->wayPointsComboBox->clear();
        bridgeins->removeAllPoints();
    }
}
void MainWindow::onReleaseNavSlider()
{
    bridgeins->setNavPointRotate(int(ui->navigationPointDirectSlider->value()));
}
void MainWindow::onTakeoffButton()
{
    QJsonObject jsonToSend;
    jsonToSend.insert("mission", 0);
    QString str = QString(QJsonDocument(jsonToSend).toJson());
    qDebug()<<str;
    server_->sendMessage(str);
}

// 无人机虚拟控制
void MainWindow::onEnableVirtualStickButton()
{
    ui->virtualStickEnableBtn->setEnabled(false);
    ui->virtualStickDisableBtn->setEnabled(true);
    virtualStickTimer->start(100);
    connect(virtualStickTimer, SIGNAL(timeout()), this, SLOT(sendVirtualStickCommand()));
}
void MainWindow::sendVirtualStickCommand()
{
    QJsonObject jsonToSend_0;
    jsonToSend_0.insert("mission", 2);            //takeoff = 0, waypoint = 1, virtual stick = 2, disable virtual stick = 3
    jsonToSend_0.insert("yaw", double(ui->virtualStickYawSlider->value())/1000);
    jsonToSend_0.insert("roll", double(ui->virtualStickRollSlider->value())/1000);
    jsonToSend_0.insert("pitch", double(ui->virtualStickPitchSlider->value())/1000);
    jsonToSend_0.insert("throttle", double(ui->virtualStickThrottleSlider->value())/1000);

    QString str = QString(QJsonDocument(jsonToSend_0).toJson());
//    qDebug()<<str;
    server_->sendMessage(str);
}
void MainWindow::onDisableVirtualStickButton()
{
    disconnect(virtualStickTimer, SIGNAL(timeout()),0,0);
    virtualStickTimer->stop();
    ui->virtualStickYawSlider->setValue(0);
    ui->virtualStickPitchSlider->setValue(0);
    ui->virtualStickRollSlider->setValue(0);

//    QJsonObject jsonToSend_0;
//    jsonToSend_0.insert("mission", 2);            //takeoff = 0, waypoint = 1, virtual stick = 2, disable virtual stick = 3
//    jsonToSend_0.insert("yaw", ui->virtualStickYawSlider->value()/1000);
//    jsonToSend_0.insert("roll", ui->virtualStickRollSlider->value()/1000);
//    jsonToSend_0.insert("pitch", ui->virtualStickPitchSlider->value()/1000);
//    jsonToSend_0.insert("throttle", double(ui->virtualStickThrottleSlider->value())/1000);

//    QString str = QString(QJsonDocument(jsonToSend_0).toJson());
//    qDebug()<<str;
//    server_->sendMessage(str);

    QJsonObject jsonToSend_1;
    jsonToSend_1.insert("mission", 3);
    jsonToSend_1.insert("yaw", ui->virtualStickYawSlider->value()/1000);
    jsonToSend_1.insert("roll", ui->virtualStickRollSlider->value()/1000);
    jsonToSend_1.insert("pitch", ui->virtualStickPitchSlider->value()/1000);
    jsonToSend_1.insert("throttle", double(ui->virtualStickThrottleSlider->value())/1000);

    QString str  = QString(QJsonDocument(jsonToSend_1).toJson());
//    qDebug()<<str;
    server_->sendMessage(str);

    ui->virtualStickEnableBtn->setEnabled(true);
    ui->virtualStickDisableBtn->setEnabled(false);
}
void MainWindow::onReleaseYawSlider()
{
    ui->virtualStickYawSlider->setValue(0);
}
void MainWindow::onReleasePitchSlider()
{
    ui->virtualStickPitchSlider->setValue(0);
}
void MainWindow::onReleaseRollSlider()
{
    ui->virtualStickRollSlider->setValue(0);
}
void MainWindow::onSetRoll()
{
    ui->virtualStickRollSlider->setValue(65);
    ui->rollLabel->setText(tr("65"));
}
void MainWindow::onReleaseThrottleSlider()
{
    ui->virtualStickThrottleSlider->setValue(0);
}
void MainWindow::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
        case Qt::Key_W :    // W 增加油门
            throttleBias++;
            ui->virtualStickThrottleSlider->setValue(ui->virtualStickThrottleSlider->value()+throttleBias);
            ui->throttleBiasLabel->setText(QString::number(throttleBias));
            break;
        case Qt::Key_S :    // S 减少油门
            throttleBias--;
            ui->virtualStickThrottleSlider->setValue(ui->virtualStickThrottleSlider->value()+throttleBias);
            ui->throttleBiasLabel->setText(QString::number(throttleBias));
            break;
        case Qt::Key_A :    // A 向左偏航
            yawBias--;
            ui->yawBiasLabel->setText(QString::number(yawBias));
            break;
        case Qt::Key_D :    // D 向右偏航
            yawBias++;
            ui->yawBiasLabel->setText(QString::number(yawBias));
            break;
        case Qt::Key_I :    // I 增加横滚
            rollBias++;
            ui->rollBiasLabel->setText(QString::number(rollBias));
            break;
        case Qt::Key_K :    // K 减少横滚
            rollBias--;
            ui->rollBiasLabel->setText(QString::number(rollBias));
            break;
        case Qt::Key_J :    // J 减少俯仰
            pitchBias--;
            ui->pitchBiasLabel->setText(QString::number(pitchBias));
            break;
        case Qt::Key_L :    // L 增加俯仰
            pitchBias++;
            ui->pitchBiasLabel->setText(QString::number(pitchBias));
            break;
        default :
            break;
    }
}

/*********************************
********* 读取摄像头信息 ***********
**********************************/
void MainWindow::readFarme()
{
    cam >> frame;// 从摄像头中抓取并返回每一帧
    double radio = (double)ui->camZoomSlider->value()/1000;
    frame(cv::Rect(int((1-radio)/2*frame.size().width),int((1-radio)/2*frame.size().height),int(radio*frame.size().width),int(radio*frame.size().height))).copyTo(frame);
//    cv::rectangle(frame,cvPoint(20,200),cvPoint(200,300),cvScalar(255,0,0),1,1,0);
    //将抓取到的帧，转换为QImage格式。QImage::Format_RGB888不同的摄像头用不同的格式。
    QImage image = QImage(frame.data, frame.cols, frame.rows, static_cast<int>(frame.step), QImage::Format_RGB888).rgbSwapped().scaled(640,360,Qt::KeepAspectRatioByExpanding);
    cv::imwrite(READ_FRAME_PIC_TEMP_PATH,frame);
    if(access(READ_FRAME_PIC_PATH,F_OK))
        remove(READ_FRAME_PIC_PATH);
    rename(READ_FRAME_PIC_TEMP_PATH, READ_FRAME_PIC_PATH);

    // 显示detect后的图片
    if (CAM_SHOW_DETECT_CAMERA_FLAG) {
        frame = cv::imread(CAM_SHOW_DETECT_CAMERA, 1);    //读入一张本地图片
//        cv::cvtColor(frame,frame,cv::COLOR_BGR2RGB);//opencv读取图片按照BGR方式读取，为了正常显示，所以将BGR转为RGB
        image = QImage(frame.data, frame.cols, frame.rows, static_cast<int>(frame.step), QImage::Format_RGB888).rgbSwapped().scaled(640,320,Qt::KeepAspectRatioByExpanding);
    }
    ui->camShow->setPixmap(QPixmap::fromImage(image));  // 显示图片

//    ui->deepImg->setPixmap(QPixmap("/home/wwh/2.jpg").scaled(320,160));

    QFile my_file(QFILE_CAR_DETECT_NUMBER); //将QFile与相关文件关联
    if(!my_file.open(QIODevice::ReadOnly | QIODevice::Text)) //以只读和文本模式打开文件
    {
        qDebug() <<"Could not open file for Reading";
        return;
    }
    QTextStream outText(&my_file);  //将QTextStream与特定文件关联
    QString number = outText.readAll();  //读出QTextStream对象中所有内容
    my_file.close(); //关闭文件
    ui->carDetectNumberLineEdit->setText(number);
}
/*******************************
***关闭摄像头，释放资源，必须释放***
********************************/
void MainWindow::closeCamara()
{
    timer->stop();         // 停止读取数据。
    cam.release();//释放内存；
}

// TCP
void MainWindow::acceptConnection_for_python_controller()
{
    qDebug()<<"acceptConnection_for_python_controller";
    tcpSocket_for_python_controller = tcpServer_for_python_controller.nextPendingConnection();
    connect(bridgeins, &bridge::targetPointReceived, this, &MainWindow::onRecvTargetPoint);
    connect(tcpSocket_for_python_controller, &QTcpSocket::readyRead, this, &MainWindow::updateCommand_from_python_controller);
}
void MainWindow::onRecvTargetPoint(const QString msg)
{
    QStringList lst;
    lst = msg.split(',');
    double next_point_direction = 0;
    QString targetNum = lst[0];
    int targetRotation = lst[1].toInt();
//    bridgeins->onLightOn(targetNum);
    double targetLng = lst[2].toDouble();
    double targetLat = lst[3].toDouble();
    int arrayListLength = lst[4].toInt();
    //qDebug()<<fixed<<qSetRealNumberPrecision(7)<<targetLng<<" "<<targetLat;
    //qDebug()<<(ui->GPSLongitudeLineEdit->text().toDouble()+0.0126)<<" "<<(ui->GPSLatitudeLineEdit->text().toDouble()+0.0062);
//    qDebug()<<((targetLng - (ui->GPSLongitudeLineEdit->text().toDouble()+0.0125))/(targetLat - (ui->GPSLatitudeLineEdit->text().toDouble()+0.0062);
    // 计算方位角
    // first quadrant
    if((targetLng - (ui->GPSLongitudeLineEdit->text().toDouble()+0.0126))>0 && (targetLat - (ui->GPSLatitudeLineEdit->text().toDouble()+0.0062))>0)
        next_point_direction = atan2((targetLng - (ui->GPSLongitudeLineEdit->text().toDouble()+0.0126)), (targetLat - (ui->GPSLatitudeLineEdit->text().toDouble()+0.0062)))*180/3.14159;
    // second quadrant
    if((targetLng - (ui->GPSLongitudeLineEdit->text().toDouble()+0.0126))<0 && (targetLat - (ui->GPSLatitudeLineEdit->text().toDouble()+0.0062))>0)
        next_point_direction = 360 + atan2((targetLng - (ui->GPSLongitudeLineEdit->text().toDouble()+0.0126)), (targetLat - (ui->GPSLatitudeLineEdit->text().toDouble()+0.0062)))*180/3.14159;
    // third quadrant
    if((targetLng - (ui->GPSLongitudeLineEdit->text().toDouble()+0.0126))<0 && (targetLat - (ui->GPSLatitudeLineEdit->text().toDouble()+0.0062))<0)
        next_point_direction = 360 + atan2((targetLng - (ui->GPSLongitudeLineEdit->text().toDouble()+0.0125)), (targetLat - (ui->GPSLatitudeLineEdit->text().toDouble()+0.0062)))*180/3.14159;
    // forth quadrant
    if((targetLng - (ui->GPSLongitudeLineEdit->text().toDouble()+0.0126))>0 && (targetLat - (ui->GPSLatitudeLineEdit->text().toDouble()+0.0062))<0)
        next_point_direction = atan2((targetLng - (ui->GPSLongitudeLineEdit->text().toDouble()+0.0125)), (targetLat - (ui->GPSLatitudeLineEdit->text().toDouble()+0.0062)))*180/3.14159;

    next_point_direction = (double)targetRotation;
    qDebug()<<next_point_direction<<":"<<targetNum<<","<<arrayListLength;//server_->jsonGPS["yaw"].toDouble();
    direction = next_point_direction - server_->jsonGPS["yaw"].toDouble();
    if(direction>180)
        direction = direction - 360;
    else if(direction<-180)
        direction = 360 + direction;

    if(targetNum == QString("-1"))
        direction = 0;
}
void MainWindow::updateCommand_from_python_controller()
{
    // 读取数据
    qint64 len = tcpSocket_for_python_controller->bytesAvailable();
    QByteArray alldate = tcpSocket_for_python_controller->read(len);
    QTextCodec *utf8codec = QTextCodec::codecForName("UTF-8");
    QString utf8str = utf8codec->toUnicode(alldate.mid(2));
    //qDebug()<<"pthon_controller:"<<utf8str<<"\n";
    QStringList control_command = utf8str.split(",");
    // 设置虚拟控制滑块数值
    ui->virtualStickYawSlider->setValue(control_command.at(0).toDouble() + yawBias);
    ui->virtualStickPitchSlider->setValue(control_command.at(1).toDouble() + pitchBias);
//    qDebug()<<"yaw:"<<control_command.at(0).toDouble()<<"\n";
    ui->yawLabel->setText(control_command.at(0));
    ui->pitchLabel->setText(control_command.at(1));

    ui->virtualStickRollSlider->setValue(control_command.at(2).toDouble() + rollBias);
    ui->rollLabel->setText(control_command.at(2));

    // send the map_direct to python controller
    // 设置方向信息
    if(direction>180||direction<-180)
        direction = 0;
    if(ui->manualDirectCheckBox->isChecked())
        direction = manul_direction;

    // 显示方向信息
    if((direction>=0 && direction<30) || (direction>330 && direction<=360))
        ui->turnLabel->setText(tr("Go straight"));
    if((direction>=30 && direction<=90))
        ui->turnLabel->setText(tr("Turn right"));
    if((direction>=270 && direction<=330))
        ui->turnLabel->setText(tr("Turn left"));
    // 发送信息
    QString sWriteData = QString::number(direction);
    tcpSocket_for_python_controller->write(sWriteData.toUtf8());
}
void MainWindow::onRecvdMsg(QString msg)
{
    qDebug()<<QString("Received message：%1").arg(msg);
    QStringList lst;
    lst = msg.split(',');
    qDebug()<<lst;
    fLng = lst[0].toDouble();
    fLat = lst[1].toDouble();
    qDebug()<<fixed<<qSetRealNumberPrecision(7)<<fLng<<" "<<fLat;   // 设置实数精度
    bridgeins->newPoint(fLng,fLat);
}

// Car detection
void MainWindow::on_carDetectBtn_clicked()
{

    QFile my_file(QFILE_CAR_DETECT_TRACK);  //将QFile与相关文件关联
   if(!my_file.open(QIODevice::WriteOnly | QIODevice::Text))    //以只读和文本模式打开文件
   {
       qDebug() <<"Could not open file for Writing";
       return;
   }
   QString str = "detect";
   QTextStream in(&my_file);
   in << str << "\n";
   my_file.close(); //关闭文件
}
void MainWindow::on_carDetectTrackCarBtn_clicked()
{
    QFile my_file(QFILE_CAR_DETECT_TRACK);//将QFile与相关文件关联
   if(!my_file.open(QIODevice::WriteOnly | QIODevice::Text))//以只读和文本模式打开文件
   {
       qDebug() <<"Could not open file for Writing";
       return;
   }
   QString str = "track";
   QTextStream in(&my_file);
   in << str << "\n";
   my_file.close(); //关闭文件
}
void MainWindow::on_carDetectWaitBtn_clicked()
{
    QFile my_file(QFILE_CAR_DETECT_TRACK);//将QFile与相关文件关联
   if(!my_file.open(QIODevice::WriteOnly | QIODevice::Text))//以只读和文本模式打开文件
   {
       qDebug() <<"Could not open file for Writing";
       return;
   }
   QString str = "wait";
   QTextStream in(&my_file);
   in << str << "\n";
   my_file.close(); //关闭文件
}

// 不明函数
void MainWindow::onTimeOut()
{
//	bridgeins->onUpdateStatus();
}
