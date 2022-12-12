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

    void connectTargetX();
    void connectTargetY();
    void connectTargetZ();

    void connectSegLength1();
    void connectSegLength2();

    Realtime *realtime;
    QSlider *theta1Slider;
    QSlider *theta2Slider;
    QSlider *theta3Slider;
    QDoubleSpinBox *theta1Box;
    QDoubleSpinBox *theta2Box;
    QDoubleSpinBox *theta3Box;

    QSlider *targetXSlider;
    QSlider *targetYSlider;
    QSlider *targetZSlider;
    QDoubleSpinBox *targetXBox;
    QDoubleSpinBox *targetYBox;
    QDoubleSpinBox *targetZBox;

    QSlider *segLength1Slider;
    QSlider *segLength2Slider;
    QDoubleSpinBox *segLength1Box;
    QDoubleSpinBox *segLength2Box;

private slots:
    void onValChangeTheta1Slider(int newValue);
    void onValChangeTheta2Slider(int newValue);
    void onValChangeTheta3Slider(int newValue);
    void onValChangeTheta1Box(double newValue);
    void onValChangeTheta2Box(double newValue);
    void onValChangeTheta3Box(double newValue);

    void onValChangeTargetXSlider(int newValue);
    void onValChangeTargetYSlider(int newValue);
    void onValChangeTargetZSlider(int newValue);
    void onValChangeTargetXBox(double newValue);
    void onValChangeTargetYBox(double newValue);
    void onValChangeTargetZBox(double newValue);

    void onValChangeSegLength1Slider(int newValue);
    void onValChangeSegLength2Slider(int newValue);
    void onValChangeSegLength1Box(double newValue);
    void onValChangeSegLength2Box(double newValue);
};
