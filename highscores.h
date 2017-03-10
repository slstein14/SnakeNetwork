#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include <QMessageBox>
#include <QObject>
#include <QWidget>

namespace Ui {
class HighScores;
}

class HighScores : public QWidget
{
    Q_OBJECT

public:
    explicit HighScores(QWidget *parent = 0);
    void setHighScores(int difficulty, int gamescore);
    ~HighScores();

private slots:
    void on_reset_clicked();

private:
    Ui::HighScores *ui;
};

#endif // HIGHSCORES_H
