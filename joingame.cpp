#include "joingame.h"
#include "ui_joingame.h"
#include <QCloseEvent>

JoinGame::JoinGame(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JoinGame)
{
    ui->setupUi(this);
    game2=NULL;

    server = new QTcpServer(this);
    server->listen(QHostAddress::Any, 5300);
    socket = new QTcpSocket(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

JoinGame::~JoinGame()
{
    qDebug()<<"Destruct";
    delete ui;
}

void JoinGame::setHostIP(QString address){ hostIP = address; }

void JoinGame::closeEvent(QCloseEvent *e)
{
    socket->disconnectFromHost();
    socket->disconnectFromHost();
    e->accept();
}

void JoinGame::on_pushButton_clicked()
{
    hostIP=ui->HostIP->toPlainText();
    nickname=ui->PlayerName->toPlainText();
    qDebug() << "Player nickname: " << nickname;
    qDebug() << "IP the player is trying to connect to: " << hostIP;

    QByteArray sendData;
        sendData.append(nickname);
        //socket = new QTcpSocket(this);
        socket->connectToHost("192.168.1.8", 5300);
        socket->waitForConnected(1000);
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
            qDebug()<<"Has pending connections";
            connect(socket, SIGNAL(readyRead()),this, SLOT(readyRead()));
            connect(socket, SIGNAL(disconnected()),this, SLOT(Disconnected()));
            qDebug()<<"Has wra pending connections";

}

void JoinGame::newConnection()
{
    while (socket->isOpen()){
        qDebug()<<"Has pending connections";
        connect(socket, SIGNAL(readyRead()),this, SLOT(readyRead()));
        connect(socket, SIGNAL(disconnected()),this, SLOT(Disconnected()));
    }
}

void JoinGame::Disconnected()
{
    qDebug() << "Disconnected";

}

void JoinGame::readyRead()
{

    qDebug()<<"readyRead";
    QString data;
        data = socket->readAll();
        qDebug() << "Start Game";

   qDebug() << "Host IP: " << hostIP;
    if(data.length()!=0){
       if(game2 == NULL)
           {
               game2 = new Network2Player();
               game2->setDifficulty(1);
           }else{
               delete game2;
               game2 = new Network2Player();
               game2->setDifficulty(1);
           }
           game2->show();
    }
}
