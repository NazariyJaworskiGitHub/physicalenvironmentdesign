#ifndef XYGLRENDERFORMATDIALOG_H
#define XYGLRENDERFORMATDIALOG_H

#include <QDialog>

namespace Ui {
class XYGLRenderFormatDialog;
}

namespace UserInterface
{
    class XYGLRenderFormatDialog : public QDialog
    {
        Q_OBJECT

    public:
        explicit XYGLRenderFormatDialog(QWidget *parent = 0);
        ~XYGLRenderFormatDialog();        

    private Q_SLOTS:
        void on_BackgroundColorButton_clicked();
        void on_TextColorButton_clicked();
        void on_TextFontComboBox_currentFontChanged(const QFont & font);
        void on_TextFontSizeSpinBox_valueChanged(int value);
        void on_GridColorButton_clicked();
        void on_NumberOfAxisSpinBox_valueChanged(int arg1);
        void on_NumberPlotNodesSpinBox_valueChanged(int arg1);

        void functionColorButtonClicked();

    private:
        Ui::XYGLRenderFormatDialog *ui;
        std::vector<QPushButton *> _colorButtons;
    };
}

#endif // XYGLRENDERFORMATDIALOG_H
