#include "hostgame.h"
#include "ui_hostgame.h"

HostGame::HostGame(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HostGame)
{
    ui->setupUi(this);
    p2connect=false;
    p2connect=false;
    p1ready=false;
    p2ready=false;
    gameStarted=false;
    paused=false;

    server = new QTcpServer(this);
    // update to use this->ipAddress for localhost?
    server->listen(QHostAddress::Any, 5300);
    //socket = new QTcpSocket(this);
    socketp1 = new QTcpSocket(this);
    socketp2 = new QTcpSocket(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    //tick timer for movements
    timer = new QTimer();
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateField()));
    //timer->start();

    direction1=2;
    direction2=1;
    newDirection1=false;
    newDirection2=false;


    //initialize image files
    snakeImage1 = new QPixmap("Images/snakeSegment.png");
    snakeImage2 = new QPixmap("Images/redsnake.png");
    wallImage = new QPixmap("Images/wall.png");
    appleImage = new QPixmap("Images/apple.png");

    //initialize map borders and matrix
    //Note that the matrix assumes a 640x480 window
    //It takes 10 pixel squares and interprets them as one 'unit'
    for(int i=0;i<48;i++){
        for(int j=0;j<64;j++){
            if(0==j||0==i||63==j||47==i){
                matrix[i][j]=3;
            }
            else matrix[i][j]=0;
        }
    }

    //initialize snake
    for(int i=5;i>2;i--){
        player1 = new RenderObject(this);
        player1->setXCoord(i);
        player1->setYCoord(2);
        player1->setImage(snakeImage1);
        segments1.push_back(player1);
        matrix[i][2]=1;
    }

    for(int i=59;i<62;i++){
        player2 = new RenderObject(this);
        player2->setXCoord(i);
        player2->setYCoord(45);
        player2->setImage(snakeImage2);
        segments2.push_back(player2);
        matrix[i][45]=4;
    }

    //initialize apple randomly
    srand(time(0));
    apple = new RenderObject(this);
    apple->setImage(appleImage);
    this->moveApple();

    score1=0;
    score2=0;

    player1lost=false;
    player2lost=false;
}

HostGame::~HostGame()
{
    delete ui;
}

void HostGame::setHostIP(QString address){ hostIP = address; }

void HostGame::on_pushButton_clicked()
{
    if(p1connect&&p2connect){
        QByteArray sendStarted1;
           sendStarted1.append("STARTED;");
           qDebug() << socketp1->state();
           if(socketp1->state() == QAbstractSocket::ConnectedState)
           {
               sendStarted1.append("PLAYER1");
               socketp1->write(sendStarted1); //write the data itself
               socketp1->waitForBytesWritten();
               gameStarted=true;
           }
           else
           {
               qDebug() << socketp1->errorString();
           }
           QByteArray sendStarted2;
              sendStarted2.append("STARTED;");
           qDebug() << socketp2->state();
           if(socketp2->state() == QAbstractSocket::ConnectedState)
           {
               sendStarted2.append("PLAYER2");
               socketp2->write(sendStarted2); //write the data itself
               socketp2->waitForBytesWritten();
               gameStarted=true;
           }
           else
           {
               qDebug() << socketp2->errorString();
           }
    }
}

void HostGame::newConnection()
{
    while (server->hasPendingConnections()){
        qDebug()<<"Has pending connections";
        QTcpSocket *temp=server->nextPendingConnection();
        socket.push_back(temp);
        connect(temp, SIGNAL(readyRead()),this, SLOT(readyRead()));
        connect(temp, SIGNAL(disconnected()),this, SLOT(Disconnected()));
//        if(!p1connect){
//            socketp1 = server->nextPendingConnection();
//            connect(socketp1, SIGNAL(readyRead()),this, SLOT(p1readyRead()));
//            connect(socketp1, SIGNAL(disconnected()),this, SLOT(p1Disconnected()));
//        }
//        else if(!p2connect){
//            socketp2 = server->nextPendingConnection();
//            connect(socketp2, SIGNAL(readyRead()),this, SLOT(p2readyRead()));
//            connect(socketp2, SIGNAL(disconnected()),this, SLOT(p2Disconnected()));
//        }
    }
}

