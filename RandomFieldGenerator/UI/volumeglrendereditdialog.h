#ifndef VOLUMEGLRENDEREDITDIALOG_H
#define VOLUMEGLRENDEREDITDIALOG_H

#include <QDialog>

namespace Ui {
class VolumeGLRenderEditDialog;
}

namespace UserInterface
{
    class VolumeGLRenderEditDialog : public QDialog
    {
        Q_OBJECT

    public:
        explicit VolumeGLRenderEditDialog(QWidget *parent = 0);
        ~VolumeGLRenderEditDialog();

    private Q_SLOTS:
        void on_PotentialFieldTransparencySlider_valueChanged(int value);
        void on_MaxPotentialFieldValueSlider_valueChanged(int value);
        void on_MinPotentialFieldValueSlider_valueChanged(int value);

    private:
        Ui::VolumeGLRenderEditDialog *ui;
    };
}

#endif // VOLUMEGLRENDEREDITDIALOG_H
