#include "hostgame.h"
#include "ui_hostgame.h"

HostGame::HostGame(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HostGame)
{
    ui->setupUi(this);
    p2connect=false;
    p2connect=false;
    gameStarted=false;
    paused=false;

    server = new QTcpServer(this);
    // update to use this->ipAddress for localhost?
    server->listen(QHostAddress::Any, 5300);
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
        QByteArray sendData;
           sendData.append("STARTED;");
           qDebug() << socketp1->state();
           if(socketp1->state() == QAbstractSocket::ConnectedState)
           {
               sendData.append("PLAYER1");
               socketp1->write(sendData); //write the data itself
               socketp1->waitForBytesWritten();
               gameStarted=true;
           }
           else
           {
               qDebug() << socketp1->errorString();
           }
           qDebug() << socketp2->state();
           if(socketp2->state() == QAbstractSocket::ConnectedState)
           {
               sendData.append("PLAYER2");
               socketp2->write(sendData); //write the data itself
               socketp2->waitForBytesWritten();
               gameStarted=true;
           }
           else
           {
               qDebug() << socketp2->errorString();
           }
           timer->start();
    }
}

void HostGame::newConnection()
{
    while (server->hasPendingConnections()){
        qDebug()<<"Has pending connections";
        if(!p1connect){
            socketp1 = server->nextPendingConnection();
            connect(socketp1, SIGNAL(readyRead()),this, SLOT(p1readyRead()));
            connect(socketp1, SIGNAL(disconnected()),this, SLOT(p1Disconnected()));
        }
        else if(!p2connect){
            socketp2 = server->nextPendingConnection();
            connect(socketp2, SIGNAL(readyRead()),this, SLOT(p1readyRead()));
            connect(socketp2, SIGNAL(disconnected()),this, SLOT(p2Disconnected()));
        }
    }
}

void HostGame::p1Disconnected()
{
    p1connect=false;
    ui->Player1_Name->setText("No Player 1 Connected");
    timer->stop();
    this->resetVars();
    qDebug() << "Disconnected";
}

void HostGame::p2Disconnected()
{
    p2connect=false;
    ui->Player2_Name->setText("No Player 2 Connected");
    timer->stop();
    this->resetVars();
    qDebug() << "Disconnected";
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
           if(socketp1->state() == QAbstractSocket::ConnectedState)
           {
               //qDebug()<<"Matrix 0 0 "<<matrix[0][0]<<"sent data"<<sendData;
               socketp1->write(sendUpdateData); //write the data itself
               socketp1->waitForBytesWritten();
           }
           else
           {
               qDebug() <<"update send fail"<< socketp1->errorString();
           }
           if(socketp2->state() == QAbstractSocket::ConnectedState)
           {
               //qDebug()<<"Matrix 0 0 "<<matrix[0][0]<<"sent data"<<sendData;
               socketp2->write(sendUpdateData); //write the data itself
               socketp2->waitForBytesWritten();
           }
           else
           {
               qDebug() <<"update send fail"<< socketp2->errorString();
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
           qDebug() << socketp1->state();
           if(socketp1->state() == QAbstractSocket::ConnectedState)
           {
               socketp1->write(sendData); //write the data itself
               socketp1->waitForBytesWritten();
           }
           else
           {
               qDebug() << socketp2->errorString();
           }
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
    }
    else if(player1lost){
        //Stops all objects from moving in the background
        timer->stop();
        QByteArray sendData;
           sendData.append("END;P2WIN");
           qDebug() << socketp1->state();
           if(socketp1->state() == QAbstractSocket::ConnectedState)
           {
               socketp1->write(sendData); //write the data itself
               socketp1->waitForBytesWritten();
           }
           else
           {
               qDebug() << socketp2->errorString();
           }
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
    }
    else if(player2lost){
        //Stops all objects from moving in the background
        timer->stop();
        QByteArray sendData;
           sendData.append("END;P1WIN");
           qDebug() << socketp1->state();
           if(socketp1->state() == QAbstractSocket::ConnectedState)
           {
               socketp1->write(sendData); //write the data itself
               socketp1->waitForBytesWritten();
           }
           else
           {
               qDebug() << socketp2->errorString();
           }
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


