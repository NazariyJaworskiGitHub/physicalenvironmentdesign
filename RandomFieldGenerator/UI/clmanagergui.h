#ifndef CLMANAGERGUI_H
#define CLMANAGERGUI_H

#include <QDialog>

namespace Ui {
class CLManagerGUI;
}

class CLManagerGUI : public QDialog
{
    Q_OBJECT

public:
    explicit CLManagerGUI(QWidget *parent = 0);
    ~CLManagerGUI();

private:
    Ui::CLManagerGUI *ui;
};

#endif // CLMANAGERGUI_H
