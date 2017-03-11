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

    //Initialize Pause Menu
    msg = new QMessageBox();
    pbox = new QMessageBox();
    pbox->setText("PAUSED");
    pbox->addButton(QString("Continue") , QMessageBox::AcceptRole);
    pbox->addButton(QString("Quit") , QMessageBox::RejectRole);
    paused = false;

    //initialize image files
    snakeImage1 = new QPixmap("Images/snakeSegment.png");
    snakeImage2 = new QPixmap("Images/redsnake.png");
    wallImage = new QPixmap("Images/wall.png");
    appleImage = new QPixmap("Images/apple.png");

    direction1=2;
    newDirection1=false;
    direction2=1;
    newDirection2=false;

    //initialize map borders and matrix
    //Note that the matrix assumes a 640x480 window
    //It takes 10 pixel squares and interprets them as one 'unit'
    for(int i=0;i<48;i++){
        for(int j=0;j<64;j++){
            if(0==j||0==i||63==j||47==i){
                matrix[i][j]=3;
                wall = new RenderObject(this);
                wall->setXCoord(j);
                wall->setYCoord(i);
                wall->setImage(wallImage);
                walls.push_back(wall);
            }
            else matrix[i][j]=0;
        }
    }

    //initialize snake
    player1 = new RenderObject(this);
    segments1.push_back(player1);

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

    switch(evt->key()){
    case Qt::Key_Escape:
    case Qt::Key_P:
        if(!paused){
            paused=true;
        }
        break;
    }

    if(false==newDirection1){
        qDebug()<<"Key 1 Event?";
        switch(evt->key())
            {//Sets the snake based on the arrow keys, and pauses based on P and Esc
            case Qt::Key_Right:
                if(direction1!=1){
                    newDirection1=true;
                    direction1=2;
                }
                break;
            case Qt::Key_Down:
                if(direction1!=0){
                    newDirection1=true;
                    direction1=3;
                }
                break;
            case Qt::Key_Up:
                if(direction1!=3){
                    newDirection1=true;
                    direction1=0;
                }
                break;
            case Qt::Key_Left:
                if(direction1!=2){
                    newDirection1=true;
                    direction1=1;
                }
                break;
            default:
                break;
        }
    }
    if(false==newDirection2){
        qDebug()<<"Key 2 Event";
        switch(evt->key())
            {//Sets the snake based on the arrow keys, and pauses based on P and Esc
            case Qt::Key_D:
                if(direction2!=1){
                    newDirection2=true;
                    direction2=2;
                }
                break;
                case Qt::Key_S:
                if(direction2!=0){
                    newDirection2=true;
                    direction2=3;
                }
                break;
            case Qt::Key_W:
                if(direction2!=3){
                    newDirection2=true;
                    direction2=0;
                }
                break;
            case Qt::Key_A:
                if(direction2!=2){
                    newDirection2=true;
                    direction2=1;
                }
                break;
            default:
                break;
        }
    }
}

//void Network2Player::moveSnake()
//{
//    //Checks if the snake has hit an apple
//    if(2==matrix[(*(segments1.at(0))).getYCoord()][(*(segments1.at(0))).getXCoord()]){
//        //Flag causes a new apple to appear next tick
//        appleEatenBy1=true;
//        //Adds a segment to the snake
//        RenderObject *newseg = new RenderObject(this);
//        newseg->setXCoord(backX1);
//        newseg->setYCoord(backY1);
//        newseg->setImage(snakeImage1);
//        segments1.push_back(newseg);
//        //Increases the player score
//        score1++;
//    }
//}

void Network2Player::pauseMenu()
{//Pause Menu
   pbox->show();
   //If QUIT is clicked, it closes the game window
   if(pbox->exec() == QMessageBox::RejectRole){
       this->close();
   }//Quit
   else {
       paused=false;
   }//Continue
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

int Network2Player::getDirection1()
{
    return this->direction1;
}

int Network2Player::getDirection2()
{
    return this->direction2;
}

void Network2Player::resetDirectionKeys()
{
    this->newDirection1=false;
    this->newDirection2=false;
}

void Network2Player::setMatrix(int newMatrix[48][64])
{
    segments1.clear();
    segments2.clear();
    for(int i=0;i<48;i++){
        for(int j=0;j<64;j++){
            matrix[i][j]=newMatrix[i][j];
            if(matrix[i][j]==3){
                //its a wall - no need to update
            }
            else if(matrix[i][j]==1){
                player1->setXCoord(i);
                player1->setYCoord(j);
                segments1.push_back(player1);
            }
            else if(matrix[i][j]==4){
                player2->setXCoord(i);
                player2->setYCoord(j);
                segments2.push_back(player2);
            }
            else if(matrix[i][j]==2){
                apple->setXCoord(i);
                apple->setYCoord(j);
            }
            else if(matrix[i][j]==0){
                //nothing to see here
            }
            else{
                qDebug()<<"Matrix value out of range";
            }
        }
    }
}

bool Network2Player::isPaused()
{
    return this->paused;
}
