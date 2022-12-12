#include "mainwindow.h"
#include "settings.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QSettings>
#include <QLabel>
#include <QGroupBox>
#include <iostream>

void MainWindow::initialize() {
    realtime = new Realtime;

    QHBoxLayout *hLayout = new QHBoxLayout; // horizontal alignment
    QVBoxLayout *vLayout = new QVBoxLayout(); // vertical alignment
    vLayout->setAlignment(Qt::AlignTop);
    hLayout->addLayout(vLayout);
    hLayout->addWidget(realtime, 1);
    this->setLayout(hLayout);

    // Create labels in sidebox
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    QLabel *angles_label = new QLabel(); // Angles label
    angles_label->setText("Joint Angles");
    angles_label->setFont(font);
    QLabel *theta1_label = new QLabel(); // Theta 1 label
    theta1_label->setText("Theta 1:");
    QLabel *theta2_label = new QLabel(); // Theta 2 label
    theta2_label->setText("Theta 2:");
    QLabel *theta3_label = new QLabel(); // Theta 3 label
    theta3_label->setText("Theta 3:");

    QLabel *target_label = new QLabel(); // Target Point label
    target_label->setText("Body Position");
    target_label->setFont(font);
    QLabel *targetX_label = new QLabel(); // x label
    targetX_label->setText("X-Value:");
    QLabel *targetY_label = new QLabel(); // y label
    targetY_label->setText("Y-Value:");
    QLabel *targetZ_label = new QLabel(); // z label
    targetZ_label->setText("Z-Value:");

    QLabel *length_label = new QLabel(); // Leg Segment length label
    length_label->setText("Leg Segment Lengths");
    length_label->setFont(font);
    QLabel *segLength1_label = new QLabel(); // leg segment 1
    segLength1_label->setText("Leg Segment 1:");
    QLabel *segLength2_label = new QLabel(); // leg segment 2
    segLength2_label->setText("Leg Segment 2:");


    // Creates the boxes containing the camera sliders and number boxes
    QGroupBox *theta1Layout = new QGroupBox(); // horizonal theta 1 alignment
    QHBoxLayout *ltheta1 = new QHBoxLayout();
    QGroupBox *theta2Layout = new QGroupBox(); // horizonal theta 2 alignment
    QHBoxLayout *ltheta2 = new QHBoxLayout();
    QGroupBox *theta3Layout = new QGroupBox(); // horizonal theta 3 alignment
    QHBoxLayout *ltheta3 = new QHBoxLayout();

    QGroupBox *targetXLayout = new QGroupBox(); // target x alignment
    QHBoxLayout *lTargetX = new QHBoxLayout();
    QGroupBox *targetYLayout = new QGroupBox(); // target y alignment
    QHBoxLayout *lTargetY = new QHBoxLayout();
    QGroupBox *targetZLayout = new QGroupBox(); // target z alignment
    QHBoxLayout *lTargetZ = new QHBoxLayout();

    QGroupBox *segLength1Layout = new QGroupBox(); // segment 1 length alignment
    QHBoxLayout *lSegLength1 = new QHBoxLayout();
    QGroupBox *segLength2Layout = new QGroupBox(); // segment 2 length alignment
    QHBoxLayout *lSegLength2 = new QHBoxLayout();

    // THETAS
    theta1Slider = new QSlider(Qt::Orientation::Horizontal); // Theta 1 slider
    theta1Slider->setTickInterval(1);
    theta1Slider->setMinimum(0);
    theta1Slider->setMaximum(628);
    theta1Slider->setValue(0);

    theta1Box = new QDoubleSpinBox();
    theta1Box->setMinimum(0.0f);
    theta1Box->setMaximum(6.28f);
    theta1Box->setSingleStep(0.01f);
    theta1Box->setValue(0);

    theta2Slider = new QSlider(Qt::Orientation::Horizontal); // Theta 2 slider
    theta2Slider->setTickInterval(1);
    theta2Slider->setMinimum(0);
    theta2Slider->setMaximum(157);
    theta2Slider->setValue(0);

    theta2Box = new QDoubleSpinBox();
    theta2Box->setMinimum(0.0f);
    theta2Box->setMaximum(1.57f);
    theta2Box->setSingleStep(0.01f);
    theta2Box->setValue(0);

    theta3Slider = new QSlider(Qt::Orientation::Horizontal); // Theta 2 slider
    theta3Slider->setTickInterval(1);
    theta3Slider->setMinimum(0);
    theta3Slider->setMaximum(628);
    theta3Slider->setValue(0);

    theta3Box = new QDoubleSpinBox();
    theta3Box->setMinimum(0.0f);
    theta3Box->setMaximum(6.28f);
    theta3Box->setSingleStep(0.01f);
    theta3Box->setValue(0);

    // TARGET POSITION
    targetXSlider = new QSlider(Qt::Orientation::Horizontal); // target x slider
    targetXSlider->setTickInterval(1);
    targetXSlider->setMinimum(-100);
    targetXSlider->setMaximum(100);
    targetXSlider->setValue(0);

    targetXBox = new QDoubleSpinBox();
    targetXBox->setMinimum(-1.0f);
    targetXBox->setMaximum(1.0f);
    targetXBox->setSingleStep(0.01f);
    targetXBox->setValue(0);

    targetYSlider = new QSlider(Qt::Orientation::Horizontal); // target y slider
    targetYSlider->setTickInterval(1);
    targetYSlider->setMinimum(-100);
    targetYSlider->setMaximum(100);
    targetYSlider->setValue(0);

    targetYBox = new QDoubleSpinBox();
    targetYBox->setMinimum(-1.0f);
    targetYBox->setMaximum(1.0f);
    targetYBox->setSingleStep(0.01f);
    targetYBox->setValue(0);

    targetZSlider = new QSlider(Qt::Orientation::Horizontal); // target z slider
    targetZSlider->setTickInterval(1);
    targetZSlider->setMinimum(-100);
    targetZSlider->setMaximum(100);
    targetZSlider->setValue(0);

    targetZBox = new QDoubleSpinBox();
    targetZBox->setMinimum(-1.0f);
    targetZBox->setMaximum(1.0f);
    targetZBox->setSingleStep(0.01f);
    targetZBox->setValue(0);

    // LEG SEGMENT LENGTHS
    segLength1Slider = new QSlider(Qt::Orientation::Horizontal); // segment length 1 slider
    segLength1Slider->setTickInterval(1);
    segLength1Slider->setMinimum(1);
    segLength1Slider->setMaximum(100);
    segLength1Slider->setValue(0);

    segLength1Box = new QDoubleSpinBox();
    segLength1Box->setMinimum(0.01f);
    segLength1Box->setMaximum(1.0f);
    segLength1Box->setSingleStep(0.01f);
    segLength1Box->setValue(0);

    segLength2Slider = new QSlider(Qt::Orientation::Horizontal); // segment length 2 slider
    segLength2Slider->setTickInterval(1);
    segLength2Slider->setMinimum(1);
    segLength2Slider->setMaximum(100);
    segLength2Slider->setValue(0);

    segLength2Box = new QDoubleSpinBox();
    segLength2Box->setMinimum(0.01f);
    segLength2Box->setMaximum(1.0f);
    segLength2Box->setSingleStep(0.01f);
    segLength2Box->setValue(0);


    // Adds the slider and number box to the parameter layouts
    // THETAS
//    ltheta1->addWidget(theta1Slider);
//    ltheta1->addWidget(theta1Box);
//    theta1Layout->setLayout(ltheta1);

//    ltheta2->addWidget(theta2Slider);
//    ltheta2->addWidget(theta2Box);
//    theta2Layout->setLayout(ltheta2);

//    ltheta3->addWidget(theta3Slider);
//    ltheta3->addWidget(theta3Box);
//    theta3Layout->setLayout(ltheta3);

//    vLayout->addWidget(angles_label);
//    vLayout->addWidget(theta1_label);
//    vLayout->addWidget(theta1Layout);
//    vLayout->addWidget(theta2_label);
//    vLayout->addWidget(theta2Layout);
//    vLayout->addWidget(theta3_label);
//    vLayout->addWidget(theta3Layout);

    // TARGET POS
    lTargetX->addWidget(targetXSlider);
    lTargetX->addWidget(targetXBox);
    targetXLayout->setLayout(lTargetX);

    lTargetY->addWidget(targetYSlider);
    lTargetY->addWidget(targetYBox);
    targetYLayout->setLayout(lTargetY);

    lTargetZ->addWidget(targetZSlider);
    lTargetZ->addWidget(targetZBox);
    targetZLayout->setLayout(lTargetZ);

    vLayout->addWidget(target_label);
    vLayout->addWidget(targetX_label);
    vLayout->addWidget(targetXLayout);
    vLayout->addWidget(targetY_label);
    vLayout->addWidget(targetYLayout);
    vLayout->addWidget(targetZ_label);
    vLayout->addWidget(targetZLayout);

    // LEG SEGMENT LENGTHS
//    lSegLength1->addWidget(segLength1Slider);
//    lSegLength1->addWidget(segLength1Box);
//    segLength1Layout->setLayout(lSegLength1);

//    lSegLength2->addWidget(segLength2Slider);
//    lSegLength2->addWidget(segLength2Box);
//    segLength2Layout->setLayout(lSegLength2);

//    vLayout->addWidget(length_label);
//    vLayout->addWidget(segLength1_label);
//    vLayout->addWidget(segLength1Layout);
//    vLayout->addWidget(segLength2_label);
//    vLayout->addWidget(segLength2Layout);

    connectUIElements();

    // Set default values
    onValChangeTheta1Box(0);
    onValChangeTheta2Box(0);
    onValChangeTheta3Box(0);

    onValChangeTargetXBox(0);
    onValChangeTargetYBox(0);
    onValChangeTargetZBox(0);

    onValChangeSegLength1Box(0.5f);
    onValChangeSegLength2Box(0.5f);
}