void HostGame::p1Disconnected()
{
    p1connect=false;
    ui->Player1_Name->setText("No Player 1 Connected");
    timer->stop();
    this->resetVars();
    qDebug() << "p1Disconnected";
}

void HostGame::p2Disconnected()
{
    p2connect=false;
    ui->Player2_Name->setText("No Player 2 Connected");
    timer->stop();
    this->resetVars();
    qDebug() << "p2Disconnected";
}

void HostGame::p1readyRead()
{
    qDebug()<<"p1readyRead";
    QString data;
    data = socketp1->readAll();
    if(!p1connect){
        ui->Player1_Name->setText(data);
        p1connect=true;
        qDebug() << "Player" << data << "Has Joined";

        QByteArray sendConnected1;
           sendConnected1.append("CONNECTED;");
           qDebug() << socketp1->state();
           if(socketp1->state() == QAbstractSocket::ConnectedState)
           {
               sendConnected1.append("PLAYER1");
               socketp1->write(sendConnected1); //write the data itself
               socketp1->waitForBytesWritten();
               gameStarted=true;
           }
           else
           {
               qDebug() << socketp1->errorString();
           }
    }
    else if(gameStarted){
        QString command = data.split(";").first();
        if(command=="UPDATE"){
            QStringList dataPieces=data.split(";");
            QString dir1=dataPieces.value(1);
            if(newDirection1==false){
                direction1=dir1.toInt();
                newDirection1=true;
            }
        }
        else if(command=="READY"){
            QStringList dataPieces=data.split(";");
            QString dir1=dataPieces.value(1);
            if(newDirection1==false){
                direction1=dir1.toInt();
                newDirection1=true;
            }
            p1ready=true;
            if(p2ready&&p1ready){
                timer->start();
            }
        }
        else if(data=="PAUSE"){
            paused=true;
            QByteArray sendData;
               sendData.append("PAUSED");
               qDebug() << socketp1->state();
               if(socketp1->state() == QAbstractSocket::ConnectedState)
               {
                   socketp1->write(sendData); //write the data itself
                   socketp1->waitForBytesWritten();
               }
               else
               {
                   qDebug() << socketp1->errorString();
               }
               timer->stop();
        }
        else if(data=="STARTGAME"){
            this->startGame();
        }
    }
}

void HostGame::p2readyRead()
{
    qDebug()<<"p2readyRead";
    QString data;
    data = socketp2->readAll();
    if(!p2connect){
        ui->Player2_Name->setText(data);
        p2connect=true;
        qDebug() << "Player" << data << "Has Joined";
        QByteArray sendConnected2;
           sendConnected2.append("CONNECTED;");
           qDebug() << socketp2->state();
           if(socketp2->state() == QAbstractSocket::ConnectedState)
           {
               sendConnected2.append("PLAYER2");
               socketp2->write(sendConnected2); //write the data itself
               socketp2->waitForBytesWritten();
               gameStarted=true;
           }
           else
           {
               qDebug() << socketp1->errorString();
           }
    }
    else if(gameStarted){
        QString command = data.split(";").first();
        if(command=="UPDATE"){
            QStringList dataPieces=data.split(";");
            QString dir2=dataPieces.value(1);
            if(newDirection2==false){
                direction2=dir2.toInt();
                newDirection2=true;
            }
        }
        else if(command=="READY"){
            QStringList dataPieces=data.split(";");
            QString dir2=dataPieces.value(1);
            if(newDirection2==false){
                direction2=dir2.toInt();
                newDirection2=true;
            }
            p2ready=true;
            if(p2ready&&p1ready){
                timer->start();
            }
        }
        else if(data=="PAUSE"){
            paused=true;
            QByteArray sendData;
               sendData.append("PAUSED");
               qDebug() << socketp2->state();
               if(socketp2->state() == QAbstractSocket::ConnectedState)
               {
                   socketp2->write(sendData); //write the data itself
                   socketp2->waitForBytesWritten();
               }
               else
               {
                   qDebug() << socketp2->errorString();
               }
               timer->stop();
        }
    }
}

