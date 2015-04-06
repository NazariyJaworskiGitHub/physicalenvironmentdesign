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
        void on_BackgroundColorButton_clicked();
        void on_TextColorButton_clicked();
        void on_TextFontComboBox_currentFontChanged(const QFont & font);
        void on_TextFontSizeSpinBox_valueChanged(int value);

    private:
        Ui::VolumeGLRenderFormatDialog *ui;
//        QDoubleValidator *_validator = nullptr;
    };
}

#endif // VOLUMEGLRENDERFORMATDIALOG_H
