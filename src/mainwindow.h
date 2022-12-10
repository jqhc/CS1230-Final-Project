#pragma once

#include <QMainWindow>
#include <QCheckBox>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include "realtime.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    void initialize();
    void finish();

private:
    void connectUIElements();
    void connectTheta1();
    void connectTheta2();
    void connectTheta3();
    void connectPerPixelFilter();
    void connectKernelBasedFilter();
    void connectExtraCredit();

    Realtime *realtime;
    QCheckBox *filter1;
    QCheckBox *filter2;
    QSlider *theta1Slider;
    QSlider *theta2Slider;
    QSlider *theta3Slider;
    QDoubleSpinBox *theta1Box;
    QDoubleSpinBox *theta2Box;
    QDoubleSpinBox *theta3Box;

    // Extra Credit:
    QCheckBox *ec1;
    QCheckBox *ec2;
    QCheckBox *ec3;
    QCheckBox *ec4;

private slots:
    void onPerPixelFilter();
    void onKernelBasedFilter();
    void onValChangeTheta1Slider(int newValue);
    void onValChangeTheta2Slider(int newValue);
    void onValChangeTheta3Slider(int newValue);
    void onValChangeTheta1Box(double newValue);
    void onValChangeTheta2Box(double newValue);
    void onValChangeTheta3Box(double newValue);

    // Extra Credit:
    void onExtraCredit1();
    void onExtraCredit2();
    void onExtraCredit3();
    void onExtraCredit4();
};
