#ifndef VOLUMEGLRENDERRVEEDITDIALOG_H
#define VOLUMEGLRENDERRVEEDITDIALOG_H

#include <QDialog>
#include "filterpreviewglrender.h"
#include "inclusionpreviewglrender.h"
#include "curvepreviewglrender.h"

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

        void on_SaveRVEButton_clicked();

        void on_LoadRVEButton_clicked();

        void on_CoreIntensitySlider_Fiber_sliderReleased();
        void on_CoreIntensitySlider_Fiber_valueChanged(int value);

        void on_RotationOZSlider_Fiber_sliderReleased();
        void on_RotationOZSlider_Fiber_valueChanged(int value);

        void on_RotationOYSlider_Fiber_sliderReleased();
        void on_RotationOYSlider_Fiber_valueChanged(int value);

        void on_RotationOXSlider_Fiber_sliderReleased();
        void on_RotationOXSlider_Fiber_valueChanged(int value);

        void on_TransitionLayerSlider_Fiber_sliderReleased();
        void on_TransitionLayerSlider_Fiber_valueChanged(int value);

        void on_PathDeviationSlider_sliderReleased();
        void on_PathDeviationSlider_valueChanged(int value);

        void on_FiberRadiusSlider_sliderReleased();
        void on_FiberRadiusSlider_valueChanged(int value);

        void on_MinimalScaleSlider_sliderReleased();
        void on_MinimalScaleSlider_valueChanged(int value);

        void on_FiberLengthSlider_sliderReleased();
        void on_FiberLengthSlider_valueChanged(int value);

        void on_ApproximationPointsSlider_sliderReleased();
        void on_ApproximationPointsSlider_valueChanged(int value);

        void on_BezierCurveOrderSlider_sliderReleased();
        void on_BezierCurveOrderSlider_valueChanged(int value);

        void on_UseRandomOrientationCheckBox_Fiber_stateChanged(int arg1);

        void on_GenerateInclusionsButton_Fiber_clicked();

    private:
        Ui::VolumeGLRenderRVEEditDialog *ui;
        FilterPreviewGLRender *_previewRender = nullptr;
        InclusionPreviewGLRender *_previewRender_Inclusion = nullptr;
        CurvePreviewGLRender *_previewRender_Fiber = nullptr;

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
        float getTransitionLayerValue() const;
        float getFilterScaleFactorXValue_Inclusion() const;
        float getFilterScaleFactorYValue_Inclusion() const;
        float getFilterScaleFactorZValue_Inclusion() const;
        float getFilterRotationOXValue_Inclusion() const;
        float getFilterRotationOYValue_Inclusion() const;
        float getFilterRotationOZValue_Inclusion() const;
        float getCoreIntensityValue() const;

        int getCurveOrder() const;
        int getCurveApproximationPoints() const;
        int getFiberLength() const;
        float getMinimalScale() const;
        int getFiberRadius() const;
        float getPathDeviation() const;
        float getTransitionLayerValue_Fiber() const;
        float getFilterRotationOXValue_Fiber() const;
        float getFilterRotationOYValue_Fiber() const;
        float getFilterRotationOZValue_Fiber() const;
        float getCoreIntensityValue_Fiber() const;

    public: /// See UserInterfaceManager, there should be same transitive signals with suffix _T
        Q_SIGNAL void signal_loadRVE(QString fileName);
        Q_SIGNAL void signal_saveRVE(QString fileName);

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

        Q_SIGNAL void signal_generateOverlappingRandomBezierCurveIntenseRVE(
                int curveNum,
                int curveOrder,
                int curveApproximationPoints,
                int discreteLength,
                float minScale,
                int curveRadius,
                float pathDeviation,
                float transitionLayerSize,
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
        void _updatePreview_Fiber();
    };
}

#endif // VOLUMEGLRENDERRVEEDITDIALOG_H
