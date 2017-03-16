#include "hostgame.h"
#include "ui_hostgame.h"

HostGame::HostGame(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HostGame)
{
    ui->setupUi(this);
    gameStarted=false;

    server = new QTcpServer(this);
    // update to use this->ipAddress for localhost?
    server->listen(QHostAddress::Any, 5300);
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    //tick timer for movements
    timer = new QTimer();
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateField()));
    //timer->start();

    //initialize image files
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

    //initialize apple randomly
    srand(time(0));
    apple = new RenderObject(this);
    apple->setImage(appleImage);
    this->moveApple();
    appleEaten=false;

    connectedPlayers=0;
}

HostGame::~HostGame()
{
    delete ui;
}

void HostGame::setHostIP(QString address){ hostIP = address; }

void HostGame::newConnection()
{
    while (server->hasPendingConnections()){
        if(socket.size()<8){
            qDebug()<<"Has pending connections";
            QTcpSocket *temp=server->nextPendingConnection();
            socket.push_back(temp);
            connect(temp, SIGNAL(readyRead()),this, SLOT(readyRead()));
            connect(temp, SIGNAL(disconnected()),this, SLOT(Disconnected()));
            connectedPlayers++;
            this->initSnake();
        }
    }
}


void HostGame::initSnake()
{
    //initialize snakes
    qDebug()<<"Init Snake"<<connectedPlayers;
    vector<RenderObject*>segments;

    if(connectedPlayers==1){
        for(int i=4;i>1;i--){
            RenderObject *player1 = new RenderObject(this);
            player1->setXCoord(i);
            player1->setYCoord(2);
            QPixmap *snakeImage1 = new QPixmap("Images/snakeSegment.png");
            snakeImage.push_back(snakeImage1);
            player1->setImage(snakeImage1);
            segments.push_back(player1);
            matrix[i][2]=1;
        }
    }
    else if(connectedPlayers==2){
        for(int i=59;i<62;i++){
            RenderObject *player2 = new RenderObject(this);
            player2->setXCoord(i);
            player2->setYCoord(45);
            QPixmap *snakeImage2 = new QPixmap("Images/redsnake.png");
            snakeImage.push_back(snakeImage2);
            player2->setImage(snakeImage2);
            segments.push_back(player2);
            matrix[i][45]=4;
        }
    }
    else if(connectedPlayers==3){
        for(int i=4;i>1;i--){
            RenderObject *player3 = new RenderObject(this);
            player3->setXCoord(i);
            player3->setYCoord(45);
            QPixmap *snakeImage3 = new QPixmap("Images/redsnake.png");
            snakeImage.push_back(snakeImage3);
            player3->setImage(snakeImage3);
            segments.push_back(player3);
            matrix[i][45]=1;
        }
    }
    else if(connectedPlayers==4){
        for(int i=59;i<62;i++){
            RenderObject *player4 = new RenderObject(this);
            player4->setXCoord(i);
            player4->setYCoord(2);
            QPixmap *snakeImage4 = new QPixmap("Images/redsnake.png");
            snakeImage.push_back(snakeImage4);
            player4->setImage(snakeImage4);
            segments.push_back(player4);
            matrix[i][2]=4;
        }
    }
    else if(connectedPlayers==5){
        for(int i=4;i>1;i--){
            RenderObject *player5 = new RenderObject(this);
            player5->setXCoord(i);
            player5->setYCoord(16);
            QPixmap *snakeImage5 = new QPixmap("Images/redsnake.png");
            snakeImage.push_back(snakeImage5);
            player5->setImage(snakeImage5);
            segments.push_back(player5);
            matrix[i][16]=1;
        }
    }
    else if(connectedPlayers==6){
        for(int i=59;i<62;i++){
            RenderObject *player6 = new RenderObject(this);
            player6->setXCoord(i);
            player6->setYCoord(31);
            QPixmap *snakeImage6 = new QPixmap("Images/redsnake.png");
            snakeImage.push_back(snakeImage6);
            player6->setImage(snakeImage6);
            segments.push_back(player6);
            matrix[i][31]=4;
        }
    }
    else if(connectedPlayers==7){
        for(int i=4;i>1;i--){
            RenderObject *player7 = new RenderObject(this);
            player7->setXCoord(i);
            player7->setYCoord(31);
            QPixmap *snakeImage7 = new QPixmap("Images/redsnake.png");
            snakeImage.push_back(snakeImage7);
            player7->setImage(snakeImage7);
            segments.push_back(player7);
            matrix[i][31]=1;
        }
    }
    else if(connectedPlayers==8){
        for(int i=59;i<62;i++){
            RenderObject *player8 = new RenderObject(this);
            player8->setXCoord(i);
            player8->setYCoord(16);
            QPixmap *snakeImage8 = new QPixmap("Images/redsnake.png");
            snakeImage.push_back(snakeImage8);
            player8->setImage(snakeImage8);
            segments.push_back(player8);
            matrix[i][16]=4;
        }
    }
    snakes.push_back(segments);

    connected.push_back(false);
    ready.push_back(false);
    direction.push_back(2);
    newDirection.push_back(false);
    score.push_back(0);
    playerlost.push_back(false);
}

