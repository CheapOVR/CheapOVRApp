#include "mainwindow.h"
#include "./ui_main.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QList>
#include <QStringList>
#include <QPushButton>
#include <QDir>
#include <QProcess>
#include <QRadioButton>
QStringList GetComList()
{
    QStringList CommPortList;
 
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        if (serial.open(QIODevice::ReadWrite))
        {
            CommPortList.append(serial.portName());
            serial.close();
        }
    }
 
    return CommPortList;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    reloadSerial();
    
    connect(ui->reloadBtn, &QPushButton::clicked, this, &MainWindow::reloadSerial);    
    connect(ui->uartRadio, &QRadioButton::toggled, this, [this](){
        ui->portLine->setReadOnly(true);
        ui->ipLine->setReadOnly(true);
        });
    connect(ui->bleRadio, &QRadioButton::toggled, this, [this](){
        ui->portLine->setReadOnly(true);
        ui->ipLine->setReadOnly(true);
        });
    connect(ui->udpRadio, &QRadioButton::toggled, this, [this](){
        ui->portLine->setReadOnly(false);
        ui->ipLine->setReadOnly(false);
        });
    connect(ui->wsRadio, &QRadioButton::toggled, this, [this](){
        ui->portLine->setReadOnly(false);
        ui->ipLine->setReadOnly(false);
        });
    connect(ui->trackerBtn, &QPushButton::clicked, this, [this](){
        runTrackerTest(1);
        });


    
}
void MainWindow::reloadSerial(){
    ui->serialCombo->clear();
    ui->serialCombo->addItems(GetComList());

}
void MainWindow::runTrackerTest(int mode){
    QProcess *process = new QProcess(this);
    const QString file = QDir::currentPath() + "/glvrtest";
    QStringList arguments;
    arguments << "/dev/" + ui->serialCombo->currentText();
    arguments << (ui->rotBtn->isChecked() ? "1" : "0");
    process->start(file, arguments);
}

MainWindow::~MainWindow()
{
    delete ui;
}

