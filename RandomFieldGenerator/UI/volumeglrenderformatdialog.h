#ifndef VOLUMEGLRENDERFORMATDIALOG_H
#define VOLUMEGLRENDERFORMATDIALOG_H

#include <QDialog>

namespace Ui {
class VolumeGLRenderFormatDialog;
}

namespace UserInterface
{
    class VolumeGLRenderFormatDialog : public QDialog
    {
        Q_OBJECT

    public:
        explicit VolumeGLRenderFormatDialog(QWidget *parent = 0);
        ~VolumeGLRenderFormatDialog();

    private Q_SLOTS:
        void on_PotentialFieldTransparencySlider_valueChanged(int value);

    private:
        Ui::VolumeGLRenderFormatDialog *ui;
    };
}

#endif // VOLUMEGLRENDERFORMATDIALOG_H
