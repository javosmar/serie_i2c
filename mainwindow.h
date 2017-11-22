#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QMessageBox>
#include <iostream>
#include <QDebug>
#include <QSerialPortInfo>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void Serial_Conf();
    void Serial_Conect();
    void Serial_Desconect();
    void Serial_Error();
    void Serial_Recibir();

    void on_comboBoxserie_activated(const QString &arg1);
    void on_pushButtondesconectar_clicked();
    void on_pushButtonenviar_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
};

#endif // MAINWINDOW_H