void HostGame::updateField()
{
    //Check that the game isn't paused
    if(!paused){
        //Renders the new snake location first
        //Otherwise it looks like it takes an extra tick to eat apples and collide with walls
        this->moveSnake();
        newDirection1=false;
        newDirection2=false;
        //Create a new apple if it has been eaten
        if(appleEatenBy1||appleEatenBy2){
            this->moveApple();
        }
        //Redraws the entire screen at once
        //this->update();

        QByteArray sendUpdateData;
           sendUpdateData.append("UPDATE;SNAKE1;");
           for(int i=0;i<segments1.size();i++){
              QString temp=QString::number((*(segments1.at(i))).getXCoord());
              sendUpdateData.append(temp);
              sendUpdateData.append(";");

              temp=QString::number((*(segments1.at(i))).getYCoord());
              sendUpdateData.append(temp);
              sendUpdateData.append(";");
           }
           sendUpdateData.append("SNAKE2;");
           for(int i=0;i<segments2.size();i++){
              QString temp=QString::number((*(segments2.at(i))).getXCoord());
              sendUpdateData.append(temp);
              sendUpdateData.append(";");

              temp=QString::number((*(segments2.at(i))).getYCoord());
              sendUpdateData.append(temp);
              sendUpdateData.append(";");
           }

           sendUpdateData.append("APPLE;");
           QString temp=QString::number(apple->getXCoord());
           sendUpdateData.append(temp);
           sendUpdateData.append(";");
           temp=QString::number(apple->getYCoord());
           sendUpdateData.append(temp);
           sendUpdateData.append(";");
           //qDebug() << socket->state();
           for(int i=0;i<socket.size();i++){
               QTcpSocket* pClient = socket.at(i);
               if(pClient->state() == QAbstractSocket::ConnectedState)
               {
                   //qDebug()<<"Matrix 0 0 "<<matrix[0][0]<<"sent data"<<sendData;
                   pClient->write(sendUpdateData); //write the data itself
                   pClient->waitForBytesWritten();
               }
               else
               {
                   qDebug() <<"update send fail"<< pClient->errorString();
               }
           }
    }
}

void HostGame::Disconnected()
{
    QTcpSocket* pClient = static_cast<QTcpSocket*>(QObject::sender());
    int playerNum=0;
    for(int i=0;i<socket.size();i++){
        if(pClient==socket.at(i)){
            playerNum=i+1;
        }
    }
    if(1==playerNum){
        p1connect=false;
        ui->Player1_Name->setText("No Player 1 Connected");
    }
    else if(2==playerNum){
        p2connect=false;
        ui->Player2_Name->setText("No Player 2 Connected");
    }
    timer->stop();
    this->resetVars();
    qDebug() << playerNum<<" Disconnected";
}

