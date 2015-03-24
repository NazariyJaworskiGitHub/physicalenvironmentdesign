#ifndef CLMANAGERGUI_H
#define CLMANAGERGUI_H

#include <QDialog>

namespace Ui {
class CLManagerGUI;
}

namespace UserInterface
{
    class CLManagerGUI : public QDialog
    {
        Q_OBJECT

    public:
        explicit CLManagerGUI(QWidget *parent = 0);
        ~CLManagerGUI();

    public: int getPlatformIndex() const;
    public: int getDeviceIndex() const;

    private Q_SLOTS:
        void on_PlatformComboBox_currentIndexChanged(int index);
        void on_DeviceComboBox_currentIndexChanged(int index);

    private:
        Ui::CLManagerGUI *ui;
    };
}

#endif // CLMANAGERGUI_H
