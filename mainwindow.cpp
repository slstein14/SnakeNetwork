//Sam Stein and Joey Rockwood
//This file contains the coding to make the Main window run and update properly,
//and to make the menus accessible to the user.
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFont>
#include <QDir>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
#ifdef __APPLE__
     QDir bin(QCoreApplication::applicationDirPath());
     bin.cdUp();
     bin.cdUp();
     bin.cdUp();
     QDir::setCurrent(bin.absolutePath());
 #endif
     //Declare Variables and set background image for main window
    ui->setupUi(this);
    QPixmap bkgnd("images/Snake.png");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);
    game=NULL;
    game2=NULL;
    hScore = NULL;
    contact = NULL;
    controls = NULL;
    host=NULL;
    join=NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete hScore;
    delete contact;
    delete controls;
    delete game;
    delete game2;
    delete host;
    delete game;
}

void MainWindow::on_actionQuit_triggered()
{
    //Close main window
    this->close();
}

void MainWindow::on_actionHigh_Score_triggered()
{//Creates a high score object if none exists, then displays it
    if(hScore==NULL)
    {
        hScore = new HighScores();
    }else{
        delete hScore;
        hScore = new HighScores();
    }

    hScore->show();
}

void MainWindow::on_actionEasy_triggered()
{//Starts game on easy
    if(hScore==NULL)
    {
        hScore = new HighScores();
    }else{
        delete hScore;
        hScore = new HighScores();
    }

    if(game == NULL)
        {
            game = new GameWindow();
            game->setDifficulty(1);
            game->sendHighScoreObject(hScore);
        }else{
            delete game;
            game = new GameWindow();
            game->setDifficulty(1);
            game->sendHighScoreObject(hScore);
        }
        game->show();
}

void MainWindow::on_actionMedium_triggered()
{//Starts game on medium
    if(hScore==NULL)
    {
        hScore = new HighScores();
    }else{
        delete hScore;
        hScore = new HighScores();
    }

    if(game == NULL)
        {
            game = new GameWindow();
            game->setDifficulty(2);
            game->sendHighScoreObject(hScore);
        }else{
            delete game;
            game = new GameWindow();
            game->setDifficulty(2);
            game->sendHighScoreObject(hScore);
        }
        game->show();
}

void MainWindow::on_actionHard_triggered()
{//Starts game on hard
    if(hScore==NULL)
    {
        hScore = new HighScores();
    }else{
        delete hScore;
        hScore = new HighScores();
    }

    if(game == NULL)
        {
            game = new GameWindow();
            game->setDifficulty(3);
            game->sendHighScoreObject(hScore);
        }else{
            delete game;
            game = new GameWindow();
            game->setDifficulty(3);
            game->sendHighScoreObject(hScore);
        }
        game->show();
}

void MainWindow::on_actionControls_triggered()
{
    if(controls==NULL){
        controls = new Controls();
    }
    controls->show();
}

void MainWindow::on_actionContact_Us_triggered()
{
    if(contact==NULL){
        contact = new ContactUs();
    }
    contact->show();
}

void MainWindow::on_action2_Player_triggered()
{
    if(hScore==NULL)
    {
        hScore = new HighScores();
    }else{
        delete hScore;
        hScore = new HighScores();
    }

    if(game2 == NULL)
        {
            game2 = new GameWindow2Player();
            game2->setDifficulty(1);
            game2->sendHighScoreObject(hScore);
        }else{
            delete game2;
            game2 = new GameWindow2Player();
            game2->setDifficulty(1);
            game2->sendHighScoreObject(hScore);
        }
        game2->show();
}

void MainWindow::on_action2_Player_Host_triggered()
{
    if(host==NULL){
        host = new HostGame();
    }
    host->show();
}

void MainWindow::on_action2_Player_Join_triggered()
{
    if(join==NULL){
        join = new JoinGame();
    }
    join->show();
}
