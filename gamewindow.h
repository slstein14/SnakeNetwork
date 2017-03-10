#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H
#include "renderobject.h"
#include <QWidget>
#include <QPixmap>
#include <QTimer>
#include <QKeyEvent>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>
#include <vector>
#include <QGenericMatrix>
#include <vector>
#include "highscores.h"
using namespace std;

class GameWindow : public QWidget
{
    Q_OBJECT
private:
    QTimer *timer;
    RenderObject *player;
    RenderObject* wall;
    RenderObject* apple;
    HighScores* scoreSet;
    vector<RenderObject*>walls;
    vector<RenderObject*>segments;
    int matrix[48][64];
    bool appleEaten;
    int difficulty;
    int score;
    bool paused;
    int direction;
    bool newDirection;
    QMessageBox *msg;
    QMessageBox *pbox;
    QPixmap *snakeImage;
    QPixmap *wallImage;
    QPixmap *appleImage;
public:
    GameWindow(QWidget *parent = 0);
    void paintEvent(QPaintEvent *e);
    void keyPressEvent(QKeyEvent *evt);
    void moveSnake();
    void moveApple();
    void setDifficulty(int difficulty);
    void sendHighScoreObject(HighScores* scoreSet);
    void pauseMenu();


public slots:
    void updateField();
};

#endif // GAMEWINDOW_H
