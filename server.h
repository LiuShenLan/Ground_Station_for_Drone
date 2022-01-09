#ifndef SERVER_H
#define SERVER_H


#include <QAbstractSocket>
#include <QTcpServer>
#include <QScrollBar>
#include <QMessageBox>

#include <QJsonDocument>
#include <QJsonObject>

#include <QtWidgets>
#include <QtNetwork>
#include <QList>

#include "mainwindow.h"

class QTcpSocket;

namespace Ui{
	class MainWindow;

	class Server : public QWidget
	{
	public:
		QJsonObject json;		// 总信息
		QJsonObject jsonGPS;	// 无人机GPS信息
		QJsonObject jsonGimbal;	// 无人机状态信息
		QJsonObject jsonBattery;	// 无人机电池信息

		explicit Server(QWidget* parent);	// 窗口左侧(Info)监听栏初始化

		void sendMessage(const QString& information);	// 向MyUX发送数据information

	private:
		// listen Init
		QPushButton* MyUXListenBtn;	// listen按钮
		QTextEdit *MyUXTextEdit;	// MyUX数据显示
		QScrollBar *MyUXScrollBar;	// MyUX数据显示滚动条

		// QT上位机与MyUX APP之间通讯所用的TCP信息
		QTcpServer tcpServerMyUXReceive, tcpServerMyUXSend;	// TCP server
		QTcpSocket *tcpServerConnectionMyUXReceive, *tcpServerConnectionMyUXSend;	// TCP socket
		QList<QTcpSocket*> *tcpSocketMyUXSendList;	// TCP socket list
		QList<QHostAddress> ipAllList;	// 主机上找到的所有IP地址

		// listen 函数
		void onMyUXListenBtn();	// 开始监听MyUX数据
		void onMyUXClearBtn();	// 清除MyUX数据显示
		void acceptConnectionMyUXReceive();	// MyUX TCP socket 接收信息初始化
		void updateServerProgress();		// 接收 MyUX TCP socket信息并显示到listen窗口
		void acceptConnectionMyUXSend();	// TCP socket 发送信息初始化

		static QJsonObject getJsonObjectFromString(const QString& jsonString);	// 将string转换为Json文件并保存
		static void saveUAVStatus(const char* saveTempPath, const char* saveRenamePath, const QString& jsonString);
	};

} //namespace Ui
#endif // SERVER_H
