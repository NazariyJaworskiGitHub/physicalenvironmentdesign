#ifndef VOLUMEGLRENDERRVEEDITDIALOG_H
#define VOLUMEGLRENDERRVEEDITDIALOG_H

#include <QDialog>
#include "filterpreviewglrender.h"

namespace Ui {
class VolumeGLRenderRVEEditDialog;
}

namespace UserInterface
{
    class VolumeGLRenderRVEEditDialog : public QDialog
    {
        Q_OBJECT

    public:
        explicit VolumeGLRenderRVEEditDialog(QWidget *parent = 0);
        ~VolumeGLRenderRVEEditDialog();

    private Q_SLOTS:
        void on_BottomCutLevelSlider_valueChanged(int value);

        void on_TopCutLevelSlider_valueChanged(int value);

        void on_FilterRadiusSlider_valueChanged(int value);

        void on_ScaleFactorXSlider_valueChanged(int value);

        void on_ScaleFactorYSlider_valueChanged(int value);

        void on_ScaleFactorZSlider_valueChanged(int value);

        void on_GenerateRandomFieldPushButton_clicked();

    private:
        Ui::VolumeGLRenderRVEEditDialog *ui;
        FilterPreviewGLRender *_previewRender = nullptr;

    public:
        float getFilterRadiusValue() const;

        float getFilterScaleFactorXValue() const;

        float getFilterScaleFactorYValue() const;

        float getFilterScaleFactorZValue() const;
    };
}

#endif // VOLUMEGLRENDERRVEEDITDIALOG_H
