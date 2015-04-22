#ifndef VOLUMEGLRENDERRVEEDITDIALOG_H
#define VOLUMEGLRENDERRVEEDITDIALOG_H

#include <QDialog>
#include "filterpreviewglrender.h"
#include "inclusionpreviewglrender.h"

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

        void on_RotationOXSlider_valueChanged(int value);
        void on_RotationOXSlider_sliderReleased();

        void on_RotationOYSlider_valueChanged(int value);
        void on_RotationOYSlider_sliderReleased();

        void on_RotationOZSlider_valueChanged(int value);
        void on_RotationOZSlider_sliderReleased();

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

        void on_GenerateVoronoiCellsButton_clicked();

        void on_UseRotationsCheckBox_stateChanged(int arg1);

        void on_MinRadiusSlider_valueChanged(int value);
        void on_MinRadiusSlider_sliderReleased();

        void on_MaxRadiusSlider_valueChanged(int value);
        void on_MaxRadiusSlider_sliderReleased();

        void on_TransitionLayerSlider_valueChanged(int value);
        void on_TransitionLayerSlider_sliderReleased();

        void on_ScaleFactorXSlider_Inclusion_valueChanged(int value);
        void on_ScaleFactorXSlider_Inclusion_sliderReleased();

        void on_ScaleFactorYSlider_Inclusion_valueChanged(int value);
        void on_ScaleFactorYSlider_Inclusion_sliderReleased();

        void on_ScaleFactorZSlider_Inclusion_valueChanged(int value);
        void on_ScaleFactorZSlider_Inclusion_sliderReleased();

        void on_UseRandomOrientationCheckBox_stateChanged(int arg1);

        void on_RotationOXSlider_Inclusion_valueChanged(int value);
        void on_RotationOXSlider_Inclusion_sliderReleased();

        void on_RotationOYSlider_Inclusion_valueChanged(int value);
        void on_RotationOYSlider_Inclusion_sliderReleased();

        void on_RotationOZSlider_Inclusion_valueChanged(int value);
        void on_RotationOZSlider_Inclusion_sliderReleased();

        void on_CoreIntensitySlider_valueChanged(int value);

        void on_CoreIntensitySlider_sliderReleased();

        void on_GenerateInclusionsButton_clicked();

    private:
        Ui::VolumeGLRenderRVEEditDialog *ui;
        FilterPreviewGLRender *_previewRender = nullptr;
        InclusionPreviewGLRender *_previewRender_Inclusion = nullptr;

    public:
        float getFilterRadiusValue() const;

        float getFilterScaleFactorXValue() const;
        float getFilterScaleFactorYValue() const;
        float getFilterScaleFactorZValue() const;

        float getFilterRotationOXValue() const;
        float getFilterRotationOYValue() const;
        float getFilterRotationOZValue() const;

        float getMinRadiusValue() const;
        float getMaxRadiusValue() const;

        float getFilterScaleFactorXValue_Inclusion() const;
        float getFilterScaleFactorYValue_Inclusion() const;
        float getFilterScaleFactorZValue_Inclusion() const;

        float getFilterRotationOXValue_Inclusion() const;
        float getFilterRotationOYValue_Inclusion() const;
        float getFilterRotationOZValue_Inclusion() const;

        float getCoreIntensityValue() const;

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
                float intensityFactor,
                bool useRotations,
                float rotationOX,
                float rotationOY,
                float rotationOZ);

        Q_SIGNAL void signal_generateOverlappingRandomEllipsoidsIntenseRVE(
                int ellipsoidNum,
                int minRadius,
                int maxRadius,
                float transitionLayerSize,
                float ellipsoidScaleFactorX,
                float ellipsoidScaleFactorY,
                float ellipsoidScaleFactorZ,
                bool useRandomRotations,
                float rotationOX,
                float rotationOY,
                float rotationOZ,
                float coreValue);

        Q_SIGNAL void signal_generateVoronoiRandomCellsRVE(int cellNum);

    private:
        void _disableWigget();
        Q_SLOT void _enableWidget();
        void _updatePreview();
        void _updatePreview_Inclusion();
    };
}

#endif // VOLUMEGLRENDERRVEEDITDIALOG_H
