#include "clmanagergui.h"
#include "ui_clmanagergui.h"

CLManagerGUI::CLManagerGUI(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CLManagerGUI)
{
    ui->setupUi(this);
}

CLManagerGUI::~CLManagerGUI()
{
    delete ui;
}
