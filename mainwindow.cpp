#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

bool estado_serial = false, pedir_serial, ok;
QByteArray lectura;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serial = new QSerialPort(this);
    //Serial_Conf();
    connect(serial,SIGNAL(readyRead()),this,SLOT(Serial_Recibir()));
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
            ui->comboBoxserie->insertItem(0,info.portName());
    }
    ui->pushButtondesconectar->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Serial_Conf()
{
    serial->setPortName(ui->comboBoxserie->currentText());
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
}

void MainWindow::Serial_Conect()
{
    if((serial->open(QIODevice::ReadWrite)))
    {
        estado_serial = true;
        ui->comboBoxserie->setEnabled(false);
        ui->pushButtondesconectar->setEnabled(true);
    }
    else
        Serial_Error();
}

void MainWindow::Serial_Desconect()
{
    serial->close();
    estado_serial = false;
    ui->comboBoxserie->setEnabled(true);
    ui->pushButtondesconectar->setEnabled(false);
}

void MainWindow::Serial_Error()
{
    QMessageBox error;
    error.setText("Verifique la conexión de la placa.");
    error.setIcon(QMessageBox::Warning);
    error.exec();
}

void MainWindow::Serial_Recibir()
{

}

void MainWindow::on_comboBoxserie_activated(const QString &arg1)
{
    Serial_Conf();
    Serial_Conect();
    qDebug() << "conectado";
}

void MainWindow::on_pushButtondesconectar_clicked()
{
    Serial_Desconect();
}

void MainWindow::on_pushButtonenviar_clicked()
{
    QByteArray pedido;
    pedido.append((char)ui->lineEditregistro->text().toInt(&ok,16));
    serial->write(pedido);
    pedido.clear();
    pedido.append((char)ui->lineEditdato->text().toInt(&ok,16));
    serial->write(pedido);
    qDebug() << "enviado";
    ui->labelregistro->setText(ui->lineEditregistro->text());
    ui->labeldato->setText(ui->lineEditdato->text());
}
