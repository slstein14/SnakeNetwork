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
    snakeImage1 = new QPixmap("Images/snakeSegment.png");
    snakeImage2 = new QPixmap("Images/RedSnake.png");
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
    if(i==1){
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
        apple->setXCoord(x);
        apple->setYCoord(y);
    }
}

void Network2Player::resetObjects()
{
    segments1.clear();
    segments2.clear();
}

void Network2Player::setPlayer(int player)
{
    if(player==1){
        direction=2;
        qDebug()<<"Player1";
    }
    else if(player==2){
        direction=1;
        qDebug()<<"Player2";
    }
    else{
        qDebug()<<"Player out of range";
    }
}

void Network2Player::updateField()
{
    this->update();
}
