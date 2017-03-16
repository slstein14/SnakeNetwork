#include "network2player.h"
#include <QKeyEvent>
#include <QDebug>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <QDir>
#include <QCoreApplication>
using namespace std;

Network2Player::Network2Player(QWidget *parent) :
    QWidget(parent)
{
#ifdef __APPLE__//Apple File Path Fix
     QDir bin(QCoreApplication::applicationDirPath());
     bin.cdUp();
     bin.cdUp();
     bin.cdUp();
     QDir::setCurrent(bin.absolutePath());
 #endif

    //Initialize window
    QPixmap bkgnd("images/background.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
//        qDebug()<<this->size();//640x480
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);

    timer = new QTimer();
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateField()));
    timer->start();

    //initialize image files
    snakeImage1 = new QPixmap("Images/SnakePlayer1.png");
    snakeImage2 = new QPixmap("Images/SnakePlayer2.png");
    snakeImage3 = new QPixmap("Images/SnakePlayer3.png");
    snakeImage4 = new QPixmap("Images/SnakePlayer4.png");
    snakeImage5 = new QPixmap("Images/SnakePlayer5.png");
    snakeImage6 = new QPixmap("Images/SnakePlayer6.png");
    snakeImage7 = new QPixmap("Images/SnakePlayer7.png");
    snakeImage8 = new QPixmap("Images/SnakePlayer8.png");
    wallImage = new QPixmap("Images/wall.png");
    appleImage = new QPixmap("Images/apple.png");

    direction=2;

    for(int i=0;i<48;i++){
        for(int j=0;j<64;j++){
            if(0==j||0==i||63==j||47==i){
                wall = new RenderObject(this);
                wall->setXCoord(j);
                wall->setYCoord(i);
                wall->setImage(wallImage);
                walls.push_back(wall);
            }
        }
    }

    //initialize snake
    player1 = new RenderObject(this);
    player1->setImage(snakeImage1);

    player2 = new RenderObject(this);
    player2->setImage(snakeImage2);

    player3 = new RenderObject(this);
    player3->setImage(snakeImage3);

    player4 = new RenderObject(this);
    player4->setImage(snakeImage4);

    player5 = new RenderObject(this);
    player5->setImage(snakeImage5);

    player6 = new RenderObject(this);
    player6->setImage(snakeImage6);

    player7 = new RenderObject(this);
    player7->setImage(snakeImage7);

    player8 = new RenderObject(this);
    player8->setImage(snakeImage8);

    //initialize apple
    apple = new RenderObject(this);
    apple->setImage(appleImage);
}

void Network2Player::paintEvent(QPaintEvent *e)
{//Paint Event Draws The Images On The Screen
    QPainter painter(this);//Painter object indicates what window to render in
    apple->drawObject(painter);//Renders the Apple object
    for(int i=0;i<segments1.size();i++){//Renders each segment of the snake 1
        (*(segments1.at(i))).drawObject(painter);
    }
    for(int i=0;i<segments2.size();i++){//Renders each segment of the snake 2
        (*(segments2.at(i))).drawObject(painter);
    }
    for(int i=0;i<segments3.size();i++){//Renders each segment of the snake 2
        (*(segments3.at(i))).drawObject(painter);
    }
    for(int i=0;i<segments4.size();i++){//Renders each segment of the snake 2
        (*(segments4.at(i))).drawObject(painter);
    }
    for(int i=0;i<segments5.size();i++){//Renders each segment of the snake 2
        (*(segments5.at(i))).drawObject(painter);
    }
    for(int i=0;i<segments6.size();i++){//Renders each segment of the snake 2
        (*(segments6.at(i))).drawObject(painter);
    }
    for(int i=0;i<segments7.size();i++){//Renders each segment of the snake 2
        (*(segments7.at(i))).drawObject(painter);
    }
    for(int i=0;i<segments8.size();i++){//Renders each segment of the snake 2
        (*(segments8.at(i))).drawObject(painter);
    }

    for(int i=0;i<walls.size();i++){//Renders each wall segment
        (*(walls.at(i))).drawObject(painter);
    }
}

