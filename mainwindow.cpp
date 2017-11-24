#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFile>

bool estado_serial = false, pedir_serial, ok;
QByteArray lectura;
int i = 0, j = 0;
double contador = 0;
QFile file("out.txt");
QTextStream out(&file);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serial = new QSerialPort(this);
    connect(serial,SIGNAL(readyRead()),this,SLOT(Serial_Recibir_auto()));
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
            ui->comboBoxserie->insertItem(0,info.portName());
    }
    ui->pushButtondesconectar->setEnabled(false);

    //----PLOT-----
    ui->plot->addGraph();
    ui->plot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
    ui->plot->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->plot->xAxis->setRange(ui->spinBox_xmin->value(),ui->spinBox_xmax->value());
    ui->plot->yAxis->setRange(ui->spinBox_ymin->value(),ui->spinBox_ymax->value());
    //-------------

    //-----------base de datos
    qDebug() << "Iniciado";
    QString nombre;
    nombre.append("base_datos_test.sqlite");
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(nombre);
    if(db.open()){
        qDebug() << "base de datos iniciada";
    }
    else{
        qDebug() << "error al abrir base de datos";
    }

    crearTablaDatos();
    //mostrarDatos();
    //---------fin base de datos
    //QFile file("out.txt");
    //QTextStream out(&file);
    //out << "The magic number is: " << 49 << "\n";
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
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        else
            qDebug() << "txt creado con éxito";

    }
    else
    {
        Serial_Error();
        file.close();
    }
}

void MainWindow::Serial_Desconect()
{
    serial->close();
    estado_serial = false;
    ui->comboBoxserie->setEnabled(true);
    ui->pushButtondesconectar->setEnabled(false);
    file.close();
    qDebug() << "archivo cerrado";
}

void MainWindow::Serial_Error()
{
    QMessageBox error;
    error.setText("Verifique la conexión de la placa.");
    error.setIcon(QMessageBox::Warning);
    error.exec();
}

void MainWindow::Serial_Recibir_manual()
{
    qDebug() << "recibido";
    QByteArray dato;
    dato.append(serial->read(2).toHex().toUpper());
    qDebug() << dato;
    ui->labeldatocompleto->setText(dato);
    if(dato.contains("FE"))
        qDebug() << "acaaaaaaaaaaaaaaaaaaaaa";
    insertarDatos();
    mostrarDatos();
    out << dato.toInt(&ok,16) << "\n";
}

void MainWindow::Serial_Recibir_auto()
{
    if(serial->bytesAvailable() >= 2){
        contador++;
        qDebug() << "recibido";
        QByteArray dato;
        dato.append(serial->read(2).toHex().toUpper());
        qDebug() << dato;
        ui->labeldatocompleto->setText(dato);
        //insertarDatos();
        //mostrarDatos();
        int entero = dato.toInt(&ok,16);
        qDebug() << entero;
        //int entero2 = dato.toDouble();
        //qDebug() << entero2;
        out << entero << "\n";
        addPoint(contador, entero);
        plot();
        if(contador > ui->spinBox_xmax->value()){
            contador = 0;
            clearData();
        }
    }
    ui->progressBarrecibir->setValue(j);
    j++;
    if(j > 99)
        j = 0;
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

void MainWindow::crearTablaDatos()
{
    QString consulta;
    consulta.append("CREATE TABLE IF NOT EXISTS sensor("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "dato VARCHAR(10)"
                    ");");
    QSqlQuery crear;
    crear.prepare(consulta);
    if(crear.exec()){
        qDebug() << "tabla creada correctamente";
    }
    else{
        qDebug() << "ERROR! " << crear.lastError();
    }
}

void MainWindow::insertarDatos()
{
    QString consulta;
    consulta.append("INSERT INTO sensor("
                    "dato)"
                    "VALUES("
                    "'"+ui->labeldatocompleto->text()+"'"      //añadir el dato
                    ");"); 
    QSqlQuery insertar;
    insertar.prepare(consulta);
    if(insertar.exec()){
        qDebug() << "dato agregado con éxito";
    }
    else{
        qDebug() << "ERROR! " << insertar.lastError();
    }
}

void MainWindow::mostrarDatos()
{
    QString consulta;
    consulta.append("SELECT * FROM sensor");
    QSqlQuery mostrar;
    mostrar.prepare(consulta);
    if(mostrar.exec()){
        qDebug() << "consulta realizada con éxito";
    }
    else{
        qDebug() << "ERROR! " << mostrar.lastError();
    }

    int fila = 0;
    ui->tableWidgetdato->setRowCount(0);
    while(mostrar.next()){
        ui->tableWidgetdato->insertRow(fila);
        ui->tableWidgetdato->setItem(fila,0,new QTableWidgetItem(mostrar.value(1).toByteArray().constData()));
        fila++;
    }
}

void MainWindow::addPoint(double x, double y)
{
    qv_x.append(x);
    qv_y.append(y);
}

void MainWindow::clearData()
{
    qv_x.clear();
    qv_y.clear();
}

void MainWindow::plot()
{
    ui->plot->graph(0)->setData(qv_x, qv_y);
    ui->plot->replot();
    ui->plot->update();
}

void MainWindow::on_pushButton_clicked()
{
    clearData();
    plot();
    contador = 0;
}

void MainWindow::on_pushButton_ejes_clicked()
{
    ui->plot->xAxis->setRange(ui->spinBox_xmin->value(),ui->spinBox_xmax->value());
    ui->plot->yAxis->setRange(ui->spinBox_ymin->value(),ui->spinBox_ymax->value());
}
