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
    delete game2;
    delete ui;
}

void JoinGame::setHostIP(QString address){ hostIP = address; }

void JoinGame::closeEvent(QCloseEvent *e)
{
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
        socket->connectToHost(hostIP, 5300);
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
    //game2->resetDirectionKeys();
    qDebug() << "Disconnected";

}

void JoinGame::readyRead()
{

    qDebug()<<"readyRead";
    QString data;
        data = socket->readAll();
    if(data=="PAUSED"){
        qDebug()<<"Recieved PAUSED";
            game2->pauseMenu();
    }
    else{
        QString command = data.split(";").first();
        if(command=="CONNECTED"){
            if(data.split(";").last()=="PLAYER1"){
                ui->SnakeColor->setText("Your Snake Will Be: Yellow");
                ui->SnakeColor->setStyleSheet("QLabel { color : Yellow; }");
                ui->StartButton->setDisabled(false);
                ui->P1Name->setText(nickname);
            }
            else if(data.split(";").last()=="PLAYER2"){
                ui->SnakeColor->setText("Your Snake Will Be: Red");
                ui->SnakeColor->setStyleSheet("QLabel { color : Red; }");
                ui->StartButton->setDisabled(true);
                ui->P2Name->setText(nickname);
            }// For more players, datalist this and add all players and names to indicate connect/disconnect
        }
        else if(command=="STARTED"){
            qDebug()<<"New Game";
            if(game2 == NULL){
                game2 = new Network2Player();
            }
            else{
                delete game2;
                game2 = new Network2Player();
            }
            game2->show();
            qDebug()<<data;
            if(data.split(";").last()=="PLAYER1"){
                game2->setPlayer(1);
            }
            else if(data.split(";").last()=="PLAYER2"){
                game2->setPlayer(2);
            }
            qDebug()<<"Send READY";
            QByteArray readyData;
            QString dir=QString::number(game2->getDirection());
            readyData.append("READY;");
            readyData.append(dir);
            qDebug() << socket->state();
            if(socket->state() == QAbstractSocket::ConnectedState)
            {
                socket->write(readyData); //write the data itself
                socket->waitForBytesWritten();
            }
            else
            {
                qDebug() << socket->errorString();
            }
        }
        else if(command=="UPDATE"){
            qDebug()<<"Recieved UPDATE";
            if(!game2->isPaused()){
                QStringList pieces = data.split( ";" );
                int onPart=1;
                //qDebug()<<data;
                game2->resetObjects();
                for(int i=0;i<pieces.length();i++){
                    if(pieces.value(i)=="SNAKE1"){
                        onPart=1;
                        //qDebug()<<"Part1";
                    }
                    else if(pieces.value(i)=="SNAKE2"){
                        onPart=2;
                       // qDebug()<<"Part2";
                    }
                    else if(pieces.value(i)=="APPLE"){
                        onPart=3;
                       // qDebug()<<"Part3";
                    }
                    else{
                        if((0!=pieces.value(i).toInt())&&(0!=pieces.value(i+1).toInt())){
                            game2->setPart(onPart,pieces.value(i).toInt(),pieces.value(i+1).toInt());

                         //   qDebug()<<pieces.value(i).toInt()<<" "<<pieces.value(i+1).toInt();
                        }
                        i++;
                    }
                }
                qDebug()<<"Send UPDATE";
                QString dir=QString::number(game2->getDirection());
                QByteArray updateData;
                updateData.append("UPDATE;");
                updateData.append(dir);
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
                //game2->update();
            }
            else{
                qDebug()<<"Send PAUSE";
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
        else if(command=="END"){
            qDebug()<<"End";
            if(data.split(";").last()=="NOWINNER"){
                game2->gameOver(0);
            }
            else if(data.split(";").last()=="P1WIN"){
                game2->gameOver(1);
            }
            else if(data.split(";").last()=="P2WIN"){
                game2->gameOver(2);
            }
            socket->disconnectFromHost();
        }
    }
}

void JoinGame::on_StartButton_clicked()
{
    qDebug()<<"Send STARTGAME";
    QByteArray startData;
    startData.append("STARTGAME");
    qDebug() << socket->state();
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        socket->write(startData); //write the data itself
        socket->waitForBytesWritten();
    }
    else
    {
        qDebug() << socket->errorString();
    }
}
