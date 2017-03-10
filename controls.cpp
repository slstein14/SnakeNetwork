#include "controls.h"
#include "ui_controls.h"

Controls::Controls(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Controls)
{
    ui->setupUi(this);
}//Runs Controls Window

Controls::~Controls()
{
    delete ui;
}//clears potential memory leaks
