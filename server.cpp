#include "server.h"
#include "bridge.h"

namespace Ui {
    // Init
    Server::Server(QWidget* parent)
    {
        setWindowFlags(Qt::Window);
        setWindowTitle("Log");
        resize(320, 640);

        auto* grid = new QGridLayout(this);  // 网格布局
        grid->setContentsMargins(5, 10, 5, 5);  // 设置内容边距

        auto* upper_button_layout = new QHBoxLayout();   // 水平布局

        setIpAddress = new QLineEdit(); // 单行文本编辑器
        upper_button_layout->addWidget(setIpAddress);

    //    ifHostIp = new QCheckBox(tr("Host IP"),this); //check box button is useless
    //    upper_button_layout->addWidget(ifHostIp);
    //    connect(ifHostIp, &QCheckBox::stateChanged, this, &Server::responseToCheckBox);

        /******To get possible host ip address******/
        ipList = QNetworkInterface::allAddresses(); // 返回主机上找到的所有的IP地址
        foreach (QHostAddress address, ipList)
        {
            if(address.protocol() == QAbstractSocket::IPv4Protocol) //我们使用IPv4地址
                qDebug() <<"IP Address: "<<address.toString();
    //            setIpAddress->setText(QString::fromStdString(address.toString()));
        }


        auto* lower_button_layout = new QHBoxLayout();

        // listen按钮
        save_log_button = new QPushButton(tr("listen"), this);
        connect(save_log_button, &QPushButton::released, this, &Server::startListening);
        lower_button_layout->addWidget(save_log_button);
        // clear按钮
        auto* clear_button = new QPushButton(tr("Clear"), this);
        connect(clear_button, &QPushButton::released, this, &Server::clear);
        lower_button_layout->addWidget(clear_button);

        grid->addLayout(lower_button_layout, 1, 0, Qt::AlignLeft);

        grid->addLayout(upper_button_layout, 0, 0, Qt::AlignLeft);

        textEdit = new QTextEdit(this);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setReadOnly(true);
        grid->addWidget(textEdit, 2, 0, 1, 2);


        tcpServerConnection_for_receive = new QTcpSocket(this);
        tcpServerConnection_for_send = new QTcpSocket(this);
        socket_list = new QList<QTcpSocket*>;
        connect(&tcpServer_for_receive, &QTcpServer::newConnection,
                this, &Server::acceptConnection_for_receive);
        connect(&tcpServer_for_send, &QTcpServer::newConnection,
                this, &Server::acceptConnection_for_send);
        totalBytes = 0;

    }

    // TCP
    void Server::sendMessage(const QString& infomation)
    {
        qDebug() << "send data to socket";
        QByteArray message = infomation.toLocal8Bit();  // 将字符串的本地8位表示作为QByteArray返回
    //    QDataStream out(&message, QIODevice::WriteOnly);
    //    out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
    //    out<<infomation.toLocal8Bit();
        //qDebug()<<message;
        for(auto i : *socket_list)
            i->write(message);
    }

    // lsiten
    void Server::startListening()
    {
        //   if (!tcpServer.listen(QHostAddress::LocalHost, 6666)) {
         if (!tcpServer_for_receive.isListening() && !tcpServer_for_receive.listen(QHostAddress::Any, 6666))  //QHostAddress::Any = 4 	双any-address栈，与该地址绑定的socket将侦听IPv4和IPv6接口。
         {  // tcpServer_for_receive没有正在监听传入链接并且没有监听6666端口
               qDebug() <<"error_in_sever_for_receive"<< tcpServer_for_receive.errorString();
               close();
               return;
         }
         if(tcpServer_for_receive.isListening())
         {
             save_log_button->setEnabled(false);
             qDebug() <<"=============start listening==========";
         }
         if (!tcpServer_for_send.isListening() && !tcpServer_for_send.listen(QHostAddress::Any, 6665))  //QHostAddress::Any = 4 	双any-address栈，与该地址绑定的socket将侦听IPv4和IPv6接口。
         {  // tcpServer_for_send没有正在监听传入链接并且没有监听6665端口
               qDebug() <<"error_in_sever_for_send"<< tcpServer_for_send.errorString();
               close();
               return;
         }
         if(tcpServer_for_send.isListening())
         {
             qDebug() <<"=============ok for send==========";
         }
    }
    void Server::updateServerProgress()
    {
        qDebug() << "read data form socket";
        // 时间
        QDateTime time = QDateTime::currentDateTime();
        QString str = time.toString("\n[ hh:mm:ss ]"); // 时间信息，设置显示格式
        //qDebug()<<str;

        // 读取数据
        qint64 len = tcpServerConnection_for_receive->bytesAvailable();
        //qDebug()<<"socket data len:"<< len;
        QByteArray alldata = tcpServerConnection_for_receive->read(len);
        //开始转换编码
        QTextCodec *utf8codec = QTextCodec::codecForName("UTF-8");
        QString utf8str = utf8codec->toUnicode(alldata.mid(2));
//        qDebug()<<"hex:["<<alldata.toHex().toUpper()<<"]";
//        qDebug()<<"utf-8 ["<< (utf8str) << "]";


        json = getJsonObjectFromString(utf8str);

        if(json.contains("GPS")){
            jsonGPS = json["GPS"].toObject();
//            qDebug()<<"\n->"<< jsonGPS["altitude"].toDouble();
        }

        if(json.contains("Gimbal")){
            jsonGimbal = json["Gimbal"].toObject();
        }
        if(json.contains("Battery0")){
            jsonBattery = json["Battery0"].toObject();
        }

        //显示到lesten状态栏上
        textEdit->insertPlainText(str);//在标签上显示时间
        textEdit->insertPlainText(utf8str);
        textEdit->insertPlainText(tr("\n"));


        //自动滚动到最底
        scrollbar = textEdit->verticalScrollBar();
        if (scrollbar)
            scrollbar->setSliderPosition(scrollbar->maximum());

//        QString ser = "test,test,test,test,test,test\n";
//        sendMessage(ser);
    }
    void Server::acceptConnection_for_receive()
    {
        qDebug() << "============== tcpServer receive connect ==============";
        tcpServerConnection_for_receive = tcpServer_for_receive.nextPendingConnection();
        connect(tcpServerConnection_for_receive, &QTcpSocket::readyRead,
                this, &Server::updateServerProgress);
        //    connect(tcpServerConnection, SIGNAL(error(QAbstractSocket::SocketError)),
        //            this, SLOT(displayError(QAbstractSocket::SocketError)));
        //  ui->serverStatusLabel->setText(tr("接受连接"));
        //  关闭服务器，不再进行监听
        //  tcpServer.close();
    }
    void Server::acceptConnection_for_send()
    {
        qDebug() << "============== tcpServer send connect ==============";
        tcpServerConnection_for_send = tcpServer_for_send.nextPendingConnection();
        socket_list->append(tcpServerConnection_for_send);
        qDebug() <<"============== found send target=========";
    }
    void Server::clear(){
        textEdit->clear();
    }

    QJsonObject Server::getJsonObjectFromString(const QString& jsonString){
        QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toLocal8Bit().data());
        if( jsonDocument.isNull() ){
            qDebug()<< "===> please check the string "<< jsonString.toLocal8Bit().data();
        }
        QJsonObject jsonObject = jsonDocument.object();

        saveUAVStatus(SAVE_JSON_TEMP_PATH, SAVE_JSON_RENAME_PATH, jsonString);  // 保存QJson文件

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