void HostGame::Disconnected()
{
    for(int i=0;i<socket.size();i++){
        socket.at(i)->disconnectFromHost();
    }
    ui->Player1_Name->setText("No Player 1 Connected");
    ui->Player2_Name->setText("No Player 2 Connected");
    timer->stop();
    this->resetVars();
    qDebug() <<" Disconnected";
}

void HostGame::readyRead()
{
    qDebug()<<"readyRead";
    QTcpSocket* pClient = static_cast<QTcpSocket*>(QObject::sender());
    int playerNum=0;
    for(int i=0;i<socket.size();i++){
        if(pClient==socket.at(i)){
            playerNum=i;
        }
    }
    qDebug()<<"Player Number "<<playerNum;
    QString data;
    data = pClient->readAll();
    qDebug()<<data<<gameStarted;
    if(!connected.at(playerNum)){
        connected.at(playerNum)=true;
        qDebug() << "Player" << data << "Has Joined";
        QByteArray sendConnected;
           sendConnected.append("CONNECTED;");
           qDebug() << pClient->state();
           if(pClient->state() == QAbstractSocket::ConnectedState)
           {
               sendConnected.append("PLAYER");
               QString num = QString::number(playerNum);
               sendConnected.append(num);
               pClient->write(sendConnected); //write the data itself
               pClient->waitForBytesWritten();
              // gameStarted=true;
           }
           else
           {
               qDebug() <<"Connectedp1"<< pClient->errorString();
           }

           if(playerNum==0){
               ui->Player1_Name->setText(data);
           }
           else if(playerNum==1){
               ui->Player2_Name->setText(data);
           }
    }
    else if(!gameStarted){
        qDebug()<<data;
        if(data=="STARTGAME"){
            this->startGame();
        }
    }
    else if(gameStarted){
        QString command = data.split(";").first();
        if(command=="UPDATE"){
            QStringList dataPieces=data.split(";");
            QString dir=dataPieces.value(1);
            if(newDirection.at(playerNum)==false){
                direction.at(playerNum)=dir.toInt();
                newDirection.at(playerNum)=true;
            }
        }
        else if(command=="READY"){
            QStringList dataPieces=data.split(";");
            QString dir=dataPieces.value(1);
            if(newDirection.at(playerNum)==false){
                direction.at(playerNum)=dir.toInt();
                newDirection.at(playerNum)=true;
            }
            ready.at(playerNum)=true;

            bool AllReady=true;
            for(int j=0;j<snakes.size();j++){
                if(!ready.at(j)){
                    AllReady=false;
                }
            }
            if(AllReady){
                timer->start();
            }
        }
    }
}

