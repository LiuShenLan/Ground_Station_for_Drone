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
    QVBoxLayout *lWindow;
    QLabel *camShow;
    QHBoxLayout *virtualStickLayout;
    QGroupBox *manualControlGBox;
    QGroupBox *virtualStickGBox;
    QPushButton *virtualStickEnableBtn;
    QSlider *virtualStickYawSlider;
    QSlider *virtualStickPitchSlider;
    QSlider *virtualStickRollSlider;
    QLabel *yawLabel;
    QLabel *rollLabel;
    QPushButton *virtualStickResetBtn;
    QLabel *pitchLabel;
    QPushButton *virtualStickDisableBtn;
    QSlider *virtualStickThrottleSlider;
    QLabel *throttleLabel;
    QPushButton *virtualStickRollSetBtn;
    QLabel *yawBiasLabel;
    QLabel *pitchBiasLabel;
    QLabel *throttleBiasLabel;
    QLabel *rollBiasLabel;
    QSlider *camZoomSlider;
    QLabel *label_7;
    QPushButton *manualDirectGoStraightBtn;
    QPushButton *manualDirectTurnLeftBtn;
    QPushButton *manualDirectTurnRightBtn;
    QLabel *turnLabel;
    QCheckBox *manualDirectCheckBox;
    QSlider *navigationPointDirectSlider;
    QLabel *label_9;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout;
    QWebEngineView *mapShow;
    QHBoxLayout *horizontalLayout_5;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_8;
    QLabel *uavStatusVelocityV;
    QLabel *label_12;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_6;
    QLabel *uavStatusVelocityH;
    QLabel *label_14;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_10;
    QLabel *uavStatusBattery;
    QLabel *label_13;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton;
    QLabel *label;
    QLineEdit *GPSLatitudeLineEdit;
    QLabel *label_2;
    QLineEdit *GPSLongitudeLineEdit;
    QLabel *label_4;
    QLabel *GPSHeight;
    QGroupBox *groupBox1;
    QHBoxLayout *horizontalLayout_7;
    QComboBox *wayPointsComboBox;
    QPushButton *wayPointsAddBtn;
    QPushButton *wayPointsClearBtn;
    QPushButton *wayPointsTakeOffBtn;
    QLineEdit *wayPointsHeightLineEdit;
    QPushButton *wayPointsGoBtn;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *carDetectBtn;
    QPushButton *carDetectTrackCarBtn;
    QPushButton *carDetectWaitBtn;
    QLabel *label_5;
    QLineEdit *carDetectNumberLineEdit;
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
        lWindow = new QVBoxLayout();
        lWindow->setSpacing(6);
        lWindow->setObjectName(QString::fromUtf8("lWindow"));
        camShow = new QLabel(centralWidget);
        camShow->setObjectName(QString::fromUtf8("camShow"));
        camShow->setMinimumSize(QSize(640, 320));
        camShow->setBaseSize(QSize(0, 0));
        camShow->setLineWidth(1);

        lWindow->addWidget(camShow);

        virtualStickLayout = new QHBoxLayout();
        virtualStickLayout->setSpacing(6);
        virtualStickLayout->setObjectName(QString::fromUtf8("virtualStickLayout"));
        manualControlGBox = new QGroupBox(centralWidget);
        manualControlGBox->setObjectName(QString::fromUtf8("manualControlGBox"));
        virtualStickGBox = new QGroupBox(manualControlGBox);
        virtualStickGBox->setObjectName(QString::fromUtf8("virtualStickGBox"));
        virtualStickGBox->setGeometry(QRect(0, 0, 316, 301));
        virtualStickEnableBtn = new QPushButton(virtualStickGBox);
        virtualStickEnableBtn->setObjectName(QString::fromUtf8("virtualStickEnableBtn"));
        virtualStickEnableBtn->setGeometry(QRect(10, 30, 141, 25));
        virtualStickYawSlider = new QSlider(virtualStickGBox);
        virtualStickYawSlider->setObjectName(QString::fromUtf8("virtualStickYawSlider"));
        virtualStickYawSlider->setGeometry(QRect(10, 100, 121, 16));
        virtualStickYawSlider->setOrientation(Qt::Horizontal);
        virtualStickPitchSlider = new QSlider(virtualStickGBox);
        virtualStickPitchSlider->setObjectName(QString::fromUtf8("virtualStickPitchSlider"));
        virtualStickPitchSlider->setGeometry(QRect(169, 100, 131, 20));
        virtualStickPitchSlider->setOrientation(Qt::Horizontal);
        virtualStickRollSlider = new QSlider(virtualStickGBox);
        virtualStickRollSlider->setObjectName(QString::fromUtf8("virtualStickRollSlider"));
        virtualStickRollSlider->setGeometry(QRect(210, 130, 16, 121));
        virtualStickRollSlider->setOrientation(Qt::Vertical);
        yawLabel = new QLabel(virtualStickGBox);
        yawLabel->setObjectName(QString::fromUtf8("yawLabel"));
        yawLabel->setGeometry(QRect(20, 80, 67, 17));
        rollLabel = new QLabel(virtualStickGBox);
        rollLabel->setObjectName(QString::fromUtf8("rollLabel"));
        rollLabel->setGeometry(QRect(230, 170, 31, 16));
        virtualStickResetBtn = new QPushButton(virtualStickGBox);
        virtualStickResetBtn->setObjectName(QString::fromUtf8("virtualStickResetBtn"));
        virtualStickResetBtn->setGeometry(QRect(100, 260, 89, 25));
        pitchLabel = new QLabel(virtualStickGBox);
        pitchLabel->setObjectName(QString::fromUtf8("pitchLabel"));
        pitchLabel->setGeometry(QRect(190, 80, 67, 17));
        virtualStickDisableBtn = new QPushButton(virtualStickGBox);
        virtualStickDisableBtn->setObjectName(QString::fromUtf8("virtualStickDisableBtn"));
        virtualStickDisableBtn->setGeometry(QRect(170, 30, 131, 25));
        virtualStickThrottleSlider = new QSlider(virtualStickGBox);
        virtualStickThrottleSlider->setObjectName(QString::fromUtf8("virtualStickThrottleSlider"));
        virtualStickThrottleSlider->setGeometry(QRect(60, 130, 16, 121));
        virtualStickThrottleSlider->setOrientation(Qt::Vertical);
        throttleLabel = new QLabel(virtualStickGBox);
        throttleLabel->setObjectName(QString::fromUtf8("throttleLabel"));
        throttleLabel->setGeometry(QRect(80, 170, 67, 17));
        virtualStickRollSetBtn = new QPushButton(virtualStickGBox);
        virtualStickRollSetBtn->setObjectName(QString::fromUtf8("virtualStickRollSetBtn"));
        virtualStickRollSetBtn->setGeometry(QRect(230, 190, 61, 31));
        yawBiasLabel = new QLabel(virtualStickGBox);
        yawBiasLabel->setObjectName(QString::fromUtf8("yawBiasLabel"));
        yawBiasLabel->setGeometry(QRect(20, 60, 67, 17));
        pitchBiasLabel = new QLabel(virtualStickGBox);
        pitchBiasLabel->setObjectName(QString::fromUtf8("pitchBiasLabel"));
        pitchBiasLabel->setGeometry(QRect(190, 60, 67, 17));
        throttleBiasLabel = new QLabel(virtualStickGBox);
        throttleBiasLabel->setObjectName(QString::fromUtf8("throttleBiasLabel"));
        throttleBiasLabel->setGeometry(QRect(80, 150, 67, 17));
        rollBiasLabel = new QLabel(virtualStickGBox);
        rollBiasLabel->setObjectName(QString::fromUtf8("rollBiasLabel"));
        rollBiasLabel->setGeometry(QRect(230, 150, 67, 17));
        camZoomSlider = new QSlider(manualControlGBox);
        camZoomSlider->setObjectName(QString::fromUtf8("camZoomSlider"));
        camZoomSlider->setGeometry(QRect(430, 40, 160, 16));
        camZoomSlider->setOrientation(Qt::Horizontal);
        label_7 = new QLabel(manualControlGBox);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(350, 40, 67, 17));
        manualDirectGoStraightBtn = new QPushButton(manualControlGBox);
        manualDirectGoStraightBtn->setObjectName(QString::fromUtf8("manualDirectGoStraightBtn"));
        manualDirectGoStraightBtn->setGeometry(QRect(430, 240, 89, 21));
        manualDirectTurnLeftBtn = new QPushButton(manualControlGBox);
        manualDirectTurnLeftBtn->setObjectName(QString::fromUtf8("manualDirectTurnLeftBtn"));
        manualDirectTurnLeftBtn->setGeometry(QRect(330, 240, 91, 21));
        manualDirectTurnRightBtn = new QPushButton(manualControlGBox);
        manualDirectTurnRightBtn->setObjectName(QString::fromUtf8("manualDirectTurnRightBtn"));
        manualDirectTurnRightBtn->setGeometry(QRect(530, 240, 89, 21));
        turnLabel = new QLabel(manualControlGBox);
        turnLabel->setObjectName(QString::fromUtf8("turnLabel"));
        turnLabel->setGeometry(QRect(420, 180, 111, 51));
        manualDirectCheckBox = new QCheckBox(manualControlGBox);
        manualDirectCheckBox->setObjectName(QString::fromUtf8("manualDirectCheckBox"));
        manualDirectCheckBox->setGeometry(QRect(330, 270, 151, 23));
        navigationPointDirectSlider = new QSlider(manualControlGBox);
        navigationPointDirectSlider->setObjectName(QString::fromUtf8("navigationPointDirectSlider"));
        navigationPointDirectSlider->setGeometry(QRect(430, 90, 160, 16));
        navigationPointDirectSlider->setOrientation(Qt::Horizontal);
        label_9 = new QLabel(manualControlGBox);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(350, 90, 67, 17));

        virtualStickLayout->addWidget(manualControlGBox);


        lWindow->addLayout(virtualStickLayout);


        horizontalLayout_3->addLayout(lWindow);

        groupBox_3 = new QGroupBox(centralWidget);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setMinimumSize(QSize(450, 0));
        groupBox_3->setMaximumSize(QSize(200, 16777215));
        verticalLayout = new QVBoxLayout(groupBox_3);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        mapShow = new QWebEngineView(groupBox_3);
        mapShow->setObjectName(QString::fromUtf8("mapShow"));
        mapShow->setEnabled(true);
        mapShow->setMinimumSize(QSize(0, 300));
        mapShow->setMaximumSize(QSize(16777215, 16777215));

        verticalLayout->addWidget(mapShow);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        label_8 = new QLabel(groupBox_3);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        horizontalLayout_8->addWidget(label_8);

        uavStatusVelocityV = new QLabel(groupBox_3);
        uavStatusVelocityV->setObjectName(QString::fromUtf8("uavStatusVelocityV"));

        horizontalLayout_8->addWidget(uavStatusVelocityV);

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

        uavStatusVelocityH = new QLabel(groupBox_3);
        uavStatusVelocityH->setObjectName(QString::fromUtf8("uavStatusVelocityH"));
        uavStatusVelocityH->setMinimumSize(QSize(0, 30));

        horizontalLayout_6->addWidget(uavStatusVelocityH);

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

        uavStatusBattery = new QLabel(groupBox_3);
        uavStatusBattery->setObjectName(QString::fromUtf8("uavStatusBattery"));

        horizontalLayout_2->addWidget(uavStatusBattery);

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

        GPSLatitudeLineEdit = new QLineEdit(groupBox);
        GPSLatitudeLineEdit->setObjectName(QString::fromUtf8("GPSLatitudeLineEdit"));
        GPSLatitudeLineEdit->setMinimumSize(QSize(60, 0));

        horizontalLayout->addWidget(GPSLatitudeLineEdit);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout->addWidget(label_2);

        GPSLongitudeLineEdit = new QLineEdit(groupBox);
        GPSLongitudeLineEdit->setObjectName(QString::fromUtf8("GPSLongitudeLineEdit"));
        GPSLongitudeLineEdit->setMinimumSize(QSize(60, 0));
        GPSLongitudeLineEdit->setCursorPosition(8);

        horizontalLayout->addWidget(GPSLongitudeLineEdit);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout->addWidget(label_4);

        GPSHeight = new QLabel(groupBox);
        GPSHeight->setObjectName(QString::fromUtf8("GPSHeight"));

        horizontalLayout->addWidget(GPSHeight);


        verticalLayout->addWidget(groupBox);

        groupBox1 = new QGroupBox(groupBox_3);
        groupBox1->setObjectName(QString::fromUtf8("groupBox1"));
        horizontalLayout_7 = new QHBoxLayout(groupBox1);
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        wayPointsComboBox = new QComboBox(groupBox1);
        wayPointsComboBox->setObjectName(QString::fromUtf8("wayPointsComboBox"));

        horizontalLayout_7->addWidget(wayPointsComboBox);

        wayPointsAddBtn = new QPushButton(groupBox1);
        wayPointsAddBtn->setObjectName(QString::fromUtf8("wayPointsAddBtn"));

        horizontalLayout_7->addWidget(wayPointsAddBtn);

        wayPointsClearBtn = new QPushButton(groupBox1);
        wayPointsClearBtn->setObjectName(QString::fromUtf8("wayPointsClearBtn"));

        horizontalLayout_7->addWidget(wayPointsClearBtn);

        wayPointsTakeOffBtn = new QPushButton(groupBox1);
        wayPointsTakeOffBtn->setObjectName(QString::fromUtf8("wayPointsTakeOffBtn"));

        horizontalLayout_7->addWidget(wayPointsTakeOffBtn);

        wayPointsHeightLineEdit = new QLineEdit(groupBox1);
        wayPointsHeightLineEdit->setObjectName(QString::fromUtf8("wayPointsHeightLineEdit"));

        horizontalLayout_7->addWidget(wayPointsHeightLineEdit);

        wayPointsGoBtn = new QPushButton(groupBox1);
        wayPointsGoBtn->setObjectName(QString::fromUtf8("wayPointsGoBtn"));

        horizontalLayout_7->addWidget(wayPointsGoBtn);


        verticalLayout->addWidget(groupBox1);

        groupBox_2 = new QGroupBox(groupBox_3);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        horizontalLayout_4 = new QHBoxLayout(groupBox_2);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        carDetectBtn = new QPushButton(groupBox_2);
        carDetectBtn->setObjectName(QString::fromUtf8("carDetectBtn"));

        horizontalLayout_4->addWidget(carDetectBtn);

        carDetectTrackCarBtn = new QPushButton(groupBox_2);
        carDetectTrackCarBtn->setObjectName(QString::fromUtf8("carDetectTrackCarBtn"));

        horizontalLayout_4->addWidget(carDetectTrackCarBtn);

        carDetectWaitBtn = new QPushButton(groupBox_2);
        carDetectWaitBtn->setObjectName(QString::fromUtf8("carDetectWaitBtn"));

        horizontalLayout_4->addWidget(carDetectWaitBtn);

        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout_4->addWidget(label_5);

        carDetectNumberLineEdit = new QLineEdit(groupBox_2);
        carDetectNumberLineEdit->setObjectName(QString::fromUtf8("carDetectNumberLineEdit"));

        horizontalLayout_4->addWidget(carDetectNumberLineEdit);


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
        camShow->setText(QString());
        manualControlGBox->setTitle(QApplication::translate("MainWindow", "                                                                                             Add navigation point", nullptr));
        virtualStickGBox->setTitle(QApplication::translate("MainWindow", "Virtual stick", nullptr));
        virtualStickEnableBtn->setText(QApplication::translate("MainWindow", "Enable virtual Stick", nullptr));
        yawLabel->setText(QApplication::translate("MainWindow", "Yaw", nullptr));
        rollLabel->setText(QApplication::translate("MainWindow", "Roll", nullptr));
        virtualStickResetBtn->setText(QApplication::translate("MainWindow", "reset", nullptr));
        pitchLabel->setText(QApplication::translate("MainWindow", "Pitch", nullptr));
        virtualStickDisableBtn->setText(QApplication::translate("MainWindow", "Disable virtual S", nullptr));
        throttleLabel->setText(QApplication::translate("MainWindow", "Throttle", nullptr));
        virtualStickRollSetBtn->setText(QApplication::translate("MainWindow", "Set", nullptr));
        yawBiasLabel->setText(QString());
        pitchBiasLabel->setText(QString());
        throttleBiasLabel->setText(QString());
        rollBiasLabel->setText(QString());
        label_7->setText(QApplication::translate("MainWindow", "Zoom", nullptr));
        manualDirectGoStraightBtn->setText(QApplication::translate("MainWindow", "Go straight", nullptr));
        manualDirectTurnLeftBtn->setText(QApplication::translate("MainWindow", "Turn left", nullptr));
        manualDirectTurnRightBtn->setText(QApplication::translate("MainWindow", "Turn right", nullptr));
        turnLabel->setText(QString());
        manualDirectCheckBox->setText(QApplication::translate("MainWindow", "Manul direct", nullptr));
        label_9->setText(QApplication::translate("MainWindow", "Nav point", nullptr));
        groupBox_3->setTitle(QApplication::translate("MainWindow", "Map", nullptr));
        label_8->setText(QApplication::translate("MainWindow", "Velocity_V:", nullptr));
        uavStatusVelocityV->setText(QApplication::translate("MainWindow", "3.3", nullptr));
        label_12->setText(QApplication::translate("MainWindow", "m/s", nullptr));
        label_6->setText(QApplication::translate("MainWindow", "Velocity_H:", nullptr));
        uavStatusVelocityH->setText(QApplication::translate("MainWindow", "3.3", nullptr));
        label_14->setText(QApplication::translate("MainWindow", "m/s", nullptr));
        label_10->setText(QApplication::translate("MainWindow", "Battery:", nullptr));
        uavStatusBattery->setText(QApplication::translate("MainWindow", "99", nullptr));
        label_13->setText(QApplication::translate("MainWindow", "%", nullptr));
        groupBox->setTitle(QApplication::translate("MainWindow", "GPS Data", nullptr));
        pushButton->setText(QApplication::translate("MainWindow", "Map_Refresh", nullptr));
        label->setText(QApplication::translate("MainWindow", "Lat", nullptr));
        GPSLatitudeLineEdit->setText(QApplication::translate("MainWindow", "29.6169", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "Lng", nullptr));
        GPSLongitudeLineEdit->setText(QApplication::translate("MainWindow", "106.4988", nullptr));
        label_4->setText(QApplication::translate("MainWindow", " GPSHeight ", nullptr));
        GPSHeight->setText(QApplication::translate("MainWindow", "000.0", nullptr));
        groupBox1->setTitle(QApplication::translate("MainWindow", "WayPoints", nullptr));
        wayPointsAddBtn->setText(QApplication::translate("MainWindow", "Add", nullptr));
        wayPointsClearBtn->setText(QApplication::translate("MainWindow", "Clear", nullptr));
        wayPointsTakeOffBtn->setText(QApplication::translate("MainWindow", "TakeOff", nullptr));
        wayPointsGoBtn->setText(QApplication::translate("MainWindow", "Go", nullptr));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Car detection", nullptr));
        carDetectBtn->setText(QApplication::translate("MainWindow", "detect_car", nullptr));
        carDetectTrackCarBtn->setText(QApplication::translate("MainWindow", "track_car", nullptr));
        carDetectWaitBtn->setText(QApplication::translate("MainWindow", "wait", nullptr));
        label_5->setText(QApplication::translate("MainWindow", "     number:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