void Network2Player::keyPressEvent(QKeyEvent *evt)
{//Detects what key the user has pressed
    qDebug()<<"Key Event";
    switch(evt->key())
        {//Sets the snake based on the arrow keys
        case Qt::Key_Right:
            if(direction!=1){
                direction=2;
            }
            break;
        case Qt::Key_Down:
            if(direction!=0){
                direction=3;
            }
            break;
        case Qt::Key_Up:
            if(direction!=3){
                direction=0;
            }
            break;
        case Qt::Key_Left:
            if(direction!=2){
                direction=1;
            }
            break;
        default:
            break;
    }
}

void Network2Player::gameOver(int winner)
{    if(winner==0){
        //Displays Game Over Message
        QMessageBox mbox;
        mbox.setText("Game Over. Both Players Lost!");
        mbox.exec();
        //Checks if a new HighScore has been reached, and sets it
        //scoreSet->setHighScores(difficulty,score);
        //Closes game window
        this->close();
    }
    else if(winner==2){
        //Displays Game Over Message
        QMessageBox mbox;
        mbox.setText("Game Over. Player 2 Wins!");
        mbox.exec();
        //Checks if a new HighScore has been reached, and sets it
        //scoreSet->setHighScores(difficulty,score);
        //Closes game window
        this->close();
    }
    else if(winner==1){
        //Displays Game Over Message
        QMessageBox mbox;
        mbox.setText("Game Over. Player 1 Wins!");
        mbox.exec();
        //Checks if a new HighScore has been reached, and sets it
        //scoreSet->setHighScores(difficulty,score);
        //Closes game window
        this->close();
    }

}

int Network2Player::getDirection()
{
    return this->direction;
}

void Network2Player::setPart(int i, int x, int y)
{
    if(i==0){
        apple->setXCoord(x);
        apple->setYCoord(y);
    }
    else if(i==1){
        RenderObject *player1=new RenderObject(this);
        player1->setXCoord(x);
        player1->setYCoord(y);
        player1->setImage(snakeImage1);
        segments1.push_back(player1);
    }
    else if(i==2){
        RenderObject *player2=new RenderObject(this);
        player2->setXCoord(x);
        player2->setYCoord(y);
        player2->setImage(snakeImage2);
        segments2.push_back(player2);
    }
    else if(i==3){
        RenderObject *player3=new RenderObject(this);
        player3->setXCoord(x);
        player3->setYCoord(y);
        player3->setImage(snakeImage3);
        segments3.push_back(player3);
    }
    else if(i==4){
        RenderObject *player4=new RenderObject(this);
        player4->setXCoord(x);
        player4->setYCoord(y);
        player4->setImage(snakeImage4);
        segments4.push_back(player4);
    }
    else if(i==5){
        RenderObject *player5=new RenderObject(this);
        player5->setXCoord(x);
        player5->setYCoord(y);
        player5->setImage(snakeImage5);
        segments5.push_back(player5);
    }
    else if(i==6){
        RenderObject *player6=new RenderObject(this);
        player6->setXCoord(x);
        player6->setYCoord(y);
        player6->setImage(snakeImage6);
        segments6.push_back(player6);
    }
    else if(i==7){
        RenderObject *player7=new RenderObject(this);
        player7->setXCoord(x);
        player7->setYCoord(y);
        player7->setImage(snakeImage7);
        segments7.push_back(player7);
    }
    else if(i==8){
        RenderObject *player8=new RenderObject(this);
        player8->setXCoord(x);
        player8->setYCoord(y);
        player8->setImage(snakeImage8);
        segments8.push_back(player8);
    }
}

void Network2Player::resetObjects()
{
    segments1.clear();
    segments2.clear();
    segments3.clear();
    segments4.clear();
    segments5.clear();
    segments6.clear();
    segments7.clear();
    segments8.clear();
}

void Network2Player::setPlayer(int player)
{
    if((player%2)==1){
        direction=2;
        qDebug()<<"Player "<<player;
    }
    else if((player%2)==0){
        direction=1;
        qDebug()<<"Player "<<player;
    }
    else{
        qDebug()<<"Player out of range";
    }
}

void Network2Player::updateField()
{
    this->update();
}
