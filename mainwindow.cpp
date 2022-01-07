#include "mainwindow.h"
#include "ui_mainwindow.h"  // Qt编译生成的与UI文件mainwindow.ui对应的类定义文件

#include <QDebug>
#include <cmath>

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
    QMainWindow(parent),        // 执行父类QMainWindow的构造函数
    ui(new Ui::MainWindow) {    // 创建一个Ui::MainWindow类的对象
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

    // Zoom与Nav point滑动块初始化
    ui->camZoomSlider->setMaximum(1000);
    ui->camZoomSlider->setMinimum(0);
    ui->camZoomSlider->setValue(1000);
    ui->navigationPointDirectSlider->setMaximum(360);
    ui->navigationPointDirectSlider->setMinimum(0);
    ui->navigationPointDirectSlider->setValue(0);
    connect(ui->navigationPointDirectSlider, SIGNAL(sliderMoved(int)), this, SLOT(onReleaseNavSlider()));

    // 加载摄像头或本地视频
    cam = cv::VideoCapture(CAM_LOAD);
    if(!cam.isOpened())
        std::cerr << "Can't open camera!" <<std::endl;

    timer = new QTimer(this);
    timer->start(50);              // 开始计时，超时则发出timeout()信号，读取摄像头信息
    connect(timer, SIGNAL(timeout()), this, SLOT(readFarme()));  // 时间到，读取当前摄像头信息
}
void MainWindow::InitForm() {
    auto *channel = new QWebChannel(this);
    bridgeins = bridge::instance(); // 返回bridge对象
    channel->registerObject("bridge", (QObject*)bridgeins); // 将bridgeins对象注册到channel
    ui->mapShow->page()->setWebChannel(channel);
    ui->mapShow->page()->load(QUrl::fromLocalFile(qApp->applicationDirPath() + "/bin/index.html"));

    // WayPoints初始化
    const QList<Light_t>& list = bridgeins->GetLightList();
    int nCount = list.count();
    for(int i=0; i<nCount; i++) {
        Light_t tLight = list[i];
        ui->wayPointsComboBox->addItem(tLight.strDesc, tLight.strName);   // 在WayPoints下拉菜单中添加信息
    }
    ui->wayPointsComboBox->setCurrentIndex(0);
    connect(ui->wayPointsAddBtn, SIGNAL(clicked()), this, SLOT(onBtnAddLight()));
    connect(ui->wayPointsGoBtn, SIGNAL(clicked()), this, SLOT(onGoButton()));
    connect(ui->wayPointsClearBtn, SIGNAL(clicked()), this, SLOT(onClearAllPoint()));

    connect(ui->wayPointsTakeOffBtn, SIGNAL(clicked()), this, SLOT(onTakeoffButton()));
    connect(ui->wayPointsLandBtn, SIGNAL(clicked()), this, SLOT(onLandButton()));
    connect(ui->wayPointsSaveBtn, SIGNAL(clicked()), this, SLOT(onSaveButton()));
    connect(ui->wayPointsLoadBtn, SIGNAL(clicked()), this, SLOT(onLoadButton()));

    // GPS Data初始化
    ui->GPSLatitudeLineEdit->setMinimumWidth(10);
    ui->GPSLongitudeLineEdit->setMinimumWidth(10);
    connect(bridgeins, &bridge::toRecvMsg, this, &MainWindow::onRecvdMsg);
    connect(ui->GPSMapRefreshBtn, SIGNAL(clicked()), this, SLOT(on_GPSMapRefreshBtn_clicked()));

    // 无人机方向控制
    connect(ui->manualDirectTurnLeftBtn,SIGNAL(clicked()), this, SLOT(onTurnLeftButton()));
    connect(ui->manualDirectTurnRightBtn,SIGNAL(clicked()), this, SLOT(onTurnRightButton()));
    connect(ui->manualDirectGoStraightBtn,SIGNAL(clicked()), this, SLOT(onGoStraightButton()));
}
void MainWindow::InitVirtualStickControl() {
    tcpSocket_for_python_controller = new QTcpSocket(this);
    // 无人机虚拟控制
    connect(ui->virtualStickEnableBtn, SIGNAL(clicked()), this, SLOT(onEnableVirtualStickButton()));
    connect(ui->virtualStickDisableBtn, SIGNAL(clicked()), this, SLOT(onDisableVirtualStickButton()));
    connect(ui->virtualStickRollSetBtn, SIGNAL(clicked()), this, SLOT(onSetRoll()));
    connect(ui->virtualStickResetBtn, SIGNAL(clicked()), this, SLOT(onVirtualStickResetButton()));
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
    ui->virtualStickThrottleSlider->setValue(throttleBias);

    // 释放滑块，设置信息
    connect(ui->virtualStickYawSlider, SIGNAL(sliderReleased()), this, SLOT(onReleaseYawSlider()));
    connect(ui->virtualStickPitchSlider, SIGNAL(sliderReleased()), this, SLOT(onReleasePitchSlider()));
    connect(ui->virtualStickRollSlider, SIGNAL(sliderReleased()), this, SLOT(onReleaseRollSlider()));
    connect(ui->virtualStickThrottleSlider, SIGNAL(sliderReleased()), this, SLOT(onReleaseThrottleSlider()));

    // TCP dronet
    if(!tcpServer_for_python_controller.isListening() && !tcpServer_for_python_controller.listen(QHostAddress::LocalHost, 5555)) {
        qDebug() <<"error_in_sever_for_receive"<< tcpServer_for_python_controller.errorString();
        close();
        return;
    }
    if(tcpServer_for_python_controller.isListening())
        qDebug()<<"=============start listening to dronet==========";
    connect(&tcpServer_for_python_controller, &QTcpServer::newConnection, this, &MainWindow::acceptConnection_for_python_controller);

    // TCP coll pred
    connect(ui->collControlEnableBtn, SIGNAL(clicked()), this, SLOT(onEnableCollButton()));
    connect(ui->collControlDisableBtn, SIGNAL(clicked()), this, SLOT(onDisableCollButton()));
    connect(ui->collThresholdSetBtn, SIGNAL(clicked()), this, SLOT(onSetCollThreshold()));
    connect(ui->collSendTrueBtn, SIGNAL(clicked()), this, SLOT(onCollSendTrueButton()));
    connect(ui->collSendFalseBtn, SIGNAL(clicked()), this, SLOT(onCollSendFalseButton()));
    collPredTimer = new QTimer(this);   // 实例化定时器
    ui->collControlDisableBtn->setEnabled(false);

    if(!tcpServer_for_coll_pred.isListening() && !tcpServer_for_coll_pred.listen(QHostAddress::LocalHost, 5556)) {
        qDebug() << "error_in_sever_for_receive" << tcpServer_for_coll_pred.errorString();
        close();
        return;
    }
    if(tcpServer_for_coll_pred.isListening())
        qDebug()<<"=============start listening to coll pred==========";
    connect(&tcpServer_for_coll_pred, &QTcpServer::newConnection, this, &MainWindow::acceptConnection_for_coll_pred);
}
MainWindow::~MainWindow() {
    delete ui;
}