void HostGame::updateField()
{
    //Renders the new snake location first
    //Otherwise it looks like it takes an extra tick to eat apples and collide with walls
    this->moveSnake();
    for(int i=0;i<snakes.size();i++){
        newDirection.at(i)=false;
    }
    //Create a new apple if it has been eaten
    if(appleEaten){
        this->moveApple();
    }
    //Redraws the entire screen at once
    //this->update();

        QByteArray sendUpdateData;
        sendUpdateData.append("UPDATE;APPLE;");
        QString temp=QString::number(apple->getXCoord());
        sendUpdateData.append(temp);
        sendUpdateData.append(";");
        temp=QString::number(apple->getYCoord());
        sendUpdateData.append(temp);
        sendUpdateData.append(";");

        for(int i=0;i<snakes.size();i++){
            sendUpdateData.append("SNAKE");
            QString num = QString::number(i+1);
            sendUpdateData.append(num);
            sendUpdateData.append(";");
            for(int j=0;j<snakes.at(i).size();j++){
               QString temp=QString::number((*(snakes.at(i).at(j))).getXCoord());
               sendUpdateData.append(temp);
               sendUpdateData.append(";");

               temp=QString::number((*(snakes.at(i).at(j))).getYCoord());
               sendUpdateData.append(temp);
               sendUpdateData.append(";");
            }

        }
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

void HostGame::moveSnake()
{//Moves the snake across the screen

    for(int i=0;i<snakes.size();i++){
        vector<RenderObject*> moveSnake = snakes.at(i);

        //Stores the last segment location so it knows where to add new segments
        int backX1=(*(moveSnake.at(moveSnake.size()-1))).getXCoord();
        int backY1=(*(moveSnake.at(moveSnake.size()-1))).getYCoord();

        //Sets the back segment's location to 0 so the matrix doesn't think a snake is still there
        matrix[backY1][backX1]=0;
        qDebug()<<"Back Y "<<backY1<<" Back X "<<backX1;
        //Takes the back segment and moves it to the new front location, based on
        //the direction last pressed by the player
        //The other segments remain in place, but the new back segment will move next time
        rotate(moveSnake.begin(),moveSnake.end()-1,moveSnake.end());
        if(2==direction.at(i)){
            (*(moveSnake.at(0))).setXCoord((*(moveSnake.at(1))).getXCoord()+1);
            (*(moveSnake.at(0))).setYCoord((*(moveSnake.at(1))).getYCoord());
        }
        else if(1==direction.at(i)){
            (*(moveSnake.at(0))).setXCoord((*(moveSnake.at(1))).getXCoord()-1);
            (*(moveSnake.at(0))).setYCoord((*(moveSnake.at(1))).getYCoord());
        }
        else if(0==direction.at(i)){
            (*(moveSnake.at(0))).setXCoord((*(moveSnake.at(1))).getXCoord());
            (*(moveSnake.at(0))).setYCoord((*(moveSnake.at(1))).getYCoord()-1);
        }
        else if(3==direction.at(i)){
            (*(moveSnake.at(0))).setXCoord((*(moveSnake.at(1))).getXCoord());
            (*(moveSnake.at(0))).setYCoord((*(moveSnake.at(1))).getYCoord()+1);
        }

        snakes.at(i)=moveSnake;
        //Checks if the snake has hit an apple
        if(2==matrix[(*(moveSnake.at(0))).getYCoord()][(*(moveSnake.at(0))).getXCoord()]){
            //Flag causes a new apple to appear next tick
            appleEaten=true;
            //Adds a segment to the snake
            RenderObject *newseg = new RenderObject(this);
            newseg->setXCoord(backX1);
            newseg->setYCoord(backY1);
            newseg->setImage(snakeImage.at(i));
            moveSnake.push_back(newseg);
            //Increases the player score
            score.at(i)++;
        }

        //Checks if the player has collided with a wall
        if((3==matrix[(*(moveSnake.at(0))).getYCoord()][(*(moveSnake.at(0))).getXCoord()])||(1==matrix[(*(moveSnake.at(0))).getYCoord()][(*(moveSnake.at(0))).getXCoord()])){
            //Stops all objects from moving in the background
            qDebug()<<"P1 Collision Object "<<matrix[(*(moveSnake.at(0))).getYCoord()][(*(moveSnake.at(0))).getXCoord()]<<" At X "<<(*(moveSnake.at(0))).getXCoord()<<" Y "<<(*(moveSnake.at(0))).getYCoord();
            qDebug()<<"Direction "<<direction.at(i);
            playerlost.at(i)=true;
        }

        //Sets the new front segment location to indicate a snake is there
        matrix[(*(moveSnake.at(0))).getYCoord()][(*(moveSnake.at(0))).getXCoord()]=1;
    }

    if(playerlost.at(0)&&playerlost.at(1)){
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
    else if(playerlost.at(0)){
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
    else if(playerlost.at(1)){
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
    appleEaten=false;

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

    for(int i=0;i<snakes.size();i++){
        snakes.at(i).clear();
    }
    snakes.clear();
    connected.clear();
    ready.clear();
    direction.clear();
    newDirection.clear();
    score.clear();
    playerlost.clear();
    socket.clear();
    connectedPlayers=0;
    gameStarted=false;
    appleEaten=false;

    //initialize apple randomly
    this->moveApple();
}

void HostGame::startGame()
{
    for(int i=0;i<socket.size();i++){
        QTcpSocket* pClient = socket.at(i);
        if(connected.at(0)&&connected.at(1)){
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