void MainWindow::finish() {
    realtime->finish();
    delete(realtime);
}

void MainWindow::connectUIElements() {
    connectTheta1();
    connectTheta2();
    connectTheta3();

    connectTargetX();
    connectTargetY();
    connectTargetZ();

    connectSegLength1();
    connectSegLength2();
}

void MainWindow::connectTheta1() {
    connect(theta1Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeTheta1Slider);
    connect(theta1Box, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeTheta1Box);
}

void MainWindow::connectTheta2() {
    connect(theta2Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeTheta2Slider);
    connect(theta2Box, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeTheta2Box);
}

void MainWindow::connectTheta3() {
    connect(theta3Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeTheta3Slider);
    connect(theta3Box, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeTheta3Box);
}

void MainWindow::connectTargetX() {
    connect(targetXSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeTargetXSlider);
    connect(targetXBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeTargetXBox);
}

void MainWindow::connectTargetY() {
    connect(targetYSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeTargetYSlider);
    connect(targetYBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeTargetYBox);
}

void MainWindow::connectTargetZ() {
    connect(targetZSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeTargetZSlider);
    connect(targetZBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeTargetZBox);
}

void MainWindow::connectSegLength1() {
    connect(segLength1Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeSegLength1Slider);
    connect(segLength1Box, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeSegLength1Box);
}

