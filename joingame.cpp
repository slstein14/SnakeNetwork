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

    for(int i=0;i<48;i++){
        for(int j=0;j<64;j++){
           matrix[i][j]=0;
        }
    }
}

JoinGame::~JoinGame()
{
    delete game2;
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
            //qDebug()<<"Has pending connections";
            connect(socket, SIGNAL(readyRead()),this, SLOT(readyRead()));
            connect(socket, SIGNAL(disconnected()),this, SLOT(Disconnected()));
            //qDebug()<<"Has wra pending connections";

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
    if(data=="STARTED"){
        if(game2 == NULL)
            {
                game2 = new Network2Player();
            }else{
                delete game2;
                game2 = new Network2Player();
            }
            game2->show();
    }
    else if(data=="PAUSED"){
            game2->pauseMenu();
    }
    else{
        QString command = data.split(";").first();
        if(command=="UPDATE"){
            //matrix=details
            int k=0;
            for(int i=0;i<48;i++){
                for(int j=0;j<64;j++){
                    k++;
                    QStringList pieces = data.split( ";" );
                    QString neededWord = pieces.value(k);
                    matrix[i][j]=neededWord.toInt();
                    game2->setMatrix(matrix);
            }
                if(!game2->isPaused()){
                    int dir1=game2->getDirection1();
                    int dir2=game2->getDirection2();
                    QByteArray updateData;
                    updateData.append("UPDATE;");
                    updateData.append(dir1);
                    updateData.append(";");
                    updateData.append(dir2);
                    qDebug() << socket->state();
                    if(socket->state() == QAbstractSocket::ConnectedState)
                    {
                        socket->write(updateData); //write the data itself
                        socket->waitForBytesWritten();
                    }
                    else
                    {
                        qDebug() << socket->errorString();
                    }
                }
                else{
                    QByteArray pauseData;
                    pauseData.append("PAUSE");
                    qDebug() << socket->state();
                    if(socket->state() == QAbstractSocket::ConnectedState)
                    {
                        socket->write(pauseData); //write the data itself
                        socket->waitForBytesWritten();
                    }
                    else
                    {
                        qDebug() << socket->errorString();
                    }
                }
            }
        }
        else if(command=="END"){
            if(data.split(";").last()=="NOWINNER"){
                game2->gameOver(0);
            }
            else if(data.split(";").last()=="P1WIN"){
                game2->gameOver(1);
            }
            else if(data.split(";").last()=="P2WIN"){
                game2->gameOver(2);
            }
        }
    }
}
