#include "hostgame.h"
#include "ui_hostgame.h"

HostGame::HostGame(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HostGame)
{
    ui->setupUi(this);
    game2=NULL;
    p2connect=false;

    server = new QTcpServer(this);
    // update to use this->ipAddress for localhost?
    server->listen(QHostAddress::Any, 5300);
    socket = new QTcpSocket(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

HostGame::~HostGame()
{
    delete ui;
    delete game2;
}

void HostGame::setHostIP(QString address){ hostIP = address; }

void HostGame::on_pushButton_clicked()
{
    if(p2connect){

        QByteArray sendData;
           sendData.append("started");
           //socket->connectToHost("192.168.1.8", 5300);
           //socket->waitForConnected(1000);
           qDebug() << socket->state();
           if(socket->state() == QAbstractSocket::ConnectedState)
           {
               socket->write(sendData); //write the data itself
               socket->waitForBytesWritten();
           }
           else
           {
               qDebug() << socket->errorString();
           }
//       if(game2 == NULL)
//           {
//               game2 = new Network2Player();
//               game2->setDifficulty(1);
//           }else{
//               delete game2;
//               game2 = new Network2Player();
//               game2->setDifficulty(1);
//           }
//           game2->show();
    }
}

void HostGame::newConnection()
{
    while (server->hasPendingConnections()){
        qDebug()<<"Has pending connections";
        socket = server->nextPendingConnection();
        connect(socket, SIGNAL(readyRead()),this, SLOT(readyRead()));
        connect(socket, SIGNAL(disconnected()),this, SLOT(Disconnected()));
    }
}

void HostGame::Disconnected()
{
    p2connect=false;
    qDebug() << "Disconnected";
}

void HostGame::readyRead()
{
    qDebug()<<"readyRead";
    QString data;
        data = socket->readAll();
        ui->Player2_Name->setText(data);
        p2connect=true;
        qDebug() << "Player" << data << "Has Joined";
}
