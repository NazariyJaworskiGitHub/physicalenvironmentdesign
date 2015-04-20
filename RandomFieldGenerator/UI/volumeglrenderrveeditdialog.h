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
        void on_FilterRadiusSlider_sliderReleased();

        void on_ScaleFactorXSlider_valueChanged(int value);
        void on_ScaleFactorXSlider_sliderReleased();

        void on_ScaleFactorYSlider_valueChanged(int value);
        void on_ScaleFactorYSlider_sliderReleased();

        void on_ScaleFactorZSlider_valueChanged(int value);
        void on_ScaleFactorZSlider_sliderReleased();

        void on_ApplyGaussianFilterButton_clicked();

        void on_UseDataAsIntensityCheckBox_stateChanged(int arg1);

        void on_ClenRVEButton_clicked();

        void on_NormalizeRVEButton_clicked();

        void on_InvertRVEButton_clicked();

        void on_CleanMaskButton_clicked();

        void on_CleanUnMaskedElementsButton_clicked();

        void on_ApplyTwoCutMaskInsideButton_clicked();

        void on_ApplyTwoCutMaskOutsideButton_clicked();

        void on_addRandomNoiseButton_clicked();

    private:
        Ui::VolumeGLRenderRVEEditDialog *ui;
        FilterPreviewGLRender *_previewRender = nullptr;

    public:
        float getFilterRadiusValue() const;

        float getFilterScaleFactorXValue() const;

        float getFilterScaleFactorYValue() const;

        float getFilterScaleFactorZValue() const;

    public: /// See UserInterfaceManager, there should be same transitive signals with suffix _T
        Q_SIGNAL void signal_cleanRVE();
        Q_SIGNAL void signal_normalizeUnMaskedRVE();
        Q_SIGNAL void signal_invertUnMaskedRVE();

        Q_SIGNAL void signal_cleanMaskRVE();
        Q_SIGNAL void signal_cleanUnMaskedRVE(float filler);
        Q_SIGNAL void signal_applyTwoCutMaskInsideRVE(float cutLevelA, float cutLevelB);
        Q_SIGNAL void signal_applyTwoCutMaskOutsideRVE(float cutLevelA, float cutLevelB);

        Q_SIGNAL void signal_addRandomNoiseRVE();

        Q_SIGNAL void signal_applyGaussianFilterRVE(
                int discreteRadius,
                float ellipsoidScaleFactorX,
                float ellipsoidScaleFactorY,
                float ellipsoidScaleFactorZ,
                bool useDataAsIntensity,
                float intensityFactor);

    private:
        void _disableWigget();
        Q_SLOT void _enableWidget();
        void _updatePreview();
    };
}

#endif // VOLUMEGLRENDERRVEEDITDIALOG_H