void HostGame::readyRead()
{
    qDebug()<<"readyRead";
    QTcpSocket* pClient = static_cast<QTcpSocket*>(QObject::sender());
    int playerNum=0;
    for(int i=0;i<socket.size();i++){
        if(pClient==socket.at(i)){
            playerNum=i+1;
        }
    }
    qDebug()<<"Player Number "<<playerNum;
    QString data;
    data = pClient->readAll();
    if(playerNum==1){
        if(!p1connect){
            ui->Player1_Name->setText(data);
            p1connect=true;
            qDebug() << "Player" << data << "Has Joined";

            QByteArray sendConnected1;
               sendConnected1.append("CONNECTED;");
               qDebug() << pClient->state();
               if(pClient->state() == QAbstractSocket::ConnectedState)
               {
                   sendConnected1.append("PLAYER1");
                   pClient->write(sendConnected1); //write the data itself
                   pClient->waitForBytesWritten();
                   gameStarted=true;
               }
               else
               {
                   qDebug() <<"Connectedp1"<< pClient->errorString();
               }
        }
        else if(gameStarted){
            QString command = data.split(";").first();
            if(command=="UPDATE"){
                QStringList dataPieces=data.split(";");
                QString dir1=dataPieces.value(1);
                if(newDirection1==false){
                    direction1=dir1.toInt();
                    newDirection1=true;
                }
            }
            else if(command=="READY"){
                QStringList dataPieces=data.split(";");
                QString dir1=dataPieces.value(1);
                if(newDirection1==false){
                    direction1=dir1.toInt();
                    newDirection1=true;
                }
                p1ready=true;
                if(p2ready&&p1ready){
                    timer->start();
                }
            }
            else if(data=="PAUSE"){
                paused=true;
                QByteArray sendData;
                   sendData.append("PAUSED");
                   qDebug() << pClient->state();
                   if(pClient->state() == QAbstractSocket::ConnectedState)
                   {
                       pClient->write(sendData); //write the data itself
                       pClient->waitForBytesWritten();
                   }
                   else
                   {
                       qDebug() << "Pausep1"<<pClient->errorString();
                   }
                   timer->stop();
            }
            else if(data=="STARTGAME"){
                this->startGame();
            }
        }
    }
    else if(playerNum==2){
            if(!p2connect){
            ui->Player2_Name->setText(data);
            p2connect=true;
            qDebug() << "Player" << data << "Has Joined";
            QByteArray sendConnected2;
               sendConnected2.append("CONNECTED;");
               qDebug() << pClient->state();
               if(pClient->state() == QAbstractSocket::ConnectedState)
               {
                   sendConnected2.append("PLAYER2");
                   pClient->write(sendConnected2); //write the data itself
                   pClient->waitForBytesWritten();
                   gameStarted=true;
               }
               else
               {
                   qDebug() <<"Connectedp2"<< pClient->errorString();
               }
        }
        else if(gameStarted){
            QString command = data.split(";").first();
            if(command=="UPDATE"){
                QStringList dataPieces=data.split(";");
                QString dir2=dataPieces.value(1);
                if(newDirection2==false){
                    direction2=dir2.toInt();
                    newDirection2=true;
                }
            }
            else if(command=="READY"){
                QStringList dataPieces=data.split(";");
                QString dir2=dataPieces.value(1);
                if(newDirection2==false){
                    direction2=dir2.toInt();
                    newDirection2=true;
                }
                p2ready=true;
                if(p2ready&&p1ready){
                    timer->start();
                }
            }
            else if(data=="PAUSE"){
                paused=true;
                QByteArray sendData;
                   sendData.append("PAUSED");
                   qDebug() << pClient->state();
                   if(pClient->state() == QAbstractSocket::ConnectedState)
                   {
                       pClient->write(sendData); //write the data itself
                       pClient->waitForBytesWritten();
                   }
                   else
                   {
                       qDebug() << "Pausep2"<< pClient->errorString();
                   }
                   timer->stop();
            }
        }
    }
}

