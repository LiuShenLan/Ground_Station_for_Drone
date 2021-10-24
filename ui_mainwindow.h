/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qwebengineview.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_3;    // 摄像头显示信息
    QHBoxLayout *horizontalLayout_9;
    QGroupBox *groupBox_5;
    QGroupBox *groupBox_4;
    QPushButton *enableVirtualStickButton;  // 允许虚拟控制按钮 Enable virtual Stick
    QSlider *yawSlider;     // 虚拟控制 航向滑块 Yaw
    QSlider *pitchSlider;   // 虚拟控制 俯仰滑块 Pitch
    QSlider *rollSlider;    // 虚拟控制 横滚滑块 Roll
    QLabel *yawLabel;
    QLabel *rollLabel;
    QPushButton *onRestStickValueButton;    // 虚拟控制 reset按钮
    QLabel *pitchLabel;
    QPushButton *disableVirtualStickButton; // 禁止虚拟控制按钮 Disable virtual Stick
    QSlider *throttleSlider;    // 虚拟控制 油门滑块 Throttle
    QLabel *throttleLabel;
    QPushButton *setRollButton; // 虚拟控制 横滚设置按钮 Roll Set
    QLabel *yawBiasLabel;
    QLabel *pitchBiasLabel;
    QLabel *throttleBiasLabel;
    QLabel *rollBiasLabel;
    QSlider *zoomSlider;    // 摄像头图像缩放滑块 Zoom
    QLabel *label_7;
    QPushButton *goStraightButton;  // 人工导航 直行按钮 Go straight
    QPushButton *turnLeftButton;    // 人工导航 左转按钮 Turn left
    QPushButton *turnRightButton;   // 人工导航 右转按钮 Turn right
    QLabel *turnLabel;
    QCheckBox *manulDirectCheckBox; // 人工导航确定框 Manul direct
    QSlider *navPointSlider;    // 添加导航点 导航点方向滑块
    QLabel *label_9;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout;
    QWebEngineView *webView;    // 地图显示
    QHBoxLayout *horizontalLayout_5;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_8;
    QLabel *VelV;   // 无人机信息 垂直速度
    QLabel *label_12;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_6;
    QLabel *VelH;   // 无人机信息 水平速度
    QLabel *label_14;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_10;
    QLabel *Bat;    // 无人机信息 电池剩余电量
    QLabel *label_13;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton;    // 地图刷新按钮 Map_Refresh
    QLabel *label;
    QLineEdit *lineEditLat; // GPS信息 纬度
    QLabel *label_2;
    QLineEdit *lineEditLng; // GPS信息 经度
    QLabel *label_4;
    QLabel *Alt;    // GPS信息 高度
    QGroupBox *groupBox1;
    QHBoxLayout *horizontalLayout_7;
    QComboBox *cbLight; // WayPoints 下拉菜单
    QPushButton *btnAdd;    // WayPoints Add按钮
    QPushButton *clearAllPointButton;   // WayPoints Clear按钮
    QPushButton *takeoffButton; // WayPoints TakeOff按钮
    QLineEdit *waypointAlt; // WayPoints 高度输入框
    QPushButton *goButton;  // WayPoints Go按钮
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *detect_carButton;  // Car detection detect_car按钮
    QPushButton *track_carButton;   // Car detection track_car按钮
    QPushButton *do_nothing;    // Car detection wait按钮
    QLabel *label_5;
    QLineEdit *lineEditnumber;  // Car detection number输入框
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1116, 688);
        MainWindow->setMinimumSize(QSize(0, 0));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        horizontalLayout_3 = new QHBoxLayout(centralWidget);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setMinimumSize(QSize(640, 320));
        label_3->setBaseSize(QSize(0, 0));
        label_3->setLineWidth(1);

        verticalLayout_2->addWidget(label_3);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        groupBox_5 = new QGroupBox(centralWidget);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        groupBox_4 = new QGroupBox(groupBox_5);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        groupBox_4->setGeometry(QRect(0, 0, 316, 301));
        enableVirtualStickButton = new QPushButton(groupBox_4);
        enableVirtualStickButton->setObjectName(QString::fromUtf8("enableVirtualStickButton"));
        enableVirtualStickButton->setGeometry(QRect(10, 30, 141, 25));
        yawSlider = new QSlider(groupBox_4);
        yawSlider->setObjectName(QString::fromUtf8("yawSlider"));
        yawSlider->setGeometry(QRect(10, 100, 121, 16));
        yawSlider->setOrientation(Qt::Horizontal);
        pitchSlider = new QSlider(groupBox_4);
        pitchSlider->setObjectName(QString::fromUtf8("pitchSlider"));
        pitchSlider->setGeometry(QRect(169, 100, 131, 20));
        pitchSlider->setOrientation(Qt::Horizontal);
        rollSlider = new QSlider(groupBox_4);
        rollSlider->setObjectName(QString::fromUtf8("rollSlider"));
        rollSlider->setGeometry(QRect(210, 130, 16, 121));
        rollSlider->setOrientation(Qt::Vertical);
        yawLabel = new QLabel(groupBox_4);
        yawLabel->setObjectName(QString::fromUtf8("yawLabel"));
        yawLabel->setGeometry(QRect(20, 80, 67, 17));
        rollLabel = new QLabel(groupBox_4);
        rollLabel->setObjectName(QString::fromUtf8("rollLabel"));
        rollLabel->setGeometry(QRect(230, 170, 31, 16));
        onRestStickValueButton = new QPushButton(groupBox_4);
        onRestStickValueButton->setObjectName(QString::fromUtf8("onRestStickValueButton"));
        onRestStickValueButton->setGeometry(QRect(100, 260, 89, 25));
        pitchLabel = new QLabel(groupBox_4);
        pitchLabel->setObjectName(QString::fromUtf8("pitchLabel"));
        pitchLabel->setGeometry(QRect(190, 80, 67, 17));
        disableVirtualStickButton = new QPushButton(groupBox_4);
        disableVirtualStickButton->setObjectName(QString::fromUtf8("disableVirtualStickButton"));
        disableVirtualStickButton->setGeometry(QRect(170, 30, 131, 25));
        throttleSlider = new QSlider(groupBox_4);
        throttleSlider->setObjectName(QString::fromUtf8("throttleSlider"));
        throttleSlider->setGeometry(QRect(60, 130, 16, 121));
        throttleSlider->setOrientation(Qt::Vertical);
        throttleLabel = new QLabel(groupBox_4);
        throttleLabel->setObjectName(QString::fromUtf8("throttleLabel"));
        throttleLabel->setGeometry(QRect(80, 170, 67, 17));
        setRollButton = new QPushButton(groupBox_4);
        setRollButton->setObjectName(QString::fromUtf8("setRollButton"));
        setRollButton->setGeometry(QRect(230, 190, 61, 31));
        yawBiasLabel = new QLabel(groupBox_4);
        yawBiasLabel->setObjectName(QString::fromUtf8("yawBiasLabel"));
        yawBiasLabel->setGeometry(QRect(20, 60, 67, 17));
        pitchBiasLabel = new QLabel(groupBox_4);
        pitchBiasLabel->setObjectName(QString::fromUtf8("pitchBiasLabel"));
        pitchBiasLabel->setGeometry(QRect(190, 60, 67, 17));
        throttleBiasLabel = new QLabel(groupBox_4);
        throttleBiasLabel->setObjectName(QString::fromUtf8("throttleBiasLabel"));
        throttleBiasLabel->setGeometry(QRect(80, 150, 67, 17));
        rollBiasLabel = new QLabel(groupBox_4);
        rollBiasLabel->setObjectName(QString::fromUtf8("rollBiasLabel"));
        rollBiasLabel->setGeometry(QRect(230, 150, 67, 17));
        zoomSlider = new QSlider(groupBox_5);
        zoomSlider->setObjectName(QString::fromUtf8("zoomSlider"));
        zoomSlider->setGeometry(QRect(430, 40, 160, 16));
        zoomSlider->setOrientation(Qt::Horizontal);
        label_7 = new QLabel(groupBox_5);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(350, 40, 67, 17));
        goStraightButton = new QPushButton(groupBox_5);
        goStraightButton->setObjectName(QString::fromUtf8("goStraightButton"));
        goStraightButton->setGeometry(QRect(430, 240, 89, 21));
        turnLeftButton = new QPushButton(groupBox_5);
        turnLeftButton->setObjectName(QString::fromUtf8("turnLeftButton"));
        turnLeftButton->setGeometry(QRect(330, 240, 91, 21));
        turnRightButton = new QPushButton(groupBox_5);
        turnRightButton->setObjectName(QString::fromUtf8("turnRightButton"));
        turnRightButton->setGeometry(QRect(530, 240, 89, 21));
        turnLabel = new QLabel(groupBox_5);
        turnLabel->setObjectName(QString::fromUtf8("turnLabel"));
        turnLabel->setGeometry(QRect(420, 180, 111, 51));
        manulDirectCheckBox = new QCheckBox(groupBox_5);
        manulDirectCheckBox->setObjectName(QString::fromUtf8("manulDirectCheckBox"));
        manulDirectCheckBox->setGeometry(QRect(330, 270, 151, 23));
        navPointSlider = new QSlider(groupBox_5);
        navPointSlider->setObjectName(QString::fromUtf8("navPointSlider"));
        navPointSlider->setGeometry(QRect(430, 90, 160, 16));
        navPointSlider->setOrientation(Qt::Horizontal);
        label_9 = new QLabel(groupBox_5);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(350, 90, 67, 17));

        horizontalLayout_9->addWidget(groupBox_5);


        verticalLayout_2->addLayout(horizontalLayout_9);


        horizontalLayout_3->addLayout(verticalLayout_2);

        groupBox_3 = new QGroupBox(centralWidget);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setMinimumSize(QSize(450, 0));
        groupBox_3->setMaximumSize(QSize(200, 16777215));
        verticalLayout = new QVBoxLayout(groupBox_3);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        webView = new QWebEngineView(groupBox_3);
        webView->setObjectName(QString::fromUtf8("webView"));
        webView->setEnabled(true);
        webView->setMinimumSize(QSize(0, 300));
        webView->setMaximumSize(QSize(16777215, 16777215));

        verticalLayout->addWidget(webView);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        label_8 = new QLabel(groupBox_3);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        horizontalLayout_8->addWidget(label_8);

        VelV = new QLabel(groupBox_3);
        VelV->setObjectName(QString::fromUtf8("VelV"));

        horizontalLayout_8->addWidget(VelV);

        label_12 = new QLabel(groupBox_3);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setMinimumSize(QSize(0, 0));

        horizontalLayout_8->addWidget(label_12);


        horizontalLayout_5->addLayout(horizontalLayout_8);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        label_6 = new QLabel(groupBox_3);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        horizontalLayout_6->addWidget(label_6);

        VelH = new QLabel(groupBox_3);
        VelH->setObjectName(QString::fromUtf8("VelH"));
        VelH->setMinimumSize(QSize(0, 30));

        horizontalLayout_6->addWidget(VelH);

        label_14 = new QLabel(groupBox_3);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        horizontalLayout_6->addWidget(label_14);


        horizontalLayout_5->addLayout(horizontalLayout_6);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_10 = new QLabel(groupBox_3);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        horizontalLayout_2->addWidget(label_10);

        Bat = new QLabel(groupBox_3);
        Bat->setObjectName(QString::fromUtf8("Bat"));

        horizontalLayout_2->addWidget(Bat);

        label_13 = new QLabel(groupBox_3);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        horizontalLayout_2->addWidget(label_13);


        horizontalLayout_5->addLayout(horizontalLayout_2);


        verticalLayout->addLayout(horizontalLayout_5);

        groupBox = new QGroupBox(groupBox_3);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        horizontalLayout = new QHBoxLayout(groupBox);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pushButton = new QPushButton(groupBox);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        horizontalLayout->addWidget(pushButton);

        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        lineEditLat = new QLineEdit(groupBox);
        lineEditLat->setObjectName(QString::fromUtf8("lineEditLat"));
        lineEditLat->setMinimumSize(QSize(60, 0));

        horizontalLayout->addWidget(lineEditLat);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout->addWidget(label_2);

        lineEditLng = new QLineEdit(groupBox);
        lineEditLng->setObjectName(QString::fromUtf8("lineEditLng"));
        lineEditLng->setMinimumSize(QSize(60, 0));
        lineEditLng->setCursorPosition(8);

        horizontalLayout->addWidget(lineEditLng);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout->addWidget(label_4);

        Alt = new QLabel(groupBox);
        Alt->setObjectName(QString::fromUtf8("Alt"));

        horizontalLayout->addWidget(Alt);


        verticalLayout->addWidget(groupBox);

        groupBox1 = new QGroupBox(groupBox_3);
        groupBox1->setObjectName(QString::fromUtf8("groupBox1"));
        horizontalLayout_7 = new QHBoxLayout(groupBox1);
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        cbLight = new QComboBox(groupBox1);
        cbLight->setObjectName(QString::fromUtf8("cbLight"));

        horizontalLayout_7->addWidget(cbLight);

        btnAdd = new QPushButton(groupBox1);
        btnAdd->setObjectName(QString::fromUtf8("btnAdd"));

        horizontalLayout_7->addWidget(btnAdd);

        clearAllPointButton = new QPushButton(groupBox1);
        clearAllPointButton->setObjectName(QString::fromUtf8("clearAllPointButton"));

        horizontalLayout_7->addWidget(clearAllPointButton);

        takeoffButton = new QPushButton(groupBox1);
        takeoffButton->setObjectName(QString::fromUtf8("takeoffButton"));

        horizontalLayout_7->addWidget(takeoffButton);

        waypointAlt = new QLineEdit(groupBox1);
        waypointAlt->setObjectName(QString::fromUtf8("waypointAlt"));

        horizontalLayout_7->addWidget(waypointAlt);

        goButton = new QPushButton(groupBox1);
        goButton->setObjectName(QString::fromUtf8("goButton"));

        horizontalLayout_7->addWidget(goButton);


        verticalLayout->addWidget(groupBox1);

        groupBox_2 = new QGroupBox(groupBox_3);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        horizontalLayout_4 = new QHBoxLayout(groupBox_2);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        detect_carButton = new QPushButton(groupBox_2);
        detect_carButton->setObjectName(QString::fromUtf8("detect_carButton"));

        horizontalLayout_4->addWidget(detect_carButton);

        track_carButton = new QPushButton(groupBox_2);
        track_carButton->setObjectName(QString::fromUtf8("track_carButton"));

        horizontalLayout_4->addWidget(track_carButton);

        do_nothing = new QPushButton(groupBox_2);
        do_nothing->setObjectName(QString::fromUtf8("do_nothing"));

        horizontalLayout_4->addWidget(do_nothing);

        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout_4->addWidget(label_5);

        lineEditnumber = new QLineEdit(groupBox_2);
        lineEditnumber->setObjectName(QString::fromUtf8("lineEditnumber"));

        horizontalLayout_4->addWidget(lineEditnumber);


        verticalLayout->addWidget(groupBox_2);


        horizontalLayout_3->addWidget(groupBox_3);

        MainWindow->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        label_3->setText(QString());
        groupBox_5->setTitle(QApplication::translate("MainWindow", "                                                                                             Add navigation point", nullptr));
        groupBox_4->setTitle(QApplication::translate("MainWindow", "Virtual stick", nullptr));
        enableVirtualStickButton->setText(QApplication::translate("MainWindow", "Enable virtual Stick", nullptr));
        yawLabel->setText(QApplication::translate("MainWindow", "Yaw", nullptr));
        rollLabel->setText(QApplication::translate("MainWindow", "Roll", nullptr));
        onRestStickValueButton->setText(QApplication::translate("MainWindow", "reset", nullptr));
        pitchLabel->setText(QApplication::translate("MainWindow", "Pitch", nullptr));
        disableVirtualStickButton->setText(QApplication::translate("MainWindow", "Disable virtual S", nullptr));
        throttleLabel->setText(QApplication::translate("MainWindow", "Throttle", nullptr));
        setRollButton->setText(QApplication::translate("MainWindow", "Set", nullptr));
        yawBiasLabel->setText(QString());
        pitchBiasLabel->setText(QString());
        throttleBiasLabel->setText(QString());
        rollBiasLabel->setText(QString());
        label_7->setText(QApplication::translate("MainWindow", "Zoom", nullptr));
        goStraightButton->setText(QApplication::translate("MainWindow", "Go straight", nullptr));
        turnLeftButton->setText(QApplication::translate("MainWindow", "Turn left", nullptr));
        turnRightButton->setText(QApplication::translate("MainWindow", "Turn right", nullptr));
        turnLabel->setText(QString());
        manulDirectCheckBox->setText(QApplication::translate("MainWindow", "Manul direct", nullptr));
        label_9->setText(QApplication::translate("MainWindow", "Nav point", nullptr));
        groupBox_3->setTitle(QApplication::translate("MainWindow", "Map", nullptr));
        label_8->setText(QApplication::translate("MainWindow", "Velocity_V:", nullptr));
        VelV->setText(QApplication::translate("MainWindow", "3.3", nullptr));
        label_12->setText(QApplication::translate("MainWindow", "m/s", nullptr));
        label_6->setText(QApplication::translate("MainWindow", "Velocity_H:", nullptr));
        VelH->setText(QApplication::translate("MainWindow", "3.3", nullptr));
        label_14->setText(QApplication::translate("MainWindow", "m/s", nullptr));
        label_10->setText(QApplication::translate("MainWindow", "Battery:", nullptr));
        Bat->setText(QApplication::translate("MainWindow", "99", nullptr));
        label_13->setText(QApplication::translate("MainWindow", "%", nullptr));
        groupBox->setTitle(QApplication::translate("MainWindow", "GPS Data", nullptr));
        pushButton->setText(QApplication::translate("MainWindow", "Map_Refresh", nullptr));
        label->setText(QApplication::translate("MainWindow", "Lat", nullptr));
        lineEditLat->setText(QApplication::translate("MainWindow", "29.6169", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "Lng", nullptr));
        lineEditLng->setText(QApplication::translate("MainWindow", "106.4988", nullptr));
        label_4->setText(QApplication::translate("MainWindow", " Alt ", nullptr));
        Alt->setText(QApplication::translate("MainWindow", "000.0", nullptr));
        groupBox1->setTitle(QApplication::translate("MainWindow", "WayPoints", nullptr));
        btnAdd->setText(QApplication::translate("MainWindow", "Add", nullptr));
        clearAllPointButton->setText(QApplication::translate("MainWindow", "Clear", nullptr));
        takeoffButton->setText(QApplication::translate("MainWindow", "TakeOff", nullptr));
        goButton->setText(QApplication::translate("MainWindow", "Go", nullptr));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Car detection", nullptr));
        detect_carButton->setText(QApplication::translate("MainWindow", "detect_car", nullptr));
        track_carButton->setText(QApplication::translate("MainWindow", "track_car", nullptr));
        do_nothing->setText(QApplication::translate("MainWindow", "wait", nullptr));
        label_5->setText(QApplication::translate("MainWindow", "     number:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
