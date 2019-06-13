#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug() << "here";
    TCPServer *tcp_server = new TCPServer;
}

MainWindow::~MainWindow()
{
    delete ui;
}