void HostGame::moveSnake()
{//Moves the snake across the screen

    //Snake 1

    //Stores the last segment location so it knows where to add new segments
    int backX1=(*(segments1.at(segments1.size()-1))).getXCoord();
    int backY1=(*(segments1.at(segments1.size()-1))).getYCoord();

    //Sets the back segment's location to 0 so the matrix doesn't think a snake is still there
    matrix[backY1][backX1]=0;

    //Takes the back segment and moves it to the new front location, based on
    //the direction last pressed by the player
    //The other segments remain in place, but the new back segment will move next time
    rotate(segments1.begin(),segments1.end()-1,segments1.end());
    if(2==direction1){
        (*(segments1.at(0))).setXCoord((*(segments1.at(1))).getXCoord()+1);
        (*(segments1.at(0))).setYCoord((*(segments1.at(1))).getYCoord());
    }
    else if(1==direction1){
        (*(segments1.at(0))).setXCoord((*(segments1.at(1))).getXCoord()-1);
        (*(segments1.at(0))).setYCoord((*(segments1.at(1))).getYCoord());
    }
    else if(0==direction1){
        (*(segments1.at(0))).setXCoord((*(segments1.at(1))).getXCoord());
        (*(segments1.at(0))).setYCoord((*(segments1.at(1))).getYCoord()-1);
    }
    else{
        (*(segments1.at(0))).setXCoord((*(segments1.at(1))).getXCoord());
        (*(segments1.at(0))).setYCoord((*(segments1.at(1))).getYCoord()+1);
    }

    //Checks if the snake has hit an apple
    if(2==matrix[(*(segments1.at(0))).getYCoord()][(*(segments1.at(0))).getXCoord()]){
        //Flag causes a new apple to appear next tick
        appleEatenBy1=true;
        //Adds a segment to the snake
        RenderObject *newseg = new RenderObject(this);
        newseg->setXCoord(backX1);
        newseg->setYCoord(backY1);
        newseg->setImage(snakeImage1);
        segments1.push_back(newseg);
        //Increases the player score
        score1++;
    }

    //Checks if the player has collided with a wall
    if((3==matrix[(*(segments1.at(0))).getYCoord()][(*(segments1.at(0))).getXCoord()])||(1==matrix[(*(segments1.at(0))).getYCoord()][(*(segments1.at(0))).getXCoord()])){
        //Stops all objects from moving in the background
        qDebug()<<"P1 Collision Object "<<matrix[(*(segments1.at(0))).getYCoord()][(*(segments1.at(0))).getXCoord()]<<" At X "<<(*(segments1.at(0))).getXCoord()<<" Y "<<(*(segments1.at(0))).getYCoord();
        qDebug()<<"Direction "<<direction1;
        player1lost=true;
    }

    //Sets the new front segment location to indicate a snake is there
    matrix[(*(segments1.at(0))).getYCoord()][(*(segments1.at(0))).getXCoord()]=1;


    //Snake 2

    //Stores the last segment location so it knows where to add new segments1
    int backX2=(*(segments2.at(segments2.size()-1))).getXCoord();
    int backY2=(*(segments2.at(segments2.size()-1))).getYCoord();

    //Sets the back segment's location to 0 so the matrix doesn't think a snake is still there
    matrix[backY2][backX2]=0;
    //Takes the back segment and moves it to the new front location, based on
    //the direction last pressed by the player
    //The other segments2 remain in place, but the new back segment will move next time
    rotate(segments2.begin(),segments2.end()-1,segments2.end());
    if(2==direction2){
        (*(segments2.at(0))).setXCoord((*(segments2.at(1))).getXCoord()+1);
        (*(segments2.at(0))).setYCoord((*(segments2.at(1))).getYCoord());
    }
    else if(1==direction2){
        (*(segments2.at(0))).setXCoord((*(segments2.at(1))).getXCoord()-1);
        (*(segments2.at(0))).setYCoord((*(segments2.at(1))).getYCoord());
    }
    else if(0==direction2){
        (*(segments2.at(0))).setXCoord((*(segments2.at(1))).getXCoord());
        (*(segments2.at(0))).setYCoord((*(segments2.at(1))).getYCoord()-1);
    }
    else{
        (*(segments2.at(0))).setXCoord((*(segments2.at(1))).getXCoord());
        (*(segments2.at(0))).setYCoord((*(segments2.at(1))).getYCoord()+1);
    }

    //Checks if the snake has hit an apple
    if(2==matrix[(*(segments2.at(0))).getYCoord()][(*(segments2.at(0))).getXCoord()]){
        //Flag causes a new apple to appear next tick
        appleEatenBy2=true;
        //Adds a segment to the snake
        RenderObject *newseg = new RenderObject(this);
        newseg->setXCoord(backX2);
        newseg->setYCoord(backY2);
        newseg->setImage(snakeImage2);
        segments2.push_back(newseg);
        //Increases the player score
        score2++;
    }

    //Checks if the player has collided with a wall
    if((3==matrix[(*(segments2.at(0))).getYCoord()][(*(segments2.at(0))).getXCoord()])||(1==matrix[(*(segments2.at(0))).getYCoord()][(*(segments2.at(0))).getXCoord()])){
        player2lost=true;
        qDebug()<<"P2 Collision Object "<<matrix[(*(segments2.at(0))).getYCoord()][(*(segments2.at(0))).getXCoord()]<<" At X "<<(*(segments2.at(0))).getXCoord()<<" Y "<<(*(segments2.at(0))).getYCoord();
    }

    //Sets the new front segment location to indicate a snake is there
    matrix[(*(segments2.at(0))).getYCoord()][(*(segments2.at(0))).getXCoord()]=4;

    if(player1lost&&player2lost){
        //Stops all objects from moving in the background
        timer->stop();
        QByteArray sendData;
           sendData.append("END;NOWINNER");
           for(int i=0;i<socket.size();i++){
               QTcpSocket* pClient = socket.at(i);
               qDebug() << pClient->state();
               if(pClient->state() == QAbstractSocket::ConnectedState)
               {
                   pClient->write(sendData); //write the data itself
                   pClient->waitForBytesWritten();
               }
               else
               {
                   qDebug() <<"Both Lost "<< pClient->errorString()<<i;
               }
           }
    }
    else if(player1lost){
        //Stops all objects from moving in the background
        timer->stop();
        QByteArray sendData;
           sendData.append("END;P2WIN");
           for(int i=0;i<socket.size();i++){
               QTcpSocket* pClient = socket.at(i);
               qDebug() << pClient->state();
               if(pClient->state() == QAbstractSocket::ConnectedState)
               {
                   pClient->write(sendData); //write the data itself
                   pClient->waitForBytesWritten();
               }
               else
               {
                   qDebug() <<"P1Lost"<< pClient->errorString()<<i;
               }
           }
    }
    else if(player2lost){
        //Stops all objects from moving in the background
        timer->stop();
        QByteArray sendData;
           sendData.append("END;P1WIN");
           for(int i=0;i<socket.size();i++){
               QTcpSocket* pClient = socket.at(i);
               qDebug() << pClient->state();
               if(pClient->state() == QAbstractSocket::ConnectedState)
               {
                   pClient->write(sendData); //write the data itself
                   pClient->waitForBytesWritten();
               }
               else
               {
                   qDebug() <<"P2Lost" <<pClient->errorString()<<i;
               }
           }
    }
}

