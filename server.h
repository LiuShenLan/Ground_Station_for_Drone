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

class QTcpSocket;

namespace Ui{
    class MainWindow;

    class Server : public QWidget
    {
    public:
        QJsonObject json;       // 总信息
        QJsonObject jsonGPS;    // GPS信息
        QJsonObject jsonGimbal; // 无人机信息
        QJsonObject jsonBattery;    // 无人机电池信息

        Server(QWidget* parent);    // 窗口左侧(Info)监听栏初始化

        void sendMessage(QString infomation);   // 向所有TCP套接字socket_list写入information

    private:
        // listen Init
        QLineEdit *setIpAddress;    // listen 文本输入行
        QPushButton* save_log_button; // listen按钮
        QTextEdit *textEdit;    // listen信息
        QScrollBar *scrollbar;  // listen滚动条

        // TCP
        QTcpServer tcpServer_for_receive, tcpServer_for_send;   // TCP server
        QTcpSocket *tcpServerConnection_for_receive, *tcpServerConnection_for_send; // TCP socket
        QList<QTcpSocket*> *socket_list;    // TCP socket list
        QList<QHostAddress> ipList; // 主机上找到的所有IP地址

        // listen 函数
        void startListening();  // 开始监听 listen
        void updateServerProgress();    // 读取TCP socket信息并显示到listen窗口
        void acceptConnection_for_receive();    // TCP socket 读取信息初始化
        void acceptConnection_for_send();   // TCP socket 发送信息初始化
        void clear();   // 清除listen显示信息

        QJsonObject getJsonObjectFromString(const QString jsonString);  // 将string转换为Json文件

        // 不明定义
        qint64 totalBytes;     // 存放总大小信息
        QString fileName;      // 存放文件名
        QByteArray inBlock;    // 数据缓冲区
        QCheckBox *ifHostIp;
    //    void responseToCheckBox();
    //    void displayError(QAbstractSocket::SocketError socketError);
    };

} //namespace Ui
#endif // SERVER_H
