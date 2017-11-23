#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QMessageBox>
#include <iostream>
#include <QDebug>
#include <QSerialPortInfo>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <stdio.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void crearTablaDatos();
    void insertarDatos();
    void mostrarDatos();

private slots:
    //------serial-----
    void Serial_Conf();
    void Serial_Conect();
    void Serial_Desconect();
    void Serial_Error();
    void Serial_Recibir_manual();
    void Serial_Recibir_auto();
    //-----base de datos-----
    void on_comboBoxserie_activated(const QString &arg1);
    void on_pushButtondesconectar_clicked();
    void on_pushButtonenviar_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QSqlDatabase db;
};

#endif // MAINWINDOW_H
