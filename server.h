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
    Server(QWidget* parent);    // 窗口左侧(Info)监听栏初始化
    QJsonObject json;       // 总信息
    QJsonObject jsonGPS;    // GPS信息
    QJsonObject jsonGimbal; // 无人机信息
    QJsonObject jsonBattery;    // 电池信息
    void sendMessage(QString infomation);   // 向所有TCP套接字socket_list写入information

private:

    QTcpServer tcpServer_for_receive, tcpServer_for_send;   // TCP server
    QTcpSocket *tcpServerConnection_for_receive, *tcpServerConnection_for_send; // TCP socket
    QList<QTcpSocket*> *socket_list;    // TCP socket list

    qint64 totalBytes;     // 存放总大小信息
    QString fileName;      // 存放文件名
    QByteArray inBlock;    // 数据缓冲区
    QScrollBar *scrollbar;

    QTextEdit *textEdit;    // listen信息
    QJsonObject getJsonObjectFromString(const QString jsonString);

    QCheckBox *ifHostIp;
    QLineEdit *setIpAddress;
    QPushButton* save_log_button; // listen按钮

    QList<QHostAddress> ipList;

//    void responseToCheckBox();
    void startListening();
    void updateServerProgress();    // 读取TCP socket信息并显示
    void acceptConnection_for_receive();    // TCP socket 读取信息初始化
    void acceptConnection_for_send();   // TCP socket 发送信息初始化
    void clear();   // 清除listen显示信息

  //  void displayError(QAbstractSocket::SocketError socketError);
};

} //namespace Ui
#endif // SERVER_H