// 无人机人工导航控制按钮
void MainWindow::onTurnLeftButton() {
    manul_direction = 300;
}
void MainWindow::onTurnRightButton() {
    manul_direction = 60;
}
void MainWindow::onGoStraightButton() {
    manul_direction = 0;
}

// 刷新地图、GPS与无人机信息
void MainWindow::on_GPSMapRefreshBtn_clicked() {
    timer_1 = new QTimer(this);
    timer_1->start(100);

    timer_2 = new QTimer(this);
    timer_2->start(200);

    bridgeins->setNavPointRotate(int(ui->navigationPointDirectSlider->value()));

    connect(timer_1,SIGNAL(timeout()),this,SLOT(timeCountsFunction())); // disable if you want to test the map point.
    connect(timer_2,SIGNAL(timeout()),this,SLOT(callJava()));
}
void MainWindow::timeCountsFunction() {
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
void MainWindow::callJava() {
    QString strJs_ = "myFunction(%1, %2, ";
    strJs_ += QString::number(server_->jsonGPS["yaw"].toDouble(), 10, 1);
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
void MainWindow::onBtnAddLight() {
    // 根据根据临时WayPoints经纬度信息生成Light_t对象
    Light_t tLight = bridgeins->AddLight(int(ui->navigationPointDirectSlider->value()));
    // 向WayPoints下拉菜单中写入信息
    ui->wayPointsComboBox->addItem(tLight.strDesc, tLight.strName);
    ui->wayPointsComboBox->setCurrentIndex(ui->wayPointsComboBox->count()-1);
    bridgeins->onUpdateData();
}
void MainWindow::onGoButton() {
    sendWayPoint();
}
void MainWindow::sendWayPoint() {
    QList<Light_t> wayPointList = bridgeins->returnWayPointList();  // 获取所有的WayPoints信息
    QJsonObject jsonToSend;
    jsonToSend.insert("mission", 1);            //takeoff = 0, waypoint = 1
    jsonToSend.insert("altitude", ui->wayPointsHeightLineEdit->text().toDouble());
    jsonToSend.insert("way_point_num", wayPointList.size());

	// 根据wayPoints朝向计算并发送无人机朝向
	int n = wayPointList.size();
	for (int i = 0; i < n; ++i) {
		jsonToSend.insert(QString::number(i)+"Lng", wayPointList.at(i).fLng - 0.0126);
		jsonToSend.insert(QString::number(i)+"Lat", wayPointList.at(i).fLat - 0.0062);
		int head = wayPointList.at(i).rotation;
		head = head > 180 ? head - 360 : head;
		jsonToSend.insert(QString::number(i)+"head", head);
	}

    QString str = QString(QJsonDocument(jsonToSend).toJson());
    //qDebug()<<str;
    server_->sendMessage(str);
}
void MainWindow::onRecvdMsg(const QString& msg) {
	qDebug()<<QString("Received message：%1").arg(msg);
	QStringList lst;
	lst = msg.split(',');
	qDebug()<<lst;
	fLng = lst[0].toDouble();
	fLat = lst[1].toDouble();
	qDebug()<<fixed<<qSetRealNumberPrecision(7)<<fLng<<" "<<fLat;   // 设置实数精度
	bridgeins->newPoint(fLng,fLat);
}
void MainWindow::onClearAllPoint() {
    if(ui->wayPointsComboBox->count()>=1) {
        ui->wayPointsComboBox->clear();
        bridgeins->removeAllPoints();
    }
}
void MainWindow::onReleaseNavSlider() {
    bridgeins->setNavPointRotate(int(ui->navigationPointDirectSlider->value()));
}
void MainWindow::onSaveButton() {
    QJsonArray jsonArray;
    QList<Light_t> wayPointList = bridgeins->returnWayPointList();  // 获取所有的WayPoints信息
    for (const auto & i : wayPointList)
        jsonArray.append(wayPointsToJson(i));

    QFile file(SAVE_WAYPOINTS_PATH);
    if (!file.open(QIODevice::ReadWrite)) {
        qDebug() << SAVE_WAYPOINTS_PATH << " open error";
        return;
    }
    file.resize(0);

    QJsonDocument jsonDoc;
    jsonDoc.setArray(jsonArray);
    file.write(jsonDoc.toJson());
    file.close();

    qDebug() << "save wayPoints to " << SAVE_WAYPOINTS_PATH;
}
void MainWindow::onLoadButton() {
    QFile file(SAVE_WAYPOINTS_PATH);
    if (!file.open(QIODevice::ReadWrite)) {
        qDebug() << SAVE_WAYPOINTS_PATH << " open error";
        return;
    }
    QJsonParseError jsonParserError{};
    QJsonDocument jsonDocument = QJsonDocument::fromJson(file.readAll(), &jsonParserError);

    if (!jsonDocument.isNull() && jsonParserError.error == QJsonParseError::NoError) {
        // 文件解析未出现错误
        onClearAllPoint();  // 清空WayPoints
        QJsonArray array = jsonDocument.array();
        for (int i = 0; i < array.count(); ++i) {
            QJsonObject jsonWayPoint = array.at(i).toObject();
            Light_t wayPoints = jsonToWayPoints(jsonWayPoint);
            bridgeins->m_lightList.append(wayPoints);
            bridgeins->s_count++;
            ui->wayPointsComboBox->addItem(wayPoints.strDesc, wayPoints.strName);
            ui->wayPointsComboBox->setCurrentIndex(ui->wayPointsComboBox->count()-1);
            bridgeins->onUpdateData();
        }
        qDebug() << "load wayPoints from " << SAVE_WAYPOINTS_PATH;
    } else {
        // 文件解析错误
        qDebug() << SAVE_WAYPOINTS_PATH << "文件解析失败";
        return;
    }
}
QJsonObject MainWindow::wayPointsToJson(const Light_t& point) {
    QJsonObject jsonWayPoints;
    jsonWayPoints.insert("strName", point.strName);
    jsonWayPoints.insert("strDesc", point.strDesc);
    jsonWayPoints.insert("fLng", point.fLng);
    jsonWayPoints.insert("fLat", point.fLat);
    jsonWayPoints.insert("nValue", point.nValue);
    jsonWayPoints.insert("rotation", point.rotation);
    return jsonWayPoints;
}
Light_t MainWindow::jsonToWayPoints(const QJsonObject& jsonWayPoints) {
    Light_t tLight;
    tLight.strName = jsonWayPoints.value("strName").toString();
    tLight.strDesc = jsonWayPoints.value("strDesc").toString();
    tLight.fLng = jsonWayPoints.value("fLng").toDouble();
    tLight.fLat = jsonWayPoints.value("fLat").toDouble();
    tLight.nValue = jsonWayPoints.value("nValue").toInt();
    tLight.rotation = jsonWayPoints.value("rotation").toInt();
    return tLight;
}

// 无人机起降控制
void MainWindow::onTakeoffButton() {
	QJsonObject jsonToSend;
	jsonToSend.insert("mission", 0);
	QString str = QString(QJsonDocument(jsonToSend).toJson());
	qDebug()<<str;
	server_->sendMessage(str);
}
void MainWindow::onLandButton() {
	QJsonObject jsonToSend;
	jsonToSend.insert("mission", 4);
	QString str = QString(QJsonDocument(jsonToSend).toJson());
	qDebug()<<str;
	server_->sendMessage(str);
}

// 无人机虚拟控制
void MainWindow::onEnableVirtualStickButton() {
    ui->virtualStickEnableBtn->setEnabled(false);
    ui->virtualStickDisableBtn->setEnabled(true);
    virtualStickTimer->start(100);
    connect(virtualStickTimer, SIGNAL(timeout()), this, SLOT(sendVirtualStickCommand()));
}
void MainWindow::sendVirtualStickCommand() {
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
void MainWindow::onDisableVirtualStickButton() {
    disconnect(virtualStickTimer, SIGNAL(timeout()),nullptr,nullptr);
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
void MainWindow::onReleaseYawSlider() {
    ui->virtualStickYawSlider->setValue(0);
}
void MainWindow::onReleasePitchSlider() {
    ui->virtualStickPitchSlider->setValue(0);
}
void MainWindow::onReleaseRollSlider() {
    ui->virtualStickRollSlider->setValue(0);
}
void MainWindow::onSetRoll() {
    setRollFlag = true;
    int rollInt;

    QString rollStr = ui->virtualStickRollLineEdit->text();
    if (rollStr.size() > 0)
        rollInt = rollStr.toInt();
    else
        rollInt = 50;

    ui->virtualStickRollSlider->setValue(rollInt);
    ui->rollLabel->setText(QString::number(rollInt));
    ui->virtualStickRollSetBtn->setEnabled(false);
}
void MainWindow::onReleaseThrottleSlider() {
    ui->virtualStickThrottleSlider->setValue(throttleBias);
}
void MainWindow::onVirtualStickResetButton() {
    rollBias = 0;
    pitchBias = 0;
    yawBias = 0;
    throttleBias = 0;

    ui->virtualStickRollSlider->setValue(0);
    ui->virtualStickPitchSlider->setValue(0);
    ui->virtualStickYawSlider->setValue(0);
    ui->virtualStickThrottleSlider->setValue(0);

    ui->rollLabel->setText(nullptr);
    ui->pitchLabel->setText(nullptr);
    ui->yawLabel->setText(nullptr);
    ui->throttleLabel->setText(nullptr);

    ui->rollBiasLabel->setText(nullptr);
    ui->pitchBiasLabel->setText(nullptr);
    ui->yawBiasLabel->setText(nullptr);
    ui->throttleBiasLabel->setText(nullptr);

    setRollFlag = false;
    ui->virtualStickRollSetBtn->setEnabled(true);
}
void MainWindow::keyPressEvent(QKeyEvent *e) {
    switch(e->key()) {
        case Qt::Key_W :    // W 增加油门
            throttleBias++;
            ui->virtualStickThrottleSlider->setValue(throttleBias);
            ui->throttleBiasLabel->setText(QString::number(throttleBias));
            break;
        case Qt::Key_S :    // S 减少油门
            throttleBias--;
            ui->virtualStickThrottleSlider->setValue(throttleBias);
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
void MainWindow::readFarme() {
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
}
/*******************************
***关闭摄像头，释放资源，必须释放***
********************************/
void MainWindow::closeCamara() {
    timer->stop();         // 停止读取数据。
    cam.release();//释放内存；
}

// TCP dronet
void MainWindow::acceptConnection_for_python_controller() {
    qDebug()<<"acceptConnection_for_python_controller";
    tcpSocket_for_python_controller = tcpServer_for_python_controller.nextPendingConnection();
    connect(bridgeins, &bridge::targetPointReceived, this, &MainWindow::onRecvTargetPoint);
    connect(tcpSocket_for_python_controller, &QTcpSocket::readyRead, this, &MainWindow::updateCommand_from_python_controller);
}
void MainWindow::onRecvTargetPoint(const QString& msg) {
    QStringList lst;
    lst = msg.split(',');
    double next_point_direction = 0;
    QString targetNum = lst[0]; // WayPoints列表中的WayPoint的索引
    int targetRotation = lst[1].toInt();    // WayPoint旋转角度
//    bridgeins->onLightOn(targetNum);
    double targetLng = lst[2].toDouble();   // WayPoint经度
    double targetLat = lst[3].toDouble();   // WayPoint纬度
    int arrayListLength = lst[4].toInt();   // WayPoints数目
    //qDebug()<<fixed<<qSetRealNumberPrecision(7)<<targetLng<<" "<<targetLat;
    //qDebug()<<(ui->GPSLongitudeLineEdit->text().toDouble()+0.0126)<<" "<<(ui->GPSLatitudeLineEdit->text().toDouble()+0.0062);
    //qDebug()<<((targetLng - (ui->GPSLongitudeLineEdit->text().toDouble()+0.0125))/(targetLat - (ui->GPSLatitudeLineEdit->text().toDouble()+0.0062);
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
void MainWindow::updateCommand_from_python_controller() {
    if (ui->virtualStickDronetCheckBox->isChecked()) {  // 允许uav_dronet控制
        // 读取数据
        qint64 len = tcpSocket_for_python_controller->bytesAvailable();
        QByteArray alldate = tcpSocket_for_python_controller->read(len);
        QTextCodec *utf8codec = QTextCodec::codecForName("UTF-8");
        QString utf8str = utf8codec->toUnicode(alldate.mid(2));
        //qDebug()<<"pthon_controller:"<<utf8str<<"\n";
        QStringList control_command = utf8str.split(",");

        // 设置虚拟控制滑块数值
        double yawSliderReceive = control_command.at(0).toDouble();
        double pitchSliderReceive = control_command.at(1).toDouble();
        double rollSliderReceive = control_command.at(2).toDouble();
        if (ui->virtualStickSafeModeCheckBox->isChecked()) {    // 安全模式，限制阈值
            yawSliderReceive = std::max(yawSliderReceive, -SAFE_MODE_YAW_THRESHOLD);
            yawSliderReceive = std::min(yawSliderReceive, SAFE_MODE_YAW_THRESHOLD);
            pitchSliderReceive = std::max(pitchSliderReceive, -SAFE_MODE_PITCH_THRESHOLD);
            pitchSliderReceive = std::min(pitchSliderReceive, SAFE_MODE_PITCH_THRESHOLD);
            rollSliderReceive = std::max(rollSliderReceive, -SAFE_MODE_POLL_THRESHOLD);
            rollSliderReceive = std::min(rollSliderReceive, SAFE_MODE_POLL_THRESHOLD);
        }

        ui->virtualStickYawSlider->setValue((int)yawSliderReceive + yawBias);
        ui->yawLabel->setText(QString::number(yawSliderReceive));

        ui->virtualStickPitchSlider->setValue((int)pitchSliderReceive + pitchBias);
        ui->pitchLabel->setText(QString::number(pitchSliderReceive));

        if (!setRollFlag) {
            ui->virtualStickRollSlider->setValue((int)rollSliderReceive + rollBias);
            ui->rollLabel->setText(QString::number(rollSliderReceive));
        }
    } else {    // 禁止uav_dronet控制
        // 设置虚拟控制滑块数值
        ui->virtualStickYawSlider->setValue(yawBias);
        ui->virtualStickPitchSlider->setValue(pitchBias);
        if (!setRollFlag) {
            ui->virtualStickRollSlider->setValue(rollBias);
            ui->rollLabel->setText(nullptr);
        }

        ui->yawLabel->setText(nullptr);
        ui->pitchLabel->setText(nullptr);
    }
    // 250954973

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

// TCP coll pred
void MainWindow::acceptConnection_for_coll_pred() {
    qDebug()<<"acceptConnection_for_coll_pred";
    tcpSocket_for_coll_pred = tcpServer_for_coll_pred.nextPendingConnection();
    connect(tcpSocket_for_coll_pred, &QTcpSocket::readyRead, this, &MainWindow::update_coll_pred);
}
void MainWindow::update_coll_pred() {
    // 读取数据
    QTextCodec *utf8codec = QTextCodec::codecForName("UTF-8");

    qint64 len = tcpSocket_for_coll_pred->bytesAvailable();
    QByteArray alldate = tcpSocket_for_coll_pred->read(coll_pre_receive_length);
    if (len > coll_pre_receive_length)
        tcpSocket_for_coll_pred->read(len - coll_pre_receive_length);
    QString utf8str = utf8codec->toUnicode(alldate);
    collPred = utf8str.toDouble();
	bool thisCollFlag = (collPred >= collThreshold);	// 本次障碍预测的结果

	// flag去抖
	if (isCollFlagPre && !thisCollFlag) {	// 由停止到前进的跳变
		++preCollFlagIsTrueCount;
		if (preCollFlagIsTrueCount > PRE_COLL_FLAG_IS_TRUE_COUNT_THRESHOLD) {	// 接收到了足够的前进的预测结果
			isCollFlag = thisCollFlag;	// 前进
			preCollFlagIsTrueCount = 0;
		} else
			isCollFlag = isCollFlagPre;	// 停止
	} else {
		isCollFlag = thisCollFlag;		// 取本次结果
		preCollFlagIsTrueCount = 0;
	}
	isCollFlagPre = isCollFlag;	// 保存本次预测结果

	// 显示结果
    ui->collPredLabel->setText(QString::number(collPred, 'f', 10));
    if (isCollFlag)
        ui->collFlagLabel->setText("true");
    else
        ui->collFlagLabel->setText("false");
}
void MainWindow::onEnableCollButton() {
    ui->collControlEnableBtn->setEnabled(false);
    ui->collControlDisableBtn->setEnabled(true);
    collPredTimer->start(500);
    connect(collPredTimer, SIGNAL(timeout()), this, SLOT(sendCollPredCommand()));
}
void MainWindow::sendCollPredCommand() {
    QJsonObject jsonToSend_0;
    jsonToSend_0.insert("mission", 5);
    jsonToSend_0.insert("isCollFlag", isCollFlag);

    QString str = QString(QJsonDocument(jsonToSend_0).toJson());
    server_->sendMessage(str);
}
void MainWindow::onDisableCollButton() {
    disconnect(collPredTimer, SIGNAL(timeout()),nullptr,nullptr);
    collPredTimer->stop();

    // 发送mission 6
//    QJsonObject jsonToSend_1;
//    jsonToSend_1.insert("mission", 6);
//    jsonToSend_1.insert("isCollFlag", isCollFlag);
//    QString str  = QString(QJsonDocument(jsonToSend_1).toJson());
//    server_->sendMessage(str);

    ui->collControlEnableBtn->setEnabled(true);
    ui->collControlDisableBtn->setEnabled(false);
}
void MainWindow::onSetCollThreshold() {
    collThreshold = ui->collThresholdLineEdit->text().toDouble();
}
void MainWindow::onCollSendTrueButton() {
    // 断开障碍预测数据发送
    disconnect(collPredTimer, SIGNAL(timeout()),nullptr,nullptr);
    collPredTimer->stop();
    ui->collControlEnableBtn->setEnabled(true);
    ui->collControlDisableBtn->setEnabled(false);

    // 发送true
	QJsonObject jsonToSend_0;
	jsonToSend_0.insert("mission", 5);
	jsonToSend_0.insert("isCollFlag", true);

	QString str = QString(QJsonDocument(jsonToSend_0).toJson());
	server_->sendMessage(str);
}
void MainWindow::onCollSendFalseButton() {
    // 断开障碍预测数据发送
    disconnect(collPredTimer, SIGNAL(timeout()),nullptr,nullptr);
    collPredTimer->stop();
    ui->collControlEnableBtn->setEnabled(true);
    ui->collControlDisableBtn->setEnabled(false);

    // 发送false
	QJsonObject jsonToSend_0;
	jsonToSend_0.insert("mission", 5);
	jsonToSend_0.insert("isCollFlag", false);
 
	QString str = QString(QJsonDocument(jsonToSend_0).toJson());
	server_->sendMessage(str);
}

// GPS坐标系相互转换
QVector<double> MainWindow::WGS84ToBD09(double lng, double lat) {
	//第一次转换
	double dlat = transformLat(lng - 105.0, lat - 35.0);
	double dlng = transformLng(lng - 105.0, lat - 35.0);
	double radlat = lat / 180.0 * M_PI;
	double magic = sin(radlat);
	magic = 1 - transformMagicNumberB * magic * magic;
	double sqrtmagic = sqrt(magic);
	dlat = (dlat * 180.0) / ((transformMagicNumberA * (1 - transformMagicNumberB)) / (magic * sqrtmagic) * M_PI);
	dlng = (dlng * 180.0) / (transformMagicNumberA / sqrtmagic * cos(radlat) * M_PI);
	double mglat = lat + dlat;
	double mglng = lng + dlng;

	//第二次转换
	double z = sqrt(mglng * mglng + mglat * mglat) + 0.00002 * sin(mglat * M_PI * 3000.0 / 180.0);
	double theta = atan2(mglat, mglng) + 0.000003 * cos(mglng * M_PI * 3000.0 / 180.0);
	double bd_lng = z * cos(theta) + 0.0065;
	double bd_lat = z * sin(theta) + 0.006;

	QVector<double> res(2);
	res[0] = bd_lng;
	res[1] = bd_lat;
	return res;
}
QVector<double> MainWindow::BD09ToWGS84(double lng, double lat) {
	// BD09转GCJ02
	double x = lng - 0.0065;
	double y = lat - 0.006;
	double z = sqrt(x * x + y * y) - 0.00002 * sin(y * M_PI * 3000.0 / 180.0);
	double theta = atan2(y, x) - 0.000003 * cos(x * M_PI * 3000.0 / 180.0);
	double gcj02_lng = z * cos(theta);
	double gcj02_lat = z * sin(theta);

	// GCJ02转WGS84
	double dlat = transformLat(gcj02_lng - 105.0, gcj02_lat - 35.0);
	double dlng = transformLng(gcj02_lng - 105.0, gcj02_lat - 35.0);
	double radlat = gcj02_lat / 180.0 * M_PI;
	double magic = sin(radlat);
	magic = 1 - transformMagicNumberB * magic * magic;
	double sqrtmagic = sqrt(magic);
	dlat = (dlat * 180.0) / ((transformMagicNumberA * (1 - transformMagicNumberB)) / (magic * sqrtmagic) * M_PI);
	dlng = (dlng * 180.0) / (transformMagicNumberA / sqrtmagic * cos(radlat) * M_PI);
	double mglat = gcj02_lat + dlat;
	double mglng = gcj02_lng + dlng;

	QVector<double> res(2);
	res[0] = mglng;
	res[1] = mglat;
	return res;
}
double MainWindow::transformLat(double lng, double lat) {
	double ret= -100.0 + 2.0 * lng + 3.0 * lat + 0.2 * lat * lat + 0.1 * lng * lat + 0.2 * sqrt(abs(lng));
	ret += (20.0 * sin(6.0 * lng * M_PI) + 20.0 * sin(2.0 * lng * M_PI)) * 2.0 / 3.0;
	ret += (20.0 * sin(lat * M_PI) + 40.0 * sin(lat / 3.0 * M_PI)) * 2.0 / 3.0;
	ret += (160.0 * sin(lat / 12.0 * M_PI) + 320 * sin(lat * M_PI / 30.0)) * 2.0 / 3.0;
	return ret;
}
double MainWindow::transformLng(double lng, double lat) {
	double ret = 300.0 + lng + 2.0 * lat + 0.1 * lng * lng + 0.1 * lng * lat + 0.1 * sqrt(abs(lng));
	ret += (20.0 * sin(6.0 * lng * M_PI) + 20.0 * sin(2.0 * lng * M_PI)) * 2.0 / 3.0;
	ret += (20.0 * sin(lng * M_PI) + 40.0 * sin(lng / 3.0 * M_PI)) * 2.0 / 3.0;
	ret += (150.0 * sin(lng / 12.0 * M_PI) + 300.0 * sin(lng / 30.0 * M_PI)) * 2.0 / 3.0;
	return ret;
}