void MainWindow::connectSegLength2() {
    connect(segLength2Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeSegLength2Slider);
    connect(segLength2Box, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeSegLength2Box);
}

// THETAS
void MainWindow::onValChangeTheta1Slider(int newValue) {
    //nearSlider->setValue(newValue);
    theta1Box->setValue(newValue/100.f);
    settings.theta1 = theta1Box->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeTheta2Slider(int newValue) {
    //farSlider->setValue(newValue);
    theta2Box->setValue(newValue/100.f);
    settings.theta2 = theta2Box->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeTheta3Slider(int newValue) {
    //farSlider->setValue(newValue);
    theta3Box->setValue(newValue/100.f);
    settings.theta3 = theta3Box->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeTheta1Box(double newValue) {
    theta1Slider->setValue(int(newValue*100.f));
    //nearBox->setValue(newValue);
    settings.theta1 = theta1Box->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeTheta2Box(double newValue) {
    theta2Slider->setValue(int(newValue*100.f));
    //farBox->setValue(newValue);
    settings.theta2 = theta2Box->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeTheta3Box(double newValue) {
    theta3Slider->setValue(int(newValue*100.f));
    //farBox->setValue(newValue);
    settings.theta3 = theta3Box->value();
    realtime->settingsChanged();
}

// TARGET POS
void MainWindow::onValChangeTargetXSlider(int newValue) {
    //nearSlider->setValue(newValue);
    targetXBox->setValue(newValue/100.f);
    settings.targetX = targetXBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeTargetYSlider(int newValue) {
    //farSlider->setValue(newValue);
    targetYBox->setValue(newValue/100.f);
    settings.targetY = targetYBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeTargetZSlider(int newValue) {
    //farSlider->setValue(newValue);
    targetZBox->setValue(newValue/100.f);
    settings.targetZ = targetZBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeTargetXBox(double newValue) {
    targetXSlider->setValue(int(newValue*100.f));
    //nearBox->setValue(newValue);
    settings.targetX = targetXBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeTargetYBox(double newValue) {
    targetYSlider->setValue(int(newValue*100.f));
    //farBox->setValue(newValue);
    settings.targetY = targetYBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeTargetZBox(double newValue) {
    targetZSlider->setValue(int(newValue*100.f));
    //farBox->setValue(newValue);
    settings.targetZ = targetZBox->value();
    realtime->settingsChanged();
}

// LEG SEGMENT LENGTHS
void MainWindow::onValChangeSegLength1Slider(int newValue) {
    //nearSlider->setValue(newValue);
    segLength1Box->setValue(newValue/100.f);
    settings.segLength1 = segLength1Box->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeSegLength2Slider(int newValue) {
    //farSlider->setValue(newValue);
    segLength2Box->setValue(newValue/100.f);
    settings.segLength2 = segLength2Box->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeSegLength1Box(double newValue) {
    segLength1Slider->setValue(int(newValue*100.f));
    //nearBox->setValue(newValue);
    settings.segLength1 = segLength1Box->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeSegLength2Box(double newValue) {
    segLength2Slider->setValue(int(newValue*100.f));
    //farBox->setValue(newValue);
    settings.segLength2 = segLength2Box->value();
    realtime->settingsChanged();
}
