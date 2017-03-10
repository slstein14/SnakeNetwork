#include "highscores.h"
#include "ui_highscores.h"
#include <fstream>
#include <QDir>
#include <QCoreApplication>
using namespace std;

HighScores::HighScores(QWidget *parent):QWidget(parent),
       ui(new Ui::HighScores)
{//Creates a HighScores object and renders it in its own form window
#ifdef __APPLE__
     QDir bin(QCoreApplication::applicationDirPath());
     bin.cdUp();
     bin.cdUp();
     bin.cdUp();
     QDir::setCurrent(bin.absolutePath());
 #endif
     //Reads High Scores from txt files
   ui->setupUi(this);
   ifstream scoreset;
   scoreset.open("highscores/highscores1.txt");
   int scores;
   if(scoreset.is_open()){
       scoreset>>scores;
       ui->Score1_1->setText(QString::number(scores));
       scoreset>>scores;
       ui->Score1_2->setText(QString::number(scores));
       scoreset>>scores;
       ui->Score1_3->setText(QString::number(scores));
       scoreset>>scores;
       ui->Score1_4->setText(QString::number(scores));
       scoreset>>scores;
       ui->Score1_5->setText(QString::number(scores));
   }//reads current high scores
   scoreset.close();

   scoreset.open("highscores/highscores2.txt");
   if(scoreset.is_open()){
       scoreset>>scores;
       ui->Score2_1->setText(QString::number(scores));
       scoreset>>scores;
       ui->Score2_2->setText(QString::number(scores));
       scoreset>>scores;
       ui->Score2_3->setText(QString::number(scores));
       scoreset>>scores;
       ui->Score2_4->setText(QString::number(scores));
       scoreset>>scores;
       ui->Score2_5->setText(QString::number(scores));
   }//reads current high scores
   scoreset.close();

   scoreset.open("highscores/highscores3.txt");
   if(scoreset.is_open()){
       scoreset>>scores;
       ui->Score3_1->setText(QString::number(scores));
       scoreset>>scores;
       ui->Score3_2->setText(QString::number(scores));
       scoreset>>scores;
       ui->Score3_3->setText(QString::number(scores));
       scoreset>>scores;
       ui->Score3_4->setText(QString::number(scores));
       scoreset>>scores;
       ui->Score3_5->setText(QString::number(scores));
   }//reads current high scores
   scoreset.close();

}

void HighScores::setHighScores(int difficulty, int gamescore)
{
#ifdef __APPLE__
     QDir bin(QCoreApplication::applicationDirPath());
     bin.cdUp();
     bin.cdUp();
     bin.cdUp();
     QDir::setCurrent(bin.absolutePath());
 #endif
     //Determines which file is approrpiate (difficulty setting)
     //Checks if a new high score is achieved
     //Updates file if necessary
    int scorefile = difficulty;
        QString filename = "highscores/highscores"+QString::number(scorefile)+".txt";
        ifstream scoreset;
        scoreset.open(filename.toStdString().c_str());
        int scores;

        if(scoreset.is_open()){

            scoreset >> scores;
            int firstscore = scores;
            scoreset >> scores;
            int secondscore = scores;
            scoreset >> scores;
            int thirdscore = scores;
            scoreset >> scores;
            int fourthscore = scores;
            scoreset >> scores;
            int fifthscore = scores;
            scoreset.close();

            if(firstscore < gamescore)
            {
                fifthscore = fourthscore;
                fourthscore = thirdscore;
                thirdscore = secondscore;
                secondscore = firstscore;
                firstscore = gamescore;

                QMessageBox sbox;
                sbox.setText("New High Score: "+ QString::number(gamescore));
                sbox.exec();
            }
            else if(secondscore < gamescore)
            {
                fifthscore = fourthscore;
                fourthscore = thirdscore;
                thirdscore = secondscore;
                secondscore = gamescore;
            }
            else if(thirdscore < gamescore)
            {
                fifthscore = fourthscore;
                fourthscore = thirdscore;
                thirdscore = gamescore;
            }
            else if(fourthscore < gamescore)
            {
                fifthscore = fourthscore;
                fourthscore = gamescore;
            }
            else if(fifthscore<gamescore)

            {
                fifthscore = gamescore;
            }

            ofstream setscores;
            setscores.open(filename.toStdString().c_str());

            setscores << firstscore << endl;
            setscores << secondscore << endl;
            setscores << thirdscore << endl;
            setscores << fourthscore << endl;
            setscores << fifthscore << endl;

            setscores.close();
        }
}//Runs High Score Window

HighScores::~HighScores()
{
    delete ui;
}

void HighScores::on_reset_clicked()
{
#ifdef __APPLE__
     QDir bin(QCoreApplication::applicationDirPath());
     bin.cdUp();
     bin.cdUp();
     bin.cdUp();
     QDir::setCurrent(bin.absolutePath());
 #endif

     //Resets the High Scores to zero
    ofstream reset;
    reset.open("highscores/highscores1.txt");
    for(int i=0;i<5;i++){
        reset<<0<<endl;
    }
    reset.close();

    reset.open("highscores/highscores2.txt");
    for(int i=0;i<5;i++){
        reset<<0<<endl;
    }
    reset.close();

    reset.open("highscores/highscores3.txt");
    for(int i=0;i<5;i++){
        reset<<0<<endl;
    }
    reset.close();

    ifstream scoreset;
    scoreset.open("highscores/highscores1.txt");
    int scores;
    if(scoreset.is_open()){
        scoreset>>scores;
        ui->Score1_1->setText(QString::number(scores));
        scoreset>>scores;
        ui->Score1_2->setText(QString::number(scores));
        scoreset>>scores;
        ui->Score1_3->setText(QString::number(scores));
        scoreset>>scores;
        ui->Score1_4->setText(QString::number(scores));
        scoreset>>scores;
        ui->Score1_5->setText(QString::number(scores));
    }

    scoreset.open("highscores/highscores2.txt");
    if(scoreset.is_open()){
        scoreset>>scores;
        ui->Score2_1->setText(QString::number(scores));
        scoreset>>scores;
        ui->Score2_2->setText(QString::number(scores));
        scoreset>>scores;
        ui->Score2_3->setText(QString::number(scores));
        scoreset>>scores;
        ui->Score2_4->setText(QString::number(scores));
        scoreset>>scores;
        ui->Score2_5->setText(QString::number(scores));
    }//reads current high scores
    scoreset.close();

    scoreset.open("highscores/highscores3.txt");
    if(scoreset.is_open()){
        scoreset>>scores;
        ui->Score3_1->setText(QString::number(scores));
        scoreset>>scores;
        ui->Score3_2->setText(QString::number(scores));
        scoreset>>scores;
        ui->Score3_3->setText(QString::number(scores));
        scoreset>>scores;
        ui->Score3_4->setText(QString::number(scores));
        scoreset>>scores;
        ui->Score3_5->setText(QString::number(scores));
    }//reads current high scores
    scoreset.close();


}//Resets the High Scores


