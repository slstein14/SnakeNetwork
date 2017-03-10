#include "gamewindow.h"
#include <QKeyEvent>
#include <QDebug>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <QDir>
#include <QCoreApplication>
using namespace std;

GameWindow::GameWindow(QWidget *parent) :
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
    snakeImage = new QPixmap("Images/snakeSegment.png");
    wallImage = new QPixmap("Images/wall.png");
    appleImage = new QPixmap("Images/apple.png");

    direction=2;
    bool newDirection=false;

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
        player = new RenderObject(this);
        player->setXCoord(i);
        player->setYCoord(1);
        player->setImage(snakeImage);
        segments.push_back(player);
    }

    //initialize apple randomly
    srand(time(0));
    apple = new RenderObject(this);
    apple->setImage(appleImage);
    this->moveApple();

    score=0;
}

void GameWindow::paintEvent(QPaintEvent *e)
{//Paint Event Draws The Images On The Screen
    if(timer->isActive()){//Only if the game is moving
        QPainter painter(this);//Painter object indicates what window to render in
        apple->drawObject(painter);//Renders the Apple object

        for(int i=0;i<segments.size();i++){//Renders each segment of the snake
            (*(segments.at(i))).drawObject(painter);
        }

        for(int i=0;i<walls.size();i++){//Renders each wall segment
            (*(walls.at(i))).drawObject(painter);
        }
    }
}

void GameWindow::keyPressEvent(QKeyEvent *evt)
{//Detects what key the user has pressed
    if(false==newDirection){
        newDirection=true;
        switch(evt->key())
            {//Sets the snake based on the arrow keys, and pauses based on P and Esc
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
            case Qt::Key_Escape:
            case Qt::Key_P:
                if(!paused){
                    pauseMenu();
                }
                break;
            default:
                break;
        }
    }
}

void GameWindow::moveSnake()
{//Moves the snake across the screen
    //Stores the last segment location so it knows where to add new segments
    int backX=(*(segments.at(segments.size()-1))).getXCoord();
    int backY=(*(segments.at(segments.size()-1))).getYCoord();

    //Sets the back segment's location to 0 so the matrix doesn't think a snake is still there
    matrix[backY][backX]=0;

    //Takes the back segment and moves it to the new front location, based on
    //the direction last pressed by the player
    //The other segments remain in place, but the new back segment will move next time
    rotate(segments.begin(),segments.end()-1,segments.end());
    if(2==direction){
        (*(segments.at(0))).setXCoord((*(segments.at(1))).getXCoord()+1);
        (*(segments.at(0))).setYCoord((*(segments.at(1))).getYCoord());
    }
    else if(1==direction){
        (*(segments.at(0))).setXCoord((*(segments.at(1))).getXCoord()-1);
        (*(segments.at(0))).setYCoord((*(segments.at(1))).getYCoord());
    }
    else if(0==direction){
        (*(segments.at(0))).setXCoord((*(segments.at(1))).getXCoord());
        (*(segments.at(0))).setYCoord((*(segments.at(1))).getYCoord()-1);
    }
    else{
        (*(segments.at(0))).setXCoord((*(segments.at(1))).getXCoord());
        (*(segments.at(0))).setYCoord((*(segments.at(1))).getYCoord()+1);
    }

    //Checks if the snake has hit an apple
    if(2==matrix[(*(segments.at(0))).getYCoord()][(*(segments.at(0))).getXCoord()]){
        //Flag causes a new apple to appear next tick
        appleEaten=true;
        //Adds a segment to the snake
        RenderObject *newseg = new RenderObject(this);
        newseg->setXCoord(backX);
        newseg->setYCoord(backY);
        newseg->setImage(snakeImage);
        segments.push_back(newseg);
        //Increases the player score
        score++;
    }

    //Checks if the player has collided with a wall
    if((3==matrix[(*(segments.at(0))).getYCoord()][(*(segments.at(0))).getXCoord()])||(1==matrix[(*(segments.at(0))).getYCoord()][(*(segments.at(0))).getXCoord()])){
        //Stops all objects from moving in the background
        timer->stop();
        //Displays Game Over Message
        QMessageBox mbox;
        mbox.setText("Game Over");
        mbox.exec();
        //Checks if a new HighScore has been reached, and sets it
        scoreSet->setHighScores(difficulty,score);
        //Closes game window
        this->close();
    }

    //Sets the new front segment location to indicate a snake is there
    matrix[(*(segments.at(0))).getYCoord()][(*(segments.at(0))).getXCoord()]=1;
}

void GameWindow::moveApple()
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

void GameWindow::setDifficulty(int difficulty)
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

void GameWindow::sendHighScoreObject(HighScores *scoreSet)
{
    //This initializes the high scores object
    //Not sure exactly why, but something about creating a new one overlayed
    //the high score text onto the game window
    //So we send one from the main window instead
    this->scoreSet=scoreSet;
}

void GameWindow::pauseMenu()
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

void GameWindow::updateField()
{
    //Check that the game isn't paused
    if(!paused){
        //Renders the new snake location first
        //Otherwise it looks like it takes an extra tick to eat apples and collide with walls
        this->moveSnake();
        //Create a new apple if it has been eaten
        if(appleEaten){
            this->moveApple();
        }
        //Redraws the entire screen at once
        this->update();
        newDirection=false;
    }
}