void HostGame::moveApple()
{
    //Removes the old apple location
    matrix[apple->getYCoord()][apple->getXCoord()]=0;

    int x=0;
    int y=0;

    //Checks until it finds a location that isn't a wall or snake (empty)
    while(0!=matrix[y][x]){
        x = rand()%64;
        y = rand()%48;
    }

    //Sets the new apple
    apple->setXCoord(x);
    apple->setYCoord(y);
    //Tells the matrix an apple now exists there
    matrix[y][x]=2;
    //Indicates a new apple has appeared
    appleEatenBy2=false;
    appleEatenBy1=false;

}

void HostGame::resetVars()
{
    for(int i=0;i<48;i++){
        for(int j=0;j<64;j++){
            if(0==j||0==i||63==j||47==i){
                matrix[i][j]=3;
            }
            else matrix[i][j]=0;
        }
    }

    //initialize snake
    segments1.clear();
    segments2.clear();
    for(int i=4;i>1;i--){
        player1 = new RenderObject(this);
        player1->setXCoord(i);
        player1->setYCoord(1);
        player1->setImage(snakeImage1);
        segments1.push_back(player1);
        matrix[i][1]=1;
    }

    for(int i=60;i<63;i++){
        player2 = new RenderObject(this);
        player2->setXCoord(i);
        player2->setYCoord(46);
        player2->setImage(snakeImage2);
        segments2.push_back(player2);
        matrix[i][46]=4;
    }

    //initialize apple randomly
    this->moveApple();

    score1=0;
    score2=0;

    player1lost=false;
    player2lost=false;
    direction1=2;
    direction2=1;
}

void HostGame::startGame()
{
    for(int i=0;i<socket.size();i++){
        QTcpSocket* pClient = socket.at(i);
        if(p1connect&&p2connect){
            QByteArray sendStarted;
               sendStarted.append("STARTED;");
               qDebug() << pClient->state();
               if(pClient->state() == QAbstractSocket::ConnectedState)
               {
                   sendStarted.append("PLAYER");
                   QString num=QString::number(i+1);
                   sendStarted.append(num);
                   pClient->write(sendStarted); //write the data itself
                   pClient->waitForBytesWritten();
                   gameStarted=true;
               }
               else
               {
                   qDebug() <<"Start Game"<< pClient->errorString();
               }

        }
    }
}


