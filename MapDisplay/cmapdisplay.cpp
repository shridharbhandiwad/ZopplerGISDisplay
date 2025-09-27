#include "cmapdisplay.h"
#include "ui_cmapdisplay.h"

CMapDisplay::CMapDisplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CMapDisplay)
{
    ui->setupUi(this);
}

CMapDisplay::~CMapDisplay()
{
    delete ui;
}
