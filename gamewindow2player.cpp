#include "gamewindow2player.h"
#include <QKeyEvent>
#include <QDebug>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <QDir>
#include <QCoreApplication>
using namespace std;

GameWindow2Player::GameWindow2Player(QWidget *parent) :
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

    //tick timer for movements
    timer = new QTimer();
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateField()));
    timer->start();

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
    for(int i=4;i>1;i--){
        player1 = new RenderObject(this);
        player1->setXCoord(i);
        player1->setYCoord(1);
        player1->setImage(snakeImage1);
        segments1.push_back(player1);
    }

    for(int i=60;i<63;i++){
        player2 = new RenderObject(this);
        player2->setXCoord(i);
        player2->setYCoord(46);
        player2->setImage(snakeImage2);
        segments2.push_back(player2);
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

void GameWindow2Player::paintEvent(QPaintEvent *e)
{//Paint Event Draws The Images On The Screen
    if(timer->isActive()){//Only if the game is moving
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
}

void GameWindow2Player::keyPressEvent(QKeyEvent *evt)
{//Detects what key the user has pressed
    qDebug()<<"Key Event";

    switch(evt->key()){
    case Qt::Key_Escape:
    case Qt::Key_P:
        if(!paused){
            pauseMenu();
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

void GameWindow2Player::moveSnake()
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
        qDebug()<<"P1 Collision"<<matrix[(*(segments1.at(0))).getYCoord()][(*(segments1.at(0))).getXCoord()];
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
        qDebug()<<"P2 Collision"<<matrix[(*(segments2.at(0))).getYCoord()][(*(segments2.at(0))).getXCoord()];
    }

    //Sets the new front segment location to indicate a snake is there
    matrix[(*(segments2.at(0))).getYCoord()][(*(segments2.at(0))).getXCoord()]=1;

    if(player1lost&&player2lost){
        //Stops all objects from moving in the background
        timer->stop();
        //Displays Game Over Message
        QMessageBox mbox;
        mbox.setText("Game Over. Both Players Lost!");
        mbox.exec();
        //Checks if a new HighScore has been reached, and sets it
        //scoreSet->setHighScores(difficulty,score);
        //Closes game window
        this->close();
    }
    else if(player1lost){
        //Stops all objects from moving in the background
        timer->stop();
        //Displays Game Over Message
        QMessageBox mbox;
        mbox.setText("Game Over. Player 2 Wins!");
        mbox.exec();
        //Checks if a new HighScore has been reached, and sets it
        //scoreSet->setHighScores(difficulty,score);
        //Closes game window
        this->close();
    }
    else if(player2lost){
        //Stops all objects from moving in the background
        timer->stop();
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

void GameWindow2Player::moveApple()
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

void GameWindow2Player::setDifficulty(int difficulty)
{
    //Sets the gmae difficulty (Basically the speed of the snake)
    this->difficulty=difficulty;
    int timeout;
    if(1==difficulty){
        timeout=100;
    }
    else if(2==difficulty){
        timeout=66;
    }
    else{
        timeout=33;
    }
    timer->setInterval(timeout);
}

void GameWindow2Player::sendHighScoreObject(HighScores *scoreSet)
{
    //This initializes the high scores object
    //Not sure exactly why, but something about creating a new one overlayed
    //the high score text onto the game window
    //So we send one from the main window instead
    this->scoreSet=scoreSet;
}

void GameWindow2Player::pauseMenu()
{//Pause Menu
   paused=true;
   pbox->show();
   //If QUIT is clicked, it closes the game window
   if(pbox->exec() == QMessageBox::RejectRole){
       timer->stop();
       this->close();
   }//Quit
   else {
       paused=false;
   }//Continue
}

void GameWindow2Player::updateField()
{
    //Check that the game isn't paused
    if(!paused){
        //Renders the new snake location first
        //Otherwise it looks like it takes an extra tick to eat apples and collide with walls
        this->moveSnake();
        //Create a new apple if it has been eaten
        if(appleEatenBy1||appleEatenBy2){
            this->moveApple();
        }
        //Redraws the entire screen at once
        this->update();
        newDirection1=false;
        newDirection2=false;
    }
}
