#include "server.h"
#include "bridge.h"

namespace Ui {
	// Init
	Server::Server(QWidget* parent) {
		setWindowFlags(Qt::Window);
		setWindowTitle("Log");
		resize(320, 640);

		auto* grid = new QGridLayout(this);	// 网格布局
		grid->setContentsMargins(5, 10, 5, 5);	// 设置内容边距

		auto* upper_button_layout = new QHBoxLayout();	// 水平布局

		// 获取主机所有可能的ip地址
		ipAllList = QNetworkInterface::allAddresses();	// 返回主机上找到的所有的IP地址
		foreach (QHostAddress address, ipAllList)
			if(address.protocol() == QAbstractSocket::IPv4Protocol) // 使用IPv4地址
				qDebug() <<"发现IPv4地址 IP Address: "<< address.toString();

		auto* lower_button_layout = new QHBoxLayout();

		// listen按钮
		MyUXListenBtn = new QPushButton(tr("listen"), this);
		connect(MyUXListenBtn, &QPushButton::released, this, &Server::onMyUXListenBtn);
		lower_button_layout->addWidget(MyUXListenBtn);
		// clear按钮
		auto* MyUXClearBtn = new QPushButton(tr("Clear"), this);
		connect(MyUXClearBtn, &QPushButton::released, this, &Server::onMyUXClearBtn);

		lower_button_layout->addWidget(MyUXClearBtn);
		grid->addLayout(lower_button_layout, 1, 0, Qt::AlignLeft);
		grid->addLayout(upper_button_layout, 0, 0, Qt::AlignLeft);

		// MyUX信息显示区域初始化
		MyUXTextEdit = new QTextEdit(this);
		MyUXTextEdit->setObjectName(QStringLiteral("MyUXTextEdit"));
		MyUXTextEdit->setReadOnly(true);
		grid->addWidget(MyUXTextEdit, 2, 0, 1, 2);

		// MyUX TCP链接初始化
		tcpServerConnectionMyUXReceive = new QTcpSocket(this);
		tcpServerConnectionMyUXSend = new QTcpSocket(this);
		tcpSocketMyUXSendList = new QList<QTcpSocket*>;
		connect(&tcpServerMyUXReceive, &QTcpServer::newConnection, this, &Server::acceptConnectionMyUXReceive);
		connect(&tcpServerMyUXSend, &QTcpServer::newConnection, this, &Server::acceptConnectionMyUXSend);
	}

	// 向MyUX发送数据information
	void Server::sendMessage(const QString& information) {
//		qDebug() << "向MuUX发送数据";
		QByteArray message = information.toLocal8Bit();  // 将字符串的本地8位表示作为QByteArray返回
		for(auto i : *tcpSocketMyUXSendList)
			i->write(message);
	}

	// lsiten
	void Server::onMyUXListenBtn() {
		 if (!tcpServerMyUXReceive.isListening() && !tcpServerMyUXReceive.listen(QHostAddress::Any, 6666)) {
			 // QHostAddress::Any = 4 	双any-address栈，与该地址绑定的socket将侦听IPv4和IPv6接口。
			 // tcpServerMyUXReceive没有正在监听传入链接并且没有监听6666端口
			 qDebug() << "MyUX接收数据TCP端口未监听" << tcpServerMyUXReceive.errorString();
			 close();
			 return;
		 }
		 if(tcpServerMyUXReceive.isListening()) {
			 MyUXListenBtn->setEnabled(false);
			 qDebug() <<"============== 开始监听MyUX数据接收TCP端口 ==============";
		 }
		 if (!tcpServerMyUXSend.isListening() && !tcpServerMyUXSend.listen(QHostAddress::Any, 6665)) {
			 // QHostAddress::Any = 4 	双any-address栈，与该地址绑定的socket将侦听IPv4和IPv6接口。
			 // tcpServer_for_send没有正在监听传入链接并且没有监听6665端口
			 qDebug() << "MyUX发送数据TCP端口未监听" << tcpServerMyUXSend.errorString();
			 close();
			 return;
		 }
		 if(tcpServerMyUXSend.isListening())
			 qDebug() <<"============== 开始监听MyUX数据发送TCP端口，可以开始向MyUX发送数据 ==============";
	}
	void Server::onMyUXClearBtn() {
		MyUXTextEdit->clear();
	}
	void Server::acceptConnectionMyUXReceive() {
		qDebug() << "============== 接收MyUX数据TCP连接成功 ==============";
		tcpServerConnectionMyUXReceive = tcpServerMyUXReceive.nextPendingConnection();
		connect(tcpServerConnectionMyUXReceive, &QTcpSocket::readyRead, this, &Server::updateServerProgress);
	}
	void Server::updateServerProgress() {
//		qDebug() << "MyUX TCP socket 接收数据成功";
		// 时间
		QDateTime time = QDateTime::currentDateTime();
		QString str = time.toString("\n[ hh:mm:ss ]"); // 时间信息，设置显示格式

		// 读取数据
		qint64 len = tcpServerConnectionMyUXReceive->bytesAvailable();
		QByteArray alldata = tcpServerConnectionMyUXReceive->read(len);
		//开始转换编码
		QTextCodec *utf8codec = QTextCodec::codecForName("UTF-8");
		QString utf8str = utf8codec->toUnicode(alldata.mid(2));

		json = getJsonObjectFromString(utf8str);

		if(json.contains("GPS"))
			jsonGPS = json["GPS"].toObject();

		if(json.contains("Gimbal"))
			jsonGimbal = json["Gimbal"].toObject();

		if(json.contains("Battery0"))
			jsonBattery = json["Battery0"].toObject();

		//显示数据到状态栏上
		MyUXTextEdit->insertPlainText(str);		// 显示时间
		MyUXTextEdit->insertPlainText(utf8str);	// 显示信息
		MyUXTextEdit->insertPlainText(tr("\n"));

		//自动滚动到最底
		MyUXScrollBar = MyUXTextEdit->verticalScrollBar();
		if (MyUXScrollBar)
			MyUXScrollBar->setSliderPosition(MyUXScrollBar->maximum());
	}
	void Server::acceptConnectionMyUXSend() {
		qDebug() << "============== 发送MyUX数据TCP连接成功 ==============";
		tcpServerConnectionMyUXSend = tcpServerMyUXSend.nextPendingConnection();
		tcpSocketMyUXSendList->append(tcpServerConnectionMyUXSend);
		qDebug() <<"============== 发现MyUX数据发送目标 ==============";
	}

	QJsonObject Server::getJsonObjectFromString(const QString& jsonString) {
		QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toLocal8Bit().data());
		if( jsonDocument.isNull() )
			qDebug()<< "Server::getJsonObjectFromString 转换失败，请检查string "<< jsonString.toLocal8Bit().data();
		QJsonObject jsonObject = jsonDocument.object();

//		saveUAVStatus(SAVE_JSON_TEMP_PATH, SAVE_JSON_RENAME_PATH, jsonString);  // 保存QJson文件

		return jsonObject;
	}
	void Server::saveUAVStatus(const char* saveTempPath, const char* saveRenamePath, const QString& jsonString){
		// 删除已存在文件
		if(access(saveTempPath,F_OK))
			remove(saveTempPath);

		QByteArray byteArray = jsonString.toLocal8Bit();

		QFile file(saveTempPath);

		file.open(QIODevice::WriteOnly);
		file.write(byteArray);
		file.close();
		rename(saveTempPath, saveRenamePath);
	}
} //namespce Ui
