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

    ui->StartButton->setDisabled(true);
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
    ui->StartButton->setDisabled(true);
    ui->SnakeColor->setText("Your Snake Will Be: ");
    ui->SnakeColor->setStyleSheet("QLabel { color : Black; }");
    ui->P1Name->setText("No Player 1");
    ui->P2Name->setText("No Player 2");
    ui->P3Name->setText("");
    ui->P4Name->setText("");
    ui->P5Name->setText("");
    ui->P6Name->setText("");
    ui->P7Name->setText("");
    ui->P8Name->setText("");
    qDebug() << "Disconnected";

}

void JoinGame::readyRead()
{
    qDebug()<<"readyRead";
    QString data;
    data = socket->readAll();
    qDebug()<<data;
    QString command = data.split(";").first();
    if(command=="CONNECTED"){
        if(data.split(";").last()=="PLAYER1"){
            ui->SnakeColor->setText("Your Snake Will Be: Yellow");
            ui->SnakeColor->setStyleSheet("QLabel { color : Yellow; }");
            ui->StartButton->setDisabled(false);
        }
        else if(data.split(";").last()=="PLAYER2"){
            ui->SnakeColor->setText("Your Snake Will Be: Red");
            ui->SnakeColor->setStyleSheet("QLabel { color : Red; }");
        }
        else if(data.split(";").last()=="PLAYER3"){
            ui->SnakeColor->setText("Your Snake Will Be: Blue");
            ui->SnakeColor->setStyleSheet("QLabel { color : Blue; }");
        }
        else if(data.split(";").last()=="PLAYER4"){
            ui->SnakeColor->setText("Your Snake Will Be: Orange");
            ui->SnakeColor->setStyleSheet("QLabel { color : Orange; }");
        }
        else if(data.split(";").last()=="PLAYER5"){
            ui->SnakeColor->setText("Your Snake Will Be: Black");
            ui->SnakeColor->setStyleSheet("QLabel { color : Black; }");
        }
        else if(data.split(";").last()=="PLAYER6"){
            ui->SnakeColor->setText("Your Snake Will Be: Pink");
            ui->SnakeColor->setStyleSheet("QLabel { color : Pink; }");
        }
        else if(data.split(";").last()=="PLAYER7"){
            ui->SnakeColor->setText("Your Snake Will Be: Brown");
            ui->SnakeColor->setStyleSheet("QLabel { color : Brown; }");
        }
        else if(data.split(";").last()=="PLAYER8"){
            ui->SnakeColor->setText("Your Snake Will Be: Light Blue");
            ui->SnakeColor->setStyleSheet("QLabel { color : Cyan; }");
        }
    }
    else if(command=="PLAYERLIST"){
        QStringList pieces = data.split( ";" );
        for(int i=0;i<pieces.length();i++){
            if(i==1){
                ui->P1Name->setText(pieces.at(i));
            }
            else if(i==2){
                ui->P2Name->setText(pieces.at(i));
            }
            else if(i==3){
                ui->P3Name->setText(pieces.at(i));
            }
            else if(i==4){
                ui->P4Name->setText(pieces.at(i));
            }
            else if(i==5){
                ui->P5Name->setText(pieces.at(i));
            }
            else if(i==6){
                ui->P6Name->setText(pieces.at(i));
            }
            else if(i==7){
                ui->P7Name->setText(pieces.at(i));
            }
            else if(i==8){
                ui->P8Name->setText(pieces.at(i));
            }
        }
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
        int pnum=data.split(";").last().toInt();
        game2->setPlayer(pnum);
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
        QStringList pieces = data.split( ";" );
        int onPart=0;
        game2->resetObjects();
        for(int i=0;i<pieces.length();i++){
            if(pieces.value(i)=="APPLE"){
                onPart=0;
            }
            else if(pieces.value(i)=="SNAKE1"){
                onPart=1;
            }
            else if(pieces.value(i)=="SNAKE2"){
                onPart=2;
            }
            else if(pieces.value(i)=="SNAKE3"){
                onPart=3;
            }
            else if(pieces.value(i)=="SNAKE4"){
                onPart=4;
            }
            else if(pieces.value(i)=="SNAKE5"){
                onPart=5;
            }
            else if(pieces.value(i)=="SNAKE6"){
                onPart=6;
            }
            else if(pieces.value(i)=="SNAKE7"){
                onPart=7;
            }
            else if(pieces.value(i)=="SNAKE8"){
                onPart=8;
            }
            else{
                if((0!=pieces.value(i).toInt())&&(0!=pieces.value(i+1).toInt())){
                    game2->setPart(onPart,pieces.value(i).toInt(),pieces.value(i+1).toInt());
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
    }
    else if(command=="END"){
        qDebug()<<"End";
        if(data.split(";").last()=="NOWINNER"){
            game2->gameOver(0);
        }
        else{
            int winner=data.split(";").last().toInt();
            game2->gameOver(winner);
        }
        socket->disconnectFromHost();
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
