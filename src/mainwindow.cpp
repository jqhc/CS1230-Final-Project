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
    QLabel *filters_label = new QLabel(); // Filters label
    filters_label->setText("Filters");
    filters_label->setFont(font);
    QLabel *ec_label = new QLabel(); // Extra Credit label
    ec_label->setText("Extra Credit");
    ec_label->setFont(font);
    QLabel *param1_label = new QLabel(); // Parameter 1 label
    param1_label->setText("Parameter 1:");
    QLabel *param2_label = new QLabel(); // Parameter 2 label
    param2_label->setText("Parameter 2:");
    QLabel *theta1_label = new QLabel(); // Theta 1 label
    theta1_label->setText("Theta 1:");
    QLabel *theta2_label = new QLabel(); // Theta 2 label
    theta2_label->setText("Theta 2:");
    QLabel *theta3_label = new QLabel(); // Theta 3 label
    theta3_label->setText("Theta 3:");



    // Create checkbox for per-pixel filter
    filter1 = new QCheckBox();
    filter1->setText(QStringLiteral("Per-Pixel Filter"));
    filter1->setChecked(false);

    // Create checkbox for kernel-based filter
    filter2 = new QCheckBox();
    filter2->setText(QStringLiteral("Kernel-Based Filter"));
    filter2->setChecked(false);

    // Creates the boxes containing the camera sliders and number boxes
    QGroupBox *theta1Layout = new QGroupBox(); // horizonal theta 1 alignment
    QHBoxLayout *ltheta1 = new QHBoxLayout();
    QGroupBox *theta2Layout = new QGroupBox(); // horizonal theta 2 alignment
    QHBoxLayout *ltheta2 = new QHBoxLayout();
    QGroupBox *theta3Layout = new QGroupBox(); // horizonal theta 3 alignment
    QHBoxLayout *ltheta3 = new QHBoxLayout();

    // Create slider controls to control near/far planes
    theta1Slider = new QSlider(Qt::Orientation::Horizontal); // Theta 1 slider
    theta1Slider->setTickInterval(1);
    theta1Slider->setMinimum(0);
    theta1Slider->setMaximum(628);
    theta1Slider->setValue(0);

    theta1Box = new QDoubleSpinBox();
    theta1Box->setMinimum(0);
    theta1Box->setMaximum(6.28f);
    theta1Box->setSingleStep(0.01f);
    theta1Box->setValue(0);

    theta2Slider = new QSlider(Qt::Orientation::Horizontal); // Theta 2 slider
    theta2Slider->setTickInterval(1);
    theta2Slider->setMinimum(0);
    theta2Slider->setMaximum(157);
    theta2Slider->setValue(0);

    theta2Box = new QDoubleSpinBox();
    theta2Box->setMinimum(0);
    theta2Box->setMaximum(1.57f);
    theta2Box->setSingleStep(0.01f);
    theta2Box->setValue(0);

    theta3Slider = new QSlider(Qt::Orientation::Horizontal); // Theta 2 slider
    theta3Slider->setTickInterval(1);
    theta3Slider->setMinimum(0);
    theta3Slider->setMaximum(628);
    theta3Slider->setValue(0);

    theta3Box = new QDoubleSpinBox();
    theta3Box->setMinimum(0);
    theta3Box->setMaximum(6.28f);
    theta3Box->setSingleStep(0.01f);
    theta3Box->setValue(0);

    // Adds the slider and number box to the parameter layouts
    ltheta1->addWidget(theta1Slider);
    ltheta1->addWidget(theta1Box);
    theta1Layout->setLayout(ltheta1);

    ltheta2->addWidget(theta2Slider);
    ltheta2->addWidget(theta2Box);
    theta2Layout->setLayout(ltheta2);

    ltheta3->addWidget(theta3Slider);
    ltheta3->addWidget(theta3Box);
    theta3Layout->setLayout(ltheta3);

    // Extra Credit:
    ec1 = new QCheckBox();
    ec1->setText(QStringLiteral("Extra Credit 1"));
    ec1->setChecked(false);

    ec2 = new QCheckBox();
    ec2->setText(QStringLiteral("Extra Credit 2"));
    ec2->setChecked(false);

    ec3 = new QCheckBox();
    ec3->setText(QStringLiteral("Extra Credit 3"));
    ec3->setChecked(false);

    ec4 = new QCheckBox();
    ec4->setText(QStringLiteral("Extra Credit 4"));
    ec4->setChecked(false);

    vLayout->addWidget(angles_label);
    vLayout->addWidget(theta1_label);
    vLayout->addWidget(theta1Layout);
    vLayout->addWidget(theta2_label);
    vLayout->addWidget(theta2Layout);
    vLayout->addWidget(theta3_label);
    vLayout->addWidget(theta3Layout);
    vLayout->addWidget(filters_label);
    vLayout->addWidget(filter1);
    vLayout->addWidget(filter2);
    // Extra Credit:
    vLayout->addWidget(ec_label);
    vLayout->addWidget(ec1);
    vLayout->addWidget(ec2);
    vLayout->addWidget(ec3);
    vLayout->addWidget(ec4);

    connectUIElements();

    // Set default values for near and far planes
    onValChangeTheta1Box(0);
    onValChangeTheta2Box(0);
    onValChangeTheta3Box(0);
}

void MainWindow::finish() {
    realtime->finish();
    delete(realtime);
}

void MainWindow::connectUIElements() {
    connectPerPixelFilter();
    connectKernelBasedFilter();
    connectTheta1();
    connectTheta2();
    connectTheta3();
    connectExtraCredit();
}

void MainWindow::connectPerPixelFilter() {
    connect(filter1, &QCheckBox::clicked, this, &MainWindow::onPerPixelFilter);
}

void MainWindow::connectKernelBasedFilter() {
    connect(filter2, &QCheckBox::clicked, this, &MainWindow::onKernelBasedFilter);
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

void MainWindow::connectExtraCredit() {
    connect(ec1, &QCheckBox::clicked, this, &MainWindow::onExtraCredit1);
    connect(ec2, &QCheckBox::clicked, this, &MainWindow::onExtraCredit2);
    connect(ec3, &QCheckBox::clicked, this, &MainWindow::onExtraCredit3);
    connect(ec4, &QCheckBox::clicked, this, &MainWindow::onExtraCredit4);
}

void MainWindow::onPerPixelFilter() {
    settings.perPixelFilter = !settings.perPixelFilter;
    realtime->settingsChanged();
}

void MainWindow::onKernelBasedFilter() {
    settings.kernelBasedFilter = !settings.kernelBasedFilter;
    realtime->settingsChanged();
}

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

// Extra Credit:

void MainWindow::onExtraCredit1() {
    settings.extraCredit1 = !settings.extraCredit1;
    realtime->settingsChanged();
}

void MainWindow::onExtraCredit2() {
    settings.extraCredit2 = !settings.extraCredit2;
    realtime->settingsChanged();
}

void MainWindow::onExtraCredit3() {
    settings.extraCredit3 = !settings.extraCredit3;
    realtime->settingsChanged();
}

void MainWindow::onExtraCredit4() {
    settings.extraCredit4 = !settings.extraCredit4;
    realtime->settingsChanged();
}
