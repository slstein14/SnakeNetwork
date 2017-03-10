#include "contactus.h"
#include "ui_contactus.h"

ContactUs::ContactUs(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContactUs)
{
    ui->setupUi(this);
}//Runs Contact Us Window

ContactUs::~ContactUs()
{
    delete ui;
}//clears potential memory leaks
