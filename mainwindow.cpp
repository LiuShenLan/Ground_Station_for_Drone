#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <cmath>

#include<opencv2/opencv.hpp>

#include <QCoreApplication>
#include <Python.h>

#include <sys/file.h>

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
	QMainWindow(parent),		// 执行父类QMainWindow的构造函数
	ui(new Ui::MainWindow) {	// 创建一个Ui::MainWindow类的对象
	ui->setupUi(this);	// 执行Ui::MainWindow类的setupUi()函数，实现窗口的生成与各种属性的设置、信号与槽的关联
	InitForm();
	InitVirtualStickControl();

	// 左侧listen栏
	server_ = new Ui::Server(this);
	dock_server_ = new QDockWidget("MyUX data", this);
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

	// 加载存储与显示图片文件
	imgSaveFile = open(READ_FRAME_PIC_PATH, O_WRONLY);
	if (CAM_SHOW_DETECT_CAMERA_FLAG)
		imgShowFile = open(CAM_SHOW_DETECT_CAMERA, O_RDONLY);

	timer = new QTimer(this);
	timer->start(50);	// 开始计时，超时则发出timeout()信号，读取摄像头信息
	connect(timer, SIGNAL(timeout()), this, SLOT(readFarme()));	// 时间到，读取当前摄像头信息
}
void MainWindow::InitForm() {
	// html地图相关初始化
	auto *channel = new QWebChannel(this);
	bridgeins = bridge::instance(); // 返回bridge对象
	channel->registerObject("bridge", (QObject*)bridgeins);	// 将bridgeins对象注册到channel
	ui->mapShow->page()->setWebChannel(channel);
	ui->mapShow->page()->load(QUrl::fromLocalFile(qApp->applicationDirPath() + "/bin/index.html"));

	// WayPoints初始化
	const QList<wayPoint>& wayPointsList = bridgeins->GetLightList();
	int nCount = wayPointsList.count();
	for(int i=0; i<nCount; i++) {
		wayPoint tLight = wayPointsList[i];
		ui->wayPointsComboBox->addItem(tLight.strDesc, tLight.strName);	// 在WayPoints下拉菜单中添加信息
	}
	ui->wayPointsComboBox->setCurrentIndex(0);
	connect(ui->wayPointsAddBtn, SIGNAL(clicked()), this, SLOT(onBtnAddLight()));
	connect(ui->wayPointsAddCurBtn, SIGNAL(clicked()), this, SLOT(onBtnAddCurLight()));
	connect(ui->wayPointsGoBtn, SIGNAL(clicked()), this, SLOT(onGoButton()));
	connect(ui->wayPointsClearBtn, SIGNAL(clicked()), this, SLOT(onClearAllPoint()));

	connect(ui->wayPointsTakeOffBtn, SIGNAL(clicked()), this, SLOT(onTakeoffButton()));
	connect(ui->wayPointsLandBtn, SIGNAL(clicked()), this, SLOT(onLandButton()));
	connect(ui->wayPointsSaveBtn, SIGNAL(clicked()), this, SLOT(onSaveButton()));
	connect(ui->wayPointsLoadBtn, SIGNAL(clicked()), this, SLOT(onLoadButton()));
	connect(ui->wayPointsContinueBtn, SIGNAL(clicked()), this, SLOT(onContinueBtn()));
	connect(ui->wayPointsBackBtn, SIGNAL(clicked()), this, SLOT(onBackBtn()));

	// GPS Data初始化
	ui->GPSLatitudeLineEdit->setMinimumWidth(18);
	ui->GPSLongitudeLineEdit->setMinimumWidth(18);
	connect(bridgeins, &bridge::toRecvMsg, this, &MainWindow::onRecvdMsg);
	connect(ui->GPSMapRefreshBtn, SIGNAL(clicked()), this, SLOT(onGPSMapRefreshBtn()));

	// 无人机方向控制
	connect(ui->manualDirectTurnLeftBtn,SIGNAL(clicked()), this, SLOT(onTurnLeftButton()));
	connect(ui->manualDirectTurnRightBtn,SIGNAL(clicked()), this, SLOT(onTurnRightButton()));
	connect(ui->manualDirectGoStraightBtn,SIGNAL(clicked()), this, SLOT(onGoStraightButton()));
}
void MainWindow::InitVirtualStickControl() {
	// 无人机虚拟控制
	tcpSocketDronet = new QTcpSocket(this);
	connect(ui->virtualStickEnableBtn, SIGNAL(clicked()), this, SLOT(onEnableVirtualStickButton()));
	connect(ui->virtualStickDisableBtn, SIGNAL(clicked()), this, SLOT(onDisableVirtualStickButton()));
	connect(ui->virtualStickRollSetBtn, SIGNAL(clicked()), this, SLOT(onSetRoll()));
	connect(ui->virtualStickResetBtn, SIGNAL(clicked()), this, SLOT(onVirtualStickResetButton()));
	virtualStickTimer = new QTimer(this);	// 实例化定时器
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
	if(!tcpServerDronet.isListening() && !tcpServerDronet.listen(QHostAddress::LocalHost, 5555)) {
		qDebug() << "dronet接收数据TCP端口未监听" << tcpServerDronet.errorString();
		close();
		return;
	}
	if(tcpServerDronet.isListening())
		qDebug()<<"============== 开始监听dronet数据接收TCP端口 ==============";
	connect(&tcpServerDronet, &QTcpServer::newConnection, this, &MainWindow::acceptConnectionDrenet);

	// TCP coll pred
	connect(ui->collControlEnableBtn, SIGNAL(clicked()), this, SLOT(onEnableCollButton()));
	connect(ui->collControlDisableBtn, SIGNAL(clicked()), this, SLOT(onDisableCollButton()));
	connect(ui->collThresholdSetBtn, SIGNAL(clicked()), this, SLOT(onSetCollThreshold()));
	connect(ui->collSendTrueBtn, SIGNAL(clicked()), this, SLOT(onCollSendTrueButton()));
	connect(ui->collSendFalseBtn, SIGNAL(clicked()), this, SLOT(onCollSendFalseButton()));
	connect(ui->collHoverCheckBox, SIGNAL(stateChanged(int)), this, SLOT(collHoverCheckBoxStateChange(int)));
	ui->collAvoidanceRadioBtn->setChecked(true);
	collPredTimer = new QTimer(this);   // 实例化定时器
	ui->collControlDisableBtn->setEnabled(false);

	if(!tcpServerCollPred.isListening() && !tcpServerCollPred.listen(QHostAddress::LocalHost, 5556)) {
		qDebug() << "coll pred接收数据TCP端口未监听" << tcpServerCollPred.errorString();
		close();
		return;
	}
	if(tcpServerCollPred.isListening())
		qDebug()<<"============== 开始监听coll pred数据接收TCP端口 ==============";
	connect(&tcpServerCollPred, &QTcpServer::newConnection, this, &MainWindow::acceptConnectionCollPred);
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
void MainWindow::onGPSMapRefreshBtn() {
	timer_1 = new QTimer(this);
	timer_1->start(100);

	timer_2 = new QTimer(this);
	timer_2->start(200);

	bridgeins->setNavPointRotate(int(ui->navigationPointDirectSlider->value()));

	/**
	 * shinei室内测试注释掉下一行代码 TODO:
	 * **/
	connect(timer_1,SIGNAL(timeout()),this,SLOT(timeCountsFunction()));
	connect(timer_2,SIGNAL(timeout()),this,SLOT(callJava()));
}
void MainWindow::timeCountsFunction() {
	// 设置GPS经纬度信息
	ui->GPSLongitudeLineEdit->setText(QString::number(server_->jsonGPS["longitude"].toDouble(), 10, 14));
	ui->GPSLatitudeLineEdit->setText(QString::number(server_->jsonGPS["latitude"].toDouble(), 10, 14));
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

//	QString strJs = strJs_
//			.arg(ui->GPSLongitudeLineEdit->text().toDouble()*0.01+109.03525)
//			.arg(ui->GPSLatitudeLineEdit->text().toDouble()*0.01+26.6564);

	QVector<double> gps = WGS84ToBD09(server_->jsonGPS["longitude"].toDouble(),
                                      server_->jsonGPS["latitude"].toDouble());
	QString strJs = strJs_.arg(gps[0],0,'f',10).arg(gps[1],0,'f',10);
	ui->mapShow->page()->runJavaScript(strJs);
}

// WayPoints
void MainWindow::onBtnAddLight() {
	// 根据根据临时WayPoint经纬度信息生成wayPoint对象
	wayPoint tLight = bridgeins->AddLight(int(ui->navigationPointDirectSlider->value()),
										  ui->wayPointsHeightLineEdit->text().toDouble(),
										  ui->wayPointsTakePhotocheckBox->isChecked(),
										  ui->wayPointsTurnLeftcheckBox->isChecked());
	// 向WayPoints下拉菜单中写入信息
	ui->wayPointsComboBox->addItem(tLight.strDesc, tLight.strName);
	ui->wayPointsComboBox->setCurrentIndex(ui->wayPointsComboBox->count()-1);
	bridgeins->onUpdateData();
}
void MainWindow::onBtnAddCurLight() {
	// 读取并添加无人机当前位置
	double lng = server_->jsonGPS["longitude"].toDouble();
	double lat = server_->jsonGPS["latitude"].toDouble();
	int rotation = static_cast<int>(server_->jsonGPS["yaw"].toDouble());
	rotation = rotation < 0 ? rotation + 360 : rotation;
	bridgeins->newPoint(lng, lat);

	// 根据根据临时WayPoint经纬度信息生成wayPoint对象
	wayPoint tLight = bridgeins->AddLight((int)rotation,
										  ui->wayPointsHeightLineEdit->text().toDouble(),
										  ui->wayPointsTakePhotocheckBox->isChecked(),
										  ui->wayPointsTurnLeftcheckBox->isChecked());
	// 向WayPoints下拉菜单中写入信息
	ui->wayPointsComboBox->addItem(tLight.strDesc, tLight.strName);
	ui->wayPointsComboBox->setCurrentIndex(ui->wayPointsComboBox->count()-1);
	bridgeins->onUpdateData();
}
void MainWindow::onGoButton() {
	sendWayPoint();
}
void MainWindow::sendWayPoint() {
	QList<wayPoint> wayPointList = bridgeins->returnWayPointList();  // 所有WayPoints的list	WGS84坐标系
	QJsonObject jsonToSend;
	jsonToSend.insert("mission", 1);
	jsonToSend.insert("way_point_num", wayPointList.size());

	// 根据wayPoints朝向计算并发送无人机朝向
	int n = wayPointList.size();
	for (int i = 0; i < n; ++i) {
		jsonToSend.insert(QString::number(i)+"Lng", wayPointList.at(i).fLng);
		jsonToSend.insert(QString::number(i)+"Lat", wayPointList.at(i).fLat);
		jsonToSend.insert(QString::number(i)+"altitude", wayPointList.at(i).altitude);
		jsonToSend.insert(QString::number(i)+"takePhoto", wayPointList.at(i).takePhoto);
		jsonToSend.insert(QString::number(i)+"turnLeft", wayPointList.at(i).turnLeft);
		int head = wayPointList.at(i).rotation;
		head = head > 180 ? head - 360 : head;
		jsonToSend.insert(QString::number(i)+"head", head);
	}

	QString str = QString(QJsonDocument(jsonToSend).toJson());
//	qDebug() << str;
	server_->sendMessage(str);
}
void MainWindow::onRecvdMsg(const QString& msg) {
	// 将msg由BD09坐标系转换为WSG84坐标系
	QStringList lst = msg.split(',');
//	fLng = lst[0].toDouble();	经度
//	fLat = lst[1].toDouble();	维度
	QVector<double> gps = BD09ToWGS84(lst[0].toDouble(), lst[1].toDouble());
	QString log = "接收到html地图数据(BD09)：%1 | 转换到WSG84: %2, %3";
	QString logShow = log.arg(msg)
						.arg(gps[0], 0, 'f', 10)
						.arg(gps[1], 0, 'f', 10);
	qDebug() << logShow;
	bridgeins->newPoint(gps[0],gps[1]);
}
void MainWindow::onClearAllPoint() {
	if(ui->wayPointsComboBox->count() >= 1) {
		ui->wayPointsComboBox->clear();
		bridgeins->removeAllPoints();
	}
}
void MainWindow::onReleaseNavSlider() {
	bridgeins->setNavPointRotate(int(ui->navigationPointDirectSlider->value()));
}
void MainWindow::onSaveButton() {
	QJsonArray jsonArray;
	QList<wayPoint> wayPointList = bridgeins->returnWayPointList();  // 获取所有的WayPoints信息
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
			wayPoint wayPoints = jsonToWayPoints(jsonWayPoint);
			bridgeins->wayPointsAllList.append(wayPoints);
			bridgeins->wayPointsNum++;
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
QJsonObject MainWindow::wayPointsToJson(const wayPoint& point) {
	QJsonObject jsonWayPoints;
	jsonWayPoints.insert("strName", point.strName);
	jsonWayPoints.insert("strDesc", point.strDesc);
	jsonWayPoints.insert("fLng", point.fLng);
	jsonWayPoints.insert("fLat", point.fLat);
	jsonWayPoints.insert("altitude", point.altitude);
	jsonWayPoints.insert("nValue", point.nValue);
	jsonWayPoints.insert("rotation", point.rotation);
	jsonWayPoints.insert("takePhoto", point.takePhoto);
	jsonWayPoints.insert("turnLeft", point.turnLeft);
	return jsonWayPoints;
}
wayPoint MainWindow::jsonToWayPoints(const QJsonObject& jsonWayPoints) {
	wayPoint tLight;
	tLight.strName = jsonWayPoints.value("strName").toString();
	tLight.strDesc = jsonWayPoints.value("strDesc").toString();
	tLight.fLng = jsonWayPoints.value("fLng").toDouble();
	tLight.fLat = jsonWayPoints.value("fLat").toDouble();
	tLight.altitude = jsonWayPoints.value("altitude").toDouble();
	tLight.nValue = jsonWayPoints.value("nValue").toInt();
	tLight.rotation = jsonWayPoints.value("rotation").toInt();
	tLight.takePhoto = jsonWayPoints.value("takePhoto").toBool();
	tLight.turnLeft = jsonWayPoints.value("turnLeft").toBool();
	return tLight;
}
void MainWindow::onContinueBtn() {
	QList<wayPoint> wayPointList = bridgeins->returnWayPointList();  // 所有WayPoints的list	WGS84坐标系
	int wayPointsNextIndex = server_->jsonGimbal["wayPointsNextIndex"].toInt();


	QJsonObject jsonToSend;
	jsonToSend.insert("mission", 1);
	jsonToSend.insert("way_point_num", wayPointList.size() - wayPointsNextIndex);

	// 根据wayPoints朝向计算并发送无人机朝向
	int n = wayPointList.size();
	for (int i = wayPointsNextIndex; i < n; ++i) {
		jsonToSend.insert(QString::number(i - wayPointsNextIndex)+"Lng", wayPointList.at(i).fLng);
		jsonToSend.insert(QString::number(i - wayPointsNextIndex)+"Lat", wayPointList.at(i).fLat);
		jsonToSend.insert(QString::number(i - wayPointsNextIndex)+"altitude", wayPointList.at(i).altitude);
		jsonToSend.insert(QString::number(i - wayPointsNextIndex)+"takePhoto", wayPointList.at(i).takePhoto);
		jsonToSend.insert(QString::number(i - wayPointsNextIndex)+"turnLeft", wayPointList.at(i).turnLeft);
		int head = wayPointList.at(i).rotation;
		head = head > 180 ? head - 360 : head;
		jsonToSend.insert(QString::number(i - wayPointsNextIndex)+"head", head);
	}

	QString str = QString(QJsonDocument(jsonToSend).toJson());
	qDebug() << str;
	server_->sendMessage(str);
}
void MainWindow::onBackBtn() {
	QList<wayPoint> wayPointList = bridgeins->returnWayPointList();  // 所有WayPoints的list	WGS84坐标系
	int wayPointsNextIndex = server_->jsonGimbal["wayPointsNextIndex"].toInt();

	QJsonObject jsonToSend;
	jsonToSend.insert("mission", 1);
	jsonToSend.insert("way_point_num", wayPointsNextIndex);

	// 根据wayPoints朝向计算并发送无人机朝向
	int n = wayPointList.size();
	for (int i = wayPointList.size() - 1; i >= 0; --i) {
		jsonToSend.insert(QString::number(wayPointList.size() - 1 - i)+"Lng", wayPointList.at(i).fLng);
		jsonToSend.insert(QString::number(wayPointList.size() - 1 - i)+"Lat", wayPointList.at(i).fLat);
		jsonToSend.insert(QString::number(wayPointList.size() - 1 - i)+"altitude", wayPointList.at(i).altitude);
		jsonToSend.insert(QString::number(wayPointList.size() - 1 - i)+"takePhoto", false);
		jsonToSend.insert(QString::number(wayPointList.size() - 1 - i)+"turnLeft", !wayPointList.at(i).turnLeft);
		int head = wayPointList.at(i).rotation;
		head = head > 180 ? head - 360 : head;	// 将存储wayPoints朝向转换到发送格式
		head = head >= 0 ? head - 180 : head + 180;	// 朝向取反
		jsonToSend.insert(QString::number(wayPointList.size() - 1 - i)+"head", head);
	}

	QString str = QString(QJsonDocument(jsonToSend).toJson());
	qDebug() << str;
	server_->sendMessage(str);
}


// 无人机起降控制
void MainWindow::onTakeoffButton() {
	QJsonObject jsonToSend;
	jsonToSend.insert("mission", 0);
	QString str = QString(QJsonDocument(jsonToSend).toJson());
	server_->sendMessage(str);
}
void MainWindow::onLandButton() {
	QJsonObject jsonToSend;
	jsonToSend.insert("mission", 4);
	QString str = QString(QJsonDocument(jsonToSend).toJson());
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
	jsonToSend_0.insert("mission", 2);
	jsonToSend_0.insert("yaw", double(ui->virtualStickYawSlider->value())/1000);
	jsonToSend_0.insert("roll", double(ui->virtualStickRollSlider->value())/1000);
	jsonToSend_0.insert("pitch", double(ui->virtualStickPitchSlider->value())/1000);
	jsonToSend_0.insert("throttle", double(ui->virtualStickThrottleSlider->value())/1000);

	QString str = QString(QJsonDocument(jsonToSend_0).toJson());
	server_->sendMessage(str);
}
void MainWindow::onDisableVirtualStickButton() {
	disconnect(virtualStickTimer, SIGNAL(timeout()),nullptr,nullptr);
	virtualStickTimer->stop();
	ui->virtualStickYawSlider->setValue(0);
	ui->virtualStickPitchSlider->setValue(0);
	ui->virtualStickRollSlider->setValue(0);

//	QJsonObject jsonToSend_0;
//	jsonToSend_0.insert("mission", 2);
//	jsonToSend_0.insert("yaw", ui->virtualStickYawSlider->value()/1000);
//	jsonToSend_0.insert("roll", ui->virtualStickRollSlider->value()/1000);
//	jsonToSend_0.insert("pitch", ui->virtualStickPitchSlider->value()/1000);
//	jsonToSend_0.insert("throttle", double(ui->virtualStickThrottleSlider->value())/1000);

//	QString str = QString(QJsonDocument(jsonToSend_0).toJson());
//	server_->sendMessage(str);

	QJsonObject jsonToSend_1;
	jsonToSend_1.insert("mission", 3);
	jsonToSend_1.insert("yaw", ui->virtualStickYawSlider->value()/1000);
	jsonToSend_1.insert("roll", ui->virtualStickRollSlider->value()/1000);
	jsonToSend_1.insert("pitch", ui->virtualStickPitchSlider->value()/1000);
	jsonToSend_1.insert("throttle", double(ui->virtualStickThrottleSlider->value())/1000);

	QString str  = QString(QJsonDocument(jsonToSend_1).toJson());
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
		case Qt::Key_W :	// W 增加油门
			throttleBias++;
			ui->virtualStickThrottleSlider->setValue(throttleBias);
			ui->throttleBiasLabel->setText(QString::number(throttleBias));
			break;
		case Qt::Key_S :	// S 减少油门
			throttleBias--;
			ui->virtualStickThrottleSlider->setValue(throttleBias);
			ui->throttleBiasLabel->setText(QString::number(throttleBias));
			break;
		case Qt::Key_A :	// A 向左偏航
			yawBias--;
			ui->yawBiasLabel->setText(QString::number(yawBias));
			break;
		case Qt::Key_D :	// D 向右偏航
			yawBias++;
			ui->yawBiasLabel->setText(QString::number(yawBias));
			break;
		case Qt::Key_I :	// I 增加横滚
			rollBias++;
			ui->rollBiasLabel->setText(QString::number(rollBias));
			break;
		case Qt::Key_K :	// K 减少横滚
			rollBias--;
			ui->rollBiasLabel->setText(QString::number(rollBias));
			break;
		case Qt::Key_J :	// J 减少俯仰
			pitchBias--;
			ui->pitchBiasLabel->setText(QString::number(pitchBias));
			break;
		case Qt::Key_L :	// L 增加俯仰
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
//	cv::rectangle(frame,cvPoint(20,200),cvPoint(200,300),cvScalar(255,0,0),1,1,0);
	//将抓取到的帧，转换为QImage格式。QImage::Format_RGB888不同的摄像头用不同的格式。
	QImage image = QImage(frame.data, frame.cols, frame.rows, static_cast<int>(frame.step), QImage::Format_RGB888).rgbSwapped().scaled(640,480,Qt::KeepAspectRatioByExpanding);

	// 写文件，加锁
	flock(imgSaveFile, LOCK_EX);
	cv::imwrite(READ_FRAME_PIC_PATH,frame);
	flock(imgSaveFile, LOCK_UN);

//	cv::imwrite(READ_FRAME_PIC_TEMP_PATH,frame);
//	if(access(READ_FRAME_PIC_PATH,F_OK))
//		remove(READ_FRAME_PIC_PATH);
//	rename(READ_FRAME_PIC_TEMP_PATH, READ_FRAME_PIC_PATH);

	// 显示detect后的图片
	if (CAM_SHOW_DETECT_CAMERA_FLAG) {
		// 读文件，加锁
		flock(imgShowFile, LOCK_EX);
		frame = cv::imread(CAM_SHOW_DETECT_CAMERA, 1);	//读入一张本地图片
		flock(imgShowFile, LOCK_UN);

//		frame = cv::imread(CAM_SHOW_DETECT_CAMERA, 1);	//读入一张本地图片

//		cv::cvtColor(frame,frame,cv::COLOR_BGR2RGB);//opencv读取图片按照BGR方式读取，为了正常显示，所以将BGR转为RGB
		image = QImage(frame.data, frame.cols, frame.rows, static_cast<int>(frame.step), QImage::Format_RGB888).rgbSwapped().scaled(640,320,Qt::KeepAspectRatioByExpanding);
	}
	ui->camShow->setPixmap(QPixmap::fromImage(image));  // 显示图片

//	ui->deepImg->setPixmap(QPixmap("/home/wwh/2.jpg").scaled(320,160));
}
/*******************************
***关闭摄像头，释放资源，必须释放***
********************************/
void MainWindow::closeCamara() {
	timer->stop();	// 停止读取数据。
	cam.release();	//释放内存；
}

// TCP dronet
void MainWindow::acceptConnectionDrenet() {
	qDebug()<<"============== 接收dronet数据TCP连接成功 ==============";
	tcpSocketDronet = tcpServerDronet.nextPendingConnection();
	connect(bridgeins, &bridge::targetPointReceived, this, &MainWindow::onRecvTargetPoint);
	connect(tcpSocketDronet, &QTcpSocket::readyRead, this, &MainWindow::updateCommand_from_python_controller);
}
void MainWindow::onRecvTargetPoint(const QString& msg) {
	return;	// TODO
	QStringList lst;
	lst = msg.split(',');
	double next_point_direction = 0;
	QString targetNum = lst[0]; // WayPoints列表中的WayPoint的索引
	int targetRotation = lst[1].toInt();	// WayPoint旋转角度
//	bridgeins->onLightOn(targetNum);
	double lngBD09 = lst[2].toDouble();   // WayPoint经度，BD09坐标系
	double latBD09 = lst[3].toDouble();   // WayPoint纬度，BD09坐标系
    QVector<double> temp = BD09ToWGS84(lngBD09, latBD09);    // WSG84坐标系

	double targetLng = temp[0];   // WayPoint经度，WSG84坐标系
	double targetLat = temp[1];   // WayPoint纬度，WSG84坐标系
	int arrayListLength = lst[4].toInt();   // WayPoints数目
	//qDebug()<<fixed<<qSetRealNumberPrecision(7)<<targetLng<<" "<<targetLat;
	//qDebug()<<(ui->GPSLongitudeLineEdit->text().toDouble()+0.0126)<<" "<<(ui->GPSLatitudeLineEdit->text().toDouble()+0.0062);
	//qDebug()<<((targetLng - (ui->GPSLongitudeLineEdit->text().toDouble()+0.0125))/(targetLat - (ui->GPSLatitudeLineEdit->text().toDouble()+0.0062);
	// 计算方位角
    QVector<double> gps = WGS84ToBD09(server_->jsonGPS["longitude"].toDouble(), server_->jsonGPS["latitude"].toDouble());
	// first quadrant
	if((targetLng - gps[0])>0 && (targetLat - gps[1])>0)
		next_point_direction = atan2((targetLng - gps[0]), (targetLat - gps[1]))*180/3.14159;
	// second quadrant
	if((targetLng - gps[0])<0 && (targetLat - ui->GPSLatitudeLineEdit->text().toDouble())>0)
		next_point_direction = 360 + atan2((targetLng - gps[0]), (targetLat - gps[1]))*180/3.14159;
	// third quadrant
	if((targetLng - gps[0])<0 && (targetLat - ui->GPSLatitudeLineEdit->text().toDouble())<0)
		next_point_direction = 360 + atan2((targetLng - gps[0]), (targetLat - gps[1]))*180/3.14159;
	// forth quadrant
	if((targetLng - gps[0])>0 && (targetLat - ui->GPSLatitudeLineEdit->text().toDouble())<0)
		next_point_direction = atan2((targetLng - gps[0]), (targetLat - gps[1]))*180/3.14159;

//	next_point_direction = (double)targetRotation;
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
		qint64 len = tcpSocketDronet->bytesAvailable();
		QByteArray alldate = tcpSocketDronet->read(len);
		QTextCodec *utf8codec = QTextCodec::codecForName("UTF-8");
		QString utf8str = utf8codec->toUnicode(alldate.mid(2));
		//qDebug()<<"pthon_controller:"<<utf8str<<"\n";
		QStringList control_command = utf8str.split(",");

		// TODO
		double yawSliderReceive, pitchSliderReceive, rollSliderReceive;
		if (!bridgeins->wayPointsAllList.empty()) {	// 有wayPoints
			// 无人机当前位置，gps[0] = lng, gps[1] = lat
			QVector<double> gps = WGS84ToBD09(server_->jsonGPS["longitude"].toDouble(), server_->jsonGPS["latitude"].toDouble());
			double minDis = 100;
			int minDisIndex = 0;
			for (int i = 0; i < bridgeins->wayPointsAllList.size(); ++i) {
				double lng = gps[0] - bridgeins->wayPointsAllList[i].fLng;
				double lat = gps[1] - bridgeins->wayPointsAllList[i].fLat;
				double dis = pow(lng, 2) + pow(lat, 2);
				if (dis < minDis) {
					minDis = dis;
					minDisIndex = i;
				}
			}
			if (minDis < 0.0002) {	// 靠近wayPoints，使用wayPoints的方向
				double wayPointsRotation = bridgeins->wayPointsAllList[minDisIndex].rotation;	// wayPoints朝向
				double uavRotation = server_->jsonGPS["yaw"].toDouble();
				bool turnLeft;
				if (uavRotation >= 0) {	// 无人机朝向东北~东南
					double uavBackRotation = uavRotation - 180;
					if (wayPointsRotation >= uavBackRotation && wayPointsRotation <= uavRotation)
						turnLeft = true;
					else
						turnLeft = false;
				} else {	// 无人机朝向西北~西南
					double uavBackRotation = uavRotation + 180;
					if (wayPointsRotation >= uavRotation && wayPointsRotation <= uavBackRotation)
						turnLeft = false;
					else
						turnLeft = true;
				}

				// 设置虚拟控制滑块数值
				if (turnLeft)
					rollSliderReceive = -10;
				else
					rollSliderReceive = 10;
			} else	// 远离WayPoints，不使用wayPoints的方向，设置虚拟控制滑块数值
				rollSliderReceive = control_command.at(2).toDouble();
		} else
			rollSliderReceive = control_command.at(2).toDouble();

		yawSliderReceive = control_command.at(0).toDouble();
		pitchSliderReceive = control_command.at(1).toDouble();

//		// 设置虚拟控制滑块数值
//		double yawSliderReceive = control_command.at(0).toDouble();
//		double pitchSliderReceive = control_command.at(1).toDouble();
//		double rollSliderReceive = control_command.at(2).toDouble();

		if (ui->virtualStickSafeModeCheckBox->isChecked()) {	// 安全模式，限制阈值
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
	} else {	// 禁止uav_dronet控制
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
//	QString sWriteData = QString::number(direction);
//	tcpSocketDronet->write(sWriteData.toUtf8());
}

// TCP coll pred
void MainWindow::acceptConnectionCollPred() {
	qDebug()<<"============== 接收障碍预测数据TCP连接成功 ==============";
	tcpSocketCollPred = tcpServerCollPred.nextPendingConnection();
	connect(tcpSocketCollPred, &QTcpSocket::readyRead, this, &MainWindow::update_coll_pred);
}
void MainWindow::update_coll_pred() {
	// 读取数据
	QTextCodec *utf8codec = QTextCodec::codecForName("UTF-8");

	qint64 len = tcpSocketCollPred->bytesAvailable();
	QByteArray alldate = tcpSocketCollPred->read(coll_pre_receive_length);
	if (len > coll_pre_receive_length)
		tcpSocketCollPred->read(len - coll_pre_receive_length);
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
	collIsSending = true;
	ui->collControlEnableBtn->setEnabled(false);
	ui->collControlDisableBtn->setEnabled(true);
	collPredTimer->start(100);
	connect(collPredTimer, SIGNAL(timeout()), this, SLOT(sendCollPredCommand()));
}
void MainWindow::sendCollPredCommand() {
	QJsonObject jsonToSend_0;
	if (ui->collHoverCheckBox->isChecked()) {	// 悬停
		jsonToSend_0.insert("mission", 6);
		jsonToSend_0.insert("isCollFlag", true);
	} else {
		if (ui->collStopRadioBtn->isChecked())
			jsonToSend_0.insert("mission", 6);
		else
			jsonToSend_0.insert("mission", 5);
		jsonToSend_0.insert("isCollFlag", isCollFlag);
	}

	QString str = QString(QJsonDocument(jsonToSend_0).toJson());
	server_->sendMessage(str);
//	qDebug() << str;
}
void MainWindow::onDisableCollButton() {
	collIsSending = false;
	disconnect(collPredTimer, SIGNAL(timeout()),nullptr,nullptr);
	collPredTimer->stop();

	// 发送mission 6
//	QJsonObject jsonToSend_1;
//	jsonToSend_1.insert("mission", 6);
//	jsonToSend_1.insert("isCollFlag", isCollFlag);
//	QString str  = QString(QJsonDocument(jsonToSend_1).toJson());
//	server_->sendMessage(str);

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

void MainWindow::collHoverCheckBoxStateChange(int state) {
	if (state == Qt::Checked && !collIsSending)
		onEnableCollButton();
}

// GPS坐标系相互转换
QVector<double> MainWindow::WGS84ToBD09(double lng, double lat) {
	/**
	//第一次转换
	double dlat = transformLat(lng - 105.0, lat - 35.0);
	double dlng = transformLng(lng - 105.0, lat - 35.0);
	double radlat = lat / 180.0 * transformMagicNumberPI;
	double magic = sin(radlat);
	magic = 1 - transformMagicNumberEE * magic * magic;
	double sqrtmagic = sqrt(magic);
	dlat = (dlat * 180.0) / ((transformMagicNumberA * (1 - transformMagicNumberEE)) / (magic * sqrtmagic) * transformMagicNumberPI);
	dlng = (dlng * 180.0) / (transformMagicNumberA / sqrtmagic * cos(radlat) * transformMagicNumberPI);
	double mglat = lat + dlat;
	double mglng = lng + dlng;

	//第二次转换
	double z = sqrt(mglng * mglng + mglat * mglat) + 0.00002 * sin(mglat * transformMagicNumberX_PI);
	double theta = atan2(mglat, mglng) + 0.000003 * cos(mglng * transformMagicNumberX_PI);
	double bd_lng = z * cos(theta) + 0.0065;
	double bd_lat = z * sin(theta) + 0.006;
	 **/
	QVector<double> res(2);
	res = WGS84ToGCJ02(lng, lat);
	return GCJ02ToBD09(res[0], res[1]);
}
QVector<double> MainWindow::BD09ToWGS84(double lng, double lat) {
	QVector<double> res(2);
	res = BD09ToGCJ02(lng, lat);
	return GCJ02ToWGS84(res[0], res[1]);
}
QVector<double> MainWindow::BD09ToGCJ02(double lng, double lat) {
	double x = lng - 0.0065;
	double y = lat - 0.006;
	double z = sqrt(x * x + y * y) - 0.00002 * sin(y * transformMagicNumberX_PI);
	double theta = atan2(y, x) - 0.000003 * cos(x * transformMagicNumberX_PI);
	double gg_lng = z * cos(theta);
	double gg_lat = z * sin(theta);

	return {gg_lng, gg_lat};
}
QVector<double> MainWindow::GCJ02ToBD09(double lng, double lat) {
	double z = sqrt(lng * lng + lat * lat) + 0.00002 * sin(lat * transformMagicNumberX_PI);
	double theta = atan2(lat, lng) + 0.000003 * cos(lng * transformMagicNumberX_PI);
	double bd_lng = z * cos(theta) + 0.0065;
	double bd_lat = z * sin(theta) + 0.006;
	return {bd_lng, bd_lat};
}
QVector<double> MainWindow::WGS84ToGCJ02(double lng, double lat) {
	double dlat = transformLat(lng - 105.0, lat - 35.0);
	double dlng = transformLng(lng - 105.0, lat - 35.0);
	double radlat = lat / 180.0 * transformMagicNumberPI;
	double magic = sin(radlat);
	magic = 1 - transformMagicNumberEE * magic * magic;
	double sqrtmagic = sqrt(magic);
	dlat = (dlat * 180.0) / ((transformMagicNumberA * (1 - transformMagicNumberEE)) / (magic * sqrtmagic) * transformMagicNumberPI);
	dlng = (dlng * 180.0) / (transformMagicNumberA / sqrtmagic * cos(radlat) * transformMagicNumberPI);
	double mglat = lat + dlat;
	double mglng = lng + dlng;
	return {mglng, mglat};
}
QVector<double> MainWindow::GCJ02ToWGS84(double lng, double lat) {
	double dlat = transformLat(lng - 105.0, lat - 35.0);
	double dlng = transformLng(lng - 105.0, lat - 35.0);
	double radlat = lat / 180.0 * transformMagicNumberPI;
	double magic = sin(radlat);
	magic = 1 - transformMagicNumberEE * magic * magic;
	double sqrtmagic = sqrt(magic);
	dlat = (dlat * 180.0) / ((transformMagicNumberA * (1 - transformMagicNumberEE)) / (magic * sqrtmagic) * transformMagicNumberPI);
	dlng = (dlng * 180.0) / (transformMagicNumberA / sqrtmagic * cos(radlat) * transformMagicNumberPI);
	double mglat = lat + dlat;
	double mglng = lng + dlng;

	return {lng * 2 - mglng, lat * 2 - mglat};
}
double MainWindow::transformLat(double lng, double lat) {
	double ret= -100.0 + 2.0 * lng + 3.0 * lat + 0.2 * lat * lat + 0.1 * lng * lat + 0.2 * sqrt(abs(lng));
	ret += (20.0 * sin(6.0 * lng * transformMagicNumberPI) + 20.0 * sin(2.0 * lng * transformMagicNumberPI)) * 2.0 / 3.0;
	ret += (20.0 * sin(lat * transformMagicNumberPI) + 40.0 * sin(lat / 3.0 * transformMagicNumberPI)) * 2.0 / 3.0;
	ret += (160.0 * sin(lat / 12.0 * transformMagicNumberPI) + 320 * sin(lat * transformMagicNumberPI / 30.0)) * 2.0 / 3.0;
	return ret;
}
double MainWindow::transformLng(double lng, double lat) {
	double ret = 300.0 + lng + 2.0 * lat + 0.1 * lng * lng + 0.1 * lng * lat + 0.1 * sqrt(abs(lng));
	ret += (20.0 * sin(6.0 * lng * transformMagicNumberPI) + 20.0 * sin(2.0 * lng * transformMagicNumberPI)) * 2.0 / 3.0;
	ret += (20.0 * sin(lng * transformMagicNumberPI) + 40.0 * sin(lng / 3.0 * transformMagicNumberPI)) * 2.0 / 3.0;
	ret += (150.0 * sin(lng / 12.0 * transformMagicNumberPI) + 300.0 * sin(lng / 30.0 * transformMagicNumberPI)) * 2.0 / 3.0;
	return ret;
}
