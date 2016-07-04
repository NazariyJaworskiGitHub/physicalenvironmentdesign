#include "volumeglrenderrveeditdialog.h"
#include "ui_volumeglrenderrveeditdialog.h"

#include "volumeglrenderrve.h"

#include "userinterfacemanager.h"

#include <QDoubleValidator>
#include <QIntValidator>
#include <QFileDialog>

using namespace UserInterface;

VolumeGLRenderRVEEditDialog::VolumeGLRenderRVEEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VolumeGLRenderRVEEditDialog)
{
    ui->setupUi(this);

    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());

    // Masking
    ui->BottomCutLevelSlider->setValue(_parent->_innerBottomCutLevel * 1000);
    ui->BottomCutLevelLineEdit->setText(
                QString::number(ui->BottomCutLevelSlider->value() / 1000.0f));

    ui->TopCutLevelSlider->setValue(_parent->_innerTopCutLevel * 1000);
    ui->TopCutLevelLineEdit->setText(
                QString::number(ui->TopCutLevelSlider->value() / 1000.0f));

    // Gaussian filter
    ui->FilterRadiusSlider->setMaximum(_parent->_ptrToRVE->getSize());
    ui->FilterRadiusSlider->setValue(_parent->_FilterRadiusBackup);
    ui->FilterRadiusLineEdit->setText(QString::number(ui->FilterRadiusSlider->value()));

    ui->ScaleFactorXSlider->setValue(_parent->_FilterScaleFactorXBackup * 100);
    ui->ScileFactorXLineEdit->setText(QString::number(ui->ScaleFactorXSlider->value() / 100.0f));

    ui->ScaleFactorYSlider->setValue(_parent->_FilterScaleFactorYBackup * 100);
    ui->ScileFactorYLineEdit->setText(QString::number(ui->ScaleFactorYSlider->value() / 100.0f));

    ui->ScaleFactorZSlider->setValue(_parent->_FilterScaleFactorZBackup * 100);
    ui->ScileFactorZLineEdit->setText(QString::number(ui->ScaleFactorZSlider->value() / 100.0f));

    ui->RotationOXSlider->setValue(_parent->_FilterRotationOXBackup);
    ui->RotationOXLineEdit->setText(QString::number(ui->RotationOXSlider->value()));

    ui->RotationOYSlider->setValue(_parent->_FilterRotationOYBackup);
    ui->RotationOYLineEdit->setText(QString::number(ui->RotationOYSlider->value()));

    ui->RotationOZSlider->setValue(_parent->_FilterRotationOZBackup);
    ui->RotationOZLineEdit->setText(QString::number(ui->RotationOZSlider->value()));

    ui->UseDataAsIntensityCheckBox->setCheckState(Qt::Unchecked);
    ui->IntensityFactorLineEdit->setEnabled(false);
    ui->IntensityFactorLabel->setEnabled(false);

    QDoubleValidator *_doubleValidator = new QDoubleValidator(this);
    ui->IntensityFactorLineEdit->setValidator(_doubleValidator);

    ui->UseRotationsCheckBox->setCheckState(Qt::Unchecked);
    ui->RotationOXLabel->setEnabled(false);
    ui->RotationOXLineEdit->setEnabled(false);
    ui->RotationOXSlider->setEnabled(false);
    ui->RotationOYLabel->setEnabled(false);
    ui->RotationOYLineEdit->setEnabled(false);
    ui->RotationOYSlider->setEnabled(false);
    ui->RotationOZLabel->setEnabled(false);
    ui->RotationOZLineEdit->setEnabled(false);
    ui->RotationOZSlider->setEnabled(false);

    _previewRender = new FilterPreviewGLRender(
                _parent->_ptrToRVE, ui->GaussianFilter);
    _previewRender->resize(261,261);
    _previewRender->move(510, 10);

    // Voronoi cells
    QIntValidator *_intValidator = new QIntValidator(this);
    _intValidator->setBottom(2);
    ui->NumberOfCellsLineEdit->setValidator(_intValidator);

    // Ellipsoid inclusions
    QIntValidator *_intValidator2 = new QIntValidator(this);
    _intValidator->setBottom(1);
    ui->NumberOfInclusionsLineEdit->setValidator(_intValidator2);

    ui->MaxRadiusSlider->setMaximum(_parent->_ptrToRVE->getSize());
    ui->MaxRadiusSlider->setValue(_parent->_MaxRadiusBackup);
    ui->MaxRadiusLineEdit->setText(QString::number(ui->MaxRadiusSlider->value()));

    ui->MinRadiusSlider->setMaximum(_parent->_ptrToRVE->getSize());
    ui->MinRadiusSlider->setValue(_parent->_MinRadiusBackup);
    ui->MinRadiusLineEdit->setText(QString::number(ui->MinRadiusSlider->value()));

    ui->TransitionLayerSlider->setValue(_parent->_TransitionLayerBackup * 100);
    ui->TransitionLayerLineEdit->setText(QString::number(ui->TransitionLayerSlider->value() / 100.0f));

    ui->ScaleFactorXSlider_Inclusion->setValue(_parent->_FilterScaleFactorXBackup * 100);
    ui->ScileFactorXLineEdit_Inclusion->setText(QString::number(ui->ScaleFactorXSlider_Inclusion->value() / 100.0f));

    ui->ScaleFactorYSlider_Inclusion->setValue(_parent->_FilterScaleFactorYBackup * 100);
    ui->ScileFactorYLineEdit_Inclusion->setText(QString::number(ui->ScaleFactorYSlider_Inclusion->value() / 100.0f));

    ui->ScaleFactorZSlider_Inclusion->setValue(_parent->_FilterScaleFactorZBackup * 100);
    ui->ScileFactorZLineEdit_Inclusion->setText(QString::number(ui->ScaleFactorZSlider_Inclusion->value() / 100.0f));

    ui->UseRandomOrientationCheckBox->setCheckState(Qt::Unchecked);

    ui->RotationOXSlider_Inclusion->setValue(_parent->_FilterRotationOXBackup);
    ui->RotationOXLineEdit_Inclusion->setText(QString::number(ui->RotationOXSlider_Inclusion->value()));

    ui->RotationOYSlider_Inclusion->setValue(_parent->_FilterRotationOYBackup);
    ui->RotationOYLineEdit_Inclusion->setText(QString::number(ui->RotationOYSlider_Inclusion->value()));

    ui->RotationOZSlider_Inclusion->setValue(_parent->_FilterRotationOZBackup);
    ui->RotationOZLineEdit_Inclusion->setText(QString::number(ui->RotationOZSlider_Inclusion->value()));

    ui->CoreIntensitySlider->setValue(_parent->_CoreIntensityBackup * 100);
    ui->CoreIntensityLineEdit->setText(QString::number(ui->CoreIntensitySlider->value() / 100.0f));

    _previewRender_Inclusion = new InclusionPreviewGLRender(
                _parent->_ptrToRVE, ui->Inclusions);
    _previewRender_Inclusion->resize(261,261);
    _previewRender_Inclusion->move(510, 10);

    // Fiber inclusions
    QIntValidator *_intValidator3 = new QIntValidator(this);
    _intValidator->setBottom(1);
    ui->NumberOfFibersLineEdit->setValidator(_intValidator3);

    ui->BezierCurveOrderSlider->setValue(_parent->_CurveOrderBackup);
    ui->BezierCurveOrderLineEdit->setText(QString::number(ui->BezierCurveOrderSlider->value()));

    ui->ApproximationPointsSlider->setValue(_parent->_CurveApproximationPointsBackup);
    ui->ApproximationPointsLineEdit->setText(QString::number(ui->ApproximationPointsSlider->value()));

    ui->FiberLengthSlider->setMaximum(_parent->_ptrToRVE->getSize());
    ui->FiberLengthSlider->setValue(_parent->_FiberLengthBackup);
    ui->FiberLengthLineEdit->setText(QString::number(ui->FiberLengthSlider->value()));

    ui->FiberRadiusSlider->setMaximum(_parent->_ptrToRVE->getSize());
    ui->FiberRadiusSlider->setValue(_parent->_MinRadiusBackup);
    ui->FiberRadiusLineEdit->setText(QString::number(ui->FiberRadiusSlider->value()));

    ui->MinimalScaleSlider->setValue(_parent->_MinScaleBackup * 100);
    ui->MinimalScaleLineEdit->setText(QString::number(ui->MinimalScaleSlider->value() / 100.0f));

    ui->PathDeviationSlider->setValue(_parent->_PathDeviationBackup * 100);
    ui->PathDeviationLineEdit->setText(QString::number(ui->PathDeviationSlider->value() / 100.0f));

    ui->TransitionLayerSlider_Fiber->setValue(_parent->_TransitionLayerBackup * 100);
    ui->TransitionLayerLineEdit_Fiber->setText(QString::number(ui->TransitionLayerSlider_Fiber->value() / 100.0f));

    ui->UseRandomOrientationCheckBox_Fiber->setCheckState(Qt::Unchecked);

    ui->RotationOXSlider_Fiber->setValue(_parent->_FilterRotationOXBackup);
    ui->RotationOXLineEdit_Fiber->setText(QString::number(ui->RotationOXSlider_Fiber->value()));

    ui->RotationOYSlider_Fiber->setValue(_parent->_FilterRotationOYBackup);
    ui->RotationOYLineEdit_Fiber->setText(QString::number(ui->RotationOYSlider_Fiber->value()));

    ui->RotationOZSlider_Fiber->setValue(_parent->_FilterRotationOZBackup);
    ui->RotationOZLineEdit_Fiber->setText(QString::number(ui->RotationOZSlider_Fiber->value()));

    ui->CoreIntensitySlider_Fiber->setValue(_parent->_CoreIntensityBackup * 100);
    ui->CoreIntensityLineEdit_Fiber->setText(QString::number(ui->CoreIntensitySlider_Fiber->value() / 100.0f));

    _previewRender_Fiber = new CurvePreviewGLRender(
                _parent->_ptrToRVE, ui->Fiber);
    _previewRender_Fiber->resize(261,261);
    _previewRender_Fiber->move(510, 10);

    // Layers
    ui->LayerBottomSideSlider_Layer->setMaximum(_parent->_ptrToRVE->getSize()-2);
    ui->LayerBottomSideSlider_Layer->setMinimum(0);
    ui->LayerBottomSideSlider_Layer->setValue(0);
    ui->LayerBottomSideLineEdit_Layer->setText(0);

    ui->LayerTopSideSlider_Layer->setMaximum(_parent->_ptrToRVE->getSize()-1);
    ui->LayerTopSideSlider_Layer->setMinimum(1);
    ui->LayerTopSideSlider_Layer->setValue(_parent->_ptrToRVE->getSize()-1);
    ui->LayerTopSideLineEdit_Layer->setText(QString::number(_parent->_ptrToRVE->getSize()-1));

    ui->CoreIntensitySlider_Layer->setValue(_parent->_CoreIntensityBackup * 100);
    ui->CoreIntensityLineEdit_Layer->setText(QString::number(ui->CoreIntensitySlider_Layer->value() / 100.0f));

    // Signals and slots
    connect(this,SIGNAL(signal_loadRVE(QString)),
            &UserInterfaceManager::instance(),
            SIGNAL(signal_loadRVE_T(QString)),
            Qt::QueuedConnection);
    connect(&UserInterfaceManager::instance(), SIGNAL(signal_loadRVEDone_T()),
            this, SLOT(_enableWidget()), Qt::QueuedConnection);

    connect(this,SIGNAL(signal_saveRVE(QString)),
            &UserInterfaceManager::instance(),
            SIGNAL(signal_saveRVE_T(QString)),
            Qt::QueuedConnection);
    connect(&UserInterfaceManager::instance(), SIGNAL(signal_saveRVEDone_T()),
            this, SLOT(_enableWidget()), Qt::QueuedConnection);

    connect(this,SIGNAL(signal_cleanRVE()),
            &UserInterfaceManager::instance(),
            SIGNAL(signal_cleanRVE_T()),
            Qt::QueuedConnection);
    connect(&UserInterfaceManager::instance(), SIGNAL(signal_cleanRVEDone_T()),
            this, SLOT(_enableWidget()), Qt::QueuedConnection);

    connect(this,SIGNAL(signal_normalizeUnMaskedRVE()),
            &UserInterfaceManager::instance(),
            SIGNAL(signal_normalizeUnMaskedRVE_T()),
            Qt::QueuedConnection);
    connect(&UserInterfaceManager::instance(), SIGNAL(signal_normalizeUnMaskedRVEDone_T()),
            this, SLOT(_enableWidget()), Qt::QueuedConnection);

    connect(this,SIGNAL(signal_invertUnMaskedRVE()),
            &UserInterfaceManager::instance(),
            SIGNAL(signal_invertUnMaskedRVE_T()),
            Qt::QueuedConnection);
    connect(&UserInterfaceManager::instance(), SIGNAL(signal_invertUnMaskedRVEDone_T()),
            this, SLOT(_enableWidget()), Qt::QueuedConnection);

    connect(this,SIGNAL(signal_cleanMaskRVE()),
            &UserInterfaceManager::instance(),
            SIGNAL(signal_cleanMaskRVE_T()),
            Qt::QueuedConnection);
    connect(&UserInterfaceManager::instance(), SIGNAL(signal_cleanMaskRVEDone_T()),
            this, SLOT(_enableWidget()), Qt::QueuedConnection);

    connect(this,SIGNAL(signal_cleanUnMaskedRVE(float)),
            &UserInterfaceManager::instance(),
            SIGNAL(signal_cleanUnMaskedRVE_T(float)),
            Qt::QueuedConnection);
    connect(&UserInterfaceManager::instance(), SIGNAL(signal_cleanUnMaskedRVEDone_T()),
            this, SLOT(_enableWidget()), Qt::QueuedConnection);

    connect(this,SIGNAL(signal_applyTwoCutMaskInsideRVE(float,float)),
            &UserInterfaceManager::instance(),
            SIGNAL(signal_applyTwoCutMaskInsideRVE_T(float,float)),
            Qt::QueuedConnection);
    connect(&UserInterfaceManager::instance(), SIGNAL(signal_applyTwoCutMaskInsideRVEDone_T()),
            this, SLOT(_enableWidget()), Qt::QueuedConnection);

    connect(this,SIGNAL(signal_applyTwoCutMaskOutsideRVE(float,float)),
            &UserInterfaceManager::instance(),
            SIGNAL(signal_applyTwoCutMaskOutsideRVE_T(float,float)),
            Qt::QueuedConnection);
    connect(&UserInterfaceManager::instance(), SIGNAL(signal_applyTwoCutMaskOutsideRVEDone_T()),
            this, SLOT(_enableWidget()), Qt::QueuedConnection);

    connect(this,SIGNAL(signal_addRandomNoiseRVE()),
            &UserInterfaceManager::instance(),
            SIGNAL(signal_addRandomNoiseRVE_T()),
            Qt::QueuedConnection);
    connect(&UserInterfaceManager::instance(), SIGNAL(signal_addRandomNoiseRVEDone_T()),
            this, SLOT(_enableWidget()), Qt::QueuedConnection);

    connect(this,SIGNAL(signal_applyGaussianFilterRVE(int,float,float,float,bool,float,bool,float,float,float)),
            &UserInterfaceManager::instance(),
            SIGNAL(signal_applyGaussianFilterRVE_T(int,float,float,float,bool,float,bool,float,float,float)),
            Qt::QueuedConnection);
    connect(&UserInterfaceManager::instance(), SIGNAL(signal_applyGaussianFilterRVEDone_T()),
            this, SLOT(_enableWidget()), Qt::QueuedConnection);

    connect(this,SIGNAL(signal_generateOverlappingRandomEllipsoidsIntenseRVE(int,int,int,float,float,float,float,bool,float,float,float,float)),
            &UserInterfaceManager::instance(),
            SIGNAL(signal_generateOverlappingRandomEllipsoidsIntenseRVE_T(int,int,int,float,float,float,float,bool,float,float,float,float)),
            Qt::QueuedConnection);
    connect(&UserInterfaceManager::instance(), SIGNAL(signal_generateOverlappingRandomEllipsoidsIntenseRVEDone_T()),
            this, SLOT(_enableWidget()), Qt::QueuedConnection);

    connect(this,SIGNAL(signal_generateOverlappingRandomBezierCurveIntenseRVE(int,int,int,int,float,int,float,float,bool,float,float,float,float)),
            &UserInterfaceManager::instance(),
            SIGNAL(signal_generateOverlappingRandomBezierCurveIntenseRVE_T(int,int,int,int,float,int,float,float,bool,float,float,float,float)),
            Qt::QueuedConnection);
    connect(&UserInterfaceManager::instance(), SIGNAL(signal_generateOverlappingRandomBezierCurveIntenseRVEDone_T()),
            this, SLOT(_enableWidget()), Qt::QueuedConnection);

    connect(this,SIGNAL(signal_generateVoronoiRandomCellsRVE(int)),
            &UserInterfaceManager::instance(),
            SIGNAL(signal_generateVoronoiRandomCellsRVE_T(int)),
            Qt::QueuedConnection);
    connect(&UserInterfaceManager::instance(), SIGNAL(signal_generateVoronoiRandomCellsRVEDone_T()),
            this, SLOT(_enableWidget()), Qt::QueuedConnection);

    connect(this,SIGNAL(signal_generateLayerRVE(int, int, float)),
            &UserInterfaceManager::instance(),
            SIGNAL(signal_generateLayerRVE_T(int, int, float)),
            Qt::QueuedConnection);
    connect(&UserInterfaceManager::instance(), SIGNAL(signal_generateLayerRVEDone_T()),
            this, SLOT(_enableWidget()), Qt::QueuedConnection);

    ui->progressBar->setWindowModality(Qt::WindowModal);
    ui->progressBar->hide();
}

VolumeGLRenderRVEEditDialog::~VolumeGLRenderRVEEditDialog()
{
    if(_previewRender) delete _previewRender;
    delete ui;
}

void VolumeGLRenderRVEEditDialog::on_BottomCutLevelSlider_valueChanged(int value)
{
    if(value >= ui->TopCutLevelSlider->value())
        ui->TopCutLevelSlider->setValue(value+1);

    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_innerBottomCutLevel = value / 1000.0f;
    ui->BottomCutLevelLineEdit->setText(
                QString::number(ui->BottomCutLevelSlider->value() / 1000.0f));
    if(_previewRender)
    {
        _previewRender->doneCurrent();
        _parent->makeCurrent();
    }
    _parent->_loadRVEDataIntoTexture();
    _parent->updateGL();
}

void VolumeGLRenderRVEEditDialog::on_TopCutLevelSlider_valueChanged(int value)
{
    if(value <= ui->BottomCutLevelSlider->value())
        ui->BottomCutLevelSlider->setValue(value-1);

    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_innerTopCutLevel = value / 1000.0f;
    ui->TopCutLevelLineEdit->setText(
                QString::number(ui->TopCutLevelSlider->value() / 1000.0f));
    if(_previewRender)
    {
        _previewRender->doneCurrent();
        _parent->makeCurrent();
    }
    _parent->_loadRVEDataIntoTexture();
    _parent->updateGL();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_FilterRadiusSlider_valueChanged(int value)
{
    ui->FilterRadiusLineEdit->setText(QString::number(value));
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_FilterRadiusBackup = value;
    if(!(ui->FilterRadiusSlider->isSliderDown()))
        _updatePreview();
}
void UserInterface::VolumeGLRenderRVEEditDialog::on_FilterRadiusSlider_sliderReleased()
{
    _updatePreview();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_ScaleFactorXSlider_valueChanged(int value)
{
    ui->ScileFactorXLineEdit->setText(QString::number(value / 100.0f));
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_FilterScaleFactorXBackup = value / 100.0f;
    if(!ui->ScaleFactorXSlider->isSliderDown())
        _updatePreview();
}
void UserInterface::VolumeGLRenderRVEEditDialog::on_ScaleFactorXSlider_sliderReleased()
{
    _updatePreview();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_ScaleFactorYSlider_valueChanged(int value)
{
    ui->ScileFactorYLineEdit->setText(QString::number(value / 100.0f));
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_FilterScaleFactorYBackup = value / 100.0f;
    if(!ui->ScaleFactorYSlider->isSliderDown())
        _updatePreview();
}
void UserInterface::VolumeGLRenderRVEEditDialog::on_ScaleFactorYSlider_sliderReleased()
{
    _updatePreview();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_ScaleFactorZSlider_valueChanged(int value)
{
    ui->ScileFactorZLineEdit->setText(QString::number(value / 100.0f));
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_FilterScaleFactorZBackup = value / 100.0f;
    if(!ui->ScaleFactorZSlider->isSliderDown())
        _updatePreview();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_ScaleFactorZSlider_sliderReleased()
{
    _updatePreview();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_RotationOXSlider_valueChanged(int value)
{
    ui->RotationOXLineEdit->setText(QString::number(value));
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_FilterRotationOXBackup = value;
    if(!ui->RotationOXSlider->isSliderDown())
        _updatePreview();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_RotationOXSlider_sliderReleased()
{
    _updatePreview();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_RotationOYSlider_valueChanged(int value)
{
    ui->RotationOYLineEdit->setText(QString::number(value));
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_FilterRotationOYBackup = value;
    if(!ui->RotationOYSlider->isSliderDown())
        _updatePreview();
}
void UserInterface::VolumeGLRenderRVEEditDialog::on_RotationOYSlider_sliderReleased()
{
    _updatePreview();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_RotationOZSlider_valueChanged(int value)
{
    ui->RotationOZLineEdit->setText(QString::number(value));
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_FilterRotationOZBackup = value;
    if(!ui->RotationOZSlider->isSliderDown())
        _updatePreview();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_RotationOZSlider_sliderReleased()
{
    _updatePreview();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_ApplyGaussianFilterButton_clicked()
{
    _disableWigget();

    Q_EMIT signal_applyGaussianFilterRVE(
                ui->FilterRadiusSlider->value(),
                ui->ScaleFactorXSlider->value() / 100.0f,
                ui->ScaleFactorYSlider->value() / 100.0f,
                ui->ScaleFactorZSlider->value() / 100.0f,
                ui->UseDataAsIntensityCheckBox->checkState(),
                ui->IntensityFactorLineEdit->text().toFloat(),
                ui->UseRotationsCheckBox->checkState(),
                ui->RotationOXSlider->value(),
                ui->RotationOYSlider->value(),
                ui->RotationOZSlider->value());
}

float VolumeGLRenderRVEEditDialog::getFilterRadiusValue() const
{
    return ui->FilterRadiusSlider->value();
}

float VolumeGLRenderRVEEditDialog::getFilterScaleFactorXValue() const {
    return ui->ScaleFactorXSlider->value() / 100.0f;}

float VolumeGLRenderRVEEditDialog::getFilterScaleFactorYValue() const {
    return ui->ScaleFactorYSlider->value() / 100.0f;}

float VolumeGLRenderRVEEditDialog::getFilterScaleFactorZValue() const {
    return ui->ScaleFactorZSlider->value() / 100.0f;}

float VolumeGLRenderRVEEditDialog::getFilterRotationOXValue() const {
    return ui->RotationOXSlider->value();}

float VolumeGLRenderRVEEditDialog::getFilterRotationOYValue() const {
    return ui->RotationOYSlider->value();}

float VolumeGLRenderRVEEditDialog::getFilterRotationOZValue() const {
    return ui->RotationOZSlider->value();}

float VolumeGLRenderRVEEditDialog::getMinRadiusValue() const{
    return ui->MinRadiusSlider->value();}

float VolumeGLRenderRVEEditDialog::getMaxRadiusValue() const{
    return ui->MaxRadiusSlider->value();}

float UserInterface::VolumeGLRenderRVEEditDialog::getTransitionLayerValue() const{
    return ui->TransitionLayerSlider->value() / 100.0f; }

float VolumeGLRenderRVEEditDialog::getFilterScaleFactorXValue_Inclusion() const {
    return ui->ScaleFactorXSlider_Inclusion->value() / 100.0f;}

float VolumeGLRenderRVEEditDialog::getFilterScaleFactorYValue_Inclusion() const {
    return ui->ScaleFactorYSlider_Inclusion->value() / 100.0f;}

float VolumeGLRenderRVEEditDialog::getFilterScaleFactorZValue_Inclusion() const {
    return ui->ScaleFactorZSlider_Inclusion->value() / 100.0f;}

float VolumeGLRenderRVEEditDialog::getFilterRotationOXValue_Inclusion() const {
    return ui->RotationOXSlider_Inclusion->value();}

float VolumeGLRenderRVEEditDialog::getFilterRotationOYValue_Inclusion() const {
    return ui->RotationOYSlider_Inclusion->value();}

float VolumeGLRenderRVEEditDialog::getFilterRotationOZValue_Inclusion() const {
    return ui->RotationOZSlider_Inclusion->value();}

float VolumeGLRenderRVEEditDialog::getCoreIntensityValue() const{
    return ui->CoreIntensitySlider->value() / 100.0f;}

int UserInterface::VolumeGLRenderRVEEditDialog::getCurveOrder() const{
    return ui->BezierCurveOrderSlider->value();}

int UserInterface::VolumeGLRenderRVEEditDialog::getCurveApproximationPoints() const{
    return ui->ApproximationPointsSlider->value();}

int UserInterface::VolumeGLRenderRVEEditDialog::getFiberLength() const{
    return ui->FiberLengthSlider->value();}

float UserInterface::VolumeGLRenderRVEEditDialog::getMinimalScale() const{
    return ui->MinimalScaleSlider->value() / 100.0f;}

int UserInterface::VolumeGLRenderRVEEditDialog::getFiberRadius() const{
    return ui->FiberRadiusSlider->value();}

float UserInterface::VolumeGLRenderRVEEditDialog::getPathDeviation() const{
    return ui->PathDeviationSlider->value() / 100.0f;}

float UserInterface::VolumeGLRenderRVEEditDialog::getTransitionLayerValue_Fiber() const{
    return ui->TransitionLayerSlider_Fiber->value() / 100.0f;}

float UserInterface::VolumeGLRenderRVEEditDialog::getFilterRotationOXValue_Fiber() const{
    return ui->RotationOXSlider_Fiber->value();}

float UserInterface::VolumeGLRenderRVEEditDialog::getFilterRotationOYValue_Fiber() const{
    return ui->RotationOYSlider_Fiber->value();}

float UserInterface::VolumeGLRenderRVEEditDialog::getFilterRotationOZValue_Fiber() const{
    return ui->RotationOZSlider_Fiber->value();}

float UserInterface::VolumeGLRenderRVEEditDialog::getCoreIntensityValue_Fiber() const{
    return ui->CoreIntensitySlider_Fiber->value() / 100.0f;}

void UserInterface::VolumeGLRenderRVEEditDialog::on_UseDataAsIntensityCheckBox_stateChanged(
        int arg1)
{
    if(arg1 == Qt::Unchecked)
    {
        ui->IntensityFactorLineEdit->setEnabled(false);
        ui->IntensityFactorLabel->setEnabled(false);
    }
    else
    {
        ui->IntensityFactorLineEdit->setEnabled(true);
        ui->IntensityFactorLabel->setEnabled(true);
    }
}

void VolumeGLRenderRVEEditDialog::_disableWigget()
{
    this->setEnabled(false);

    ui->progressBar->setValue(0);
    ui->progressBar->show();
    QApplication::processEvents();
}

void VolumeGLRenderRVEEditDialog::_enableWidget()
{
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());

    _parent->makeCurrent();
    _parent->_loadRVEDataIntoTexture();
    _parent->updateInfoStringAndRepresentationSize();
    _parent->updateGL();

    ui->progressBar->setValue(100);
    ui->progressBar->hide();

    this->setEnabled(true);
    QApplication::processEvents();
}

void VolumeGLRenderRVEEditDialog::_updatePreview()
{
    if(_previewRender)
    {
        /// \todo some bug - those lines calls twice slider mouse click event
//        ui->progressBar->show();
//        ui->progressBar->setValue(0);
        this->setEnabled(false);
        QApplication::processEvents();
        _previewRender->loadDataIntoTexture();
        _previewRender->updateGL();
        this->setEnabled(true);
//        ui->progressBar->setValue(100);
//        ui->progressBar->hide();
        QApplication::processEvents();
    }
}

void VolumeGLRenderRVEEditDialog::_updatePreview_Inclusion()
{
    if(_previewRender_Inclusion)
    {
        /// \todo some bug - those lines calls twice slider mouse click event
//        ui->progressBar->show();
//        ui->progressBar->setValue(0);
        this->setEnabled(false);
        QApplication::processEvents();
        _previewRender_Inclusion->loadDataIntoTexture();
        _previewRender_Inclusion->updateGL();
        this->setEnabled(true);
//        ui->progressBar->setValue(100);
//        ui->progressBar->hide();
        QApplication::processEvents();
    }
}

void UserInterface::VolumeGLRenderRVEEditDialog::_updatePreview_Fiber()
{
    if(_previewRender_Fiber)
    {
        /// \todo some bug - those lines calls twice slider mouse click event
//        ui->progressBar->show();
//        ui->progressBar->setValue(0);
        this->setEnabled(false);
        QApplication::processEvents();
        _previewRender_Fiber->loadDataIntoTexture();
        _previewRender_Fiber->updateGL();
        this->setEnabled(true);
//        ui->progressBar->setValue(100);
//        ui->progressBar->hide();
        QApplication::processEvents();
    }
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_ClenRVEButton_clicked()
{
    _disableWigget();
    Q_EMIT signal_cleanRVE();
    ui->BottomCutLevelSlider->setValue(0);
    ui->TopCutLevelSlider->setValue(1000);
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_NormalizeRVEButton_clicked()
{
    _disableWigget();
    Q_EMIT signal_normalizeUnMaskedRVE();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_InvertRVEButton_clicked()
{
    _disableWigget();
    Q_EMIT signal_invertUnMaskedRVE();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_CleanMaskButton_clicked()
{
    _disableWigget();
    Q_EMIT signal_cleanMaskRVE();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_CleanUnMaskedElementsButton_clicked()
{
    _disableWigget();
    Q_EMIT signal_cleanUnMaskedRVE(ui->CleanUnMaskedElementsFillerLineEdit->text().toFloat());
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_ApplyTwoCutMaskInsideButton_clicked()
{
    _disableWigget();
    Q_EMIT signal_applyTwoCutMaskInsideRVE(
                ui->BottomCutLevelLineEdit->text().toFloat(),
                ui->TopCutLevelLineEdit->text().toFloat());
    ui->BottomCutLevelSlider->setValue(0);
    ui->TopCutLevelSlider->setValue(1000);
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_ApplyTwoCutMaskOutsideButton_clicked()
{
    _disableWigget();
    Q_EMIT signal_applyTwoCutMaskOutsideRVE(
                ui->BottomCutLevelLineEdit->text().toFloat(),
                ui->TopCutLevelLineEdit->text().toFloat());
    ui->BottomCutLevelSlider->setValue(0);
    ui->TopCutLevelSlider->setValue(1000);
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_addRandomNoiseButton_clicked()
{
    _disableWigget();
    Q_EMIT signal_addRandomNoiseRVE();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_GenerateVoronoiCellsButton_clicked()
{
    _disableWigget();

    Q_EMIT signal_generateVoronoiRandomCellsRVE(
                ui->NumberOfCellsLineEdit->text().toInt());
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_UseRotationsCheckBox_stateChanged(
        int arg1)
{
    if(arg1 == Qt::Unchecked)
    {
        ui->RotationOXLabel->setEnabled(false);
        ui->RotationOXLineEdit->setEnabled(false);
        ui->RotationOXSlider->setEnabled(false);

        ui->RotationOYLabel->setEnabled(false);
        ui->RotationOYLineEdit->setEnabled(false);
        ui->RotationOYSlider->setEnabled(false);

        ui->RotationOZLabel->setEnabled(false);
        ui->RotationOZLineEdit->setEnabled(false);
        ui->RotationOZSlider->setEnabled(false);
    }
    else
    {
        ui->RotationOXLabel->setEnabled(true);
        ui->RotationOXLineEdit->setEnabled(true);
        ui->RotationOXSlider->setEnabled(true);

        ui->RotationOYLabel->setEnabled(true);
        ui->RotationOYLineEdit->setEnabled(true);
        ui->RotationOYSlider->setEnabled(true);

        ui->RotationOZLabel->setEnabled(true);
        ui->RotationOZLineEdit->setEnabled(true);
        ui->RotationOZSlider->setEnabled(true);
    }
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_MinRadiusSlider_valueChanged(int value)
{
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_MinRadiusBackup = value;

    if(value > ui->MaxRadiusSlider->value())
        ui->MaxRadiusSlider->setValue(value);

    ui->MinRadiusLineEdit->setText(QString::number(value));
    if(!(ui->MinRadiusSlider->isSliderDown()))
        _updatePreview_Inclusion();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_MinRadiusSlider_sliderReleased()
{
        _updatePreview_Inclusion();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_MaxRadiusSlider_valueChanged(int value)
{
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_MaxRadiusBackup = value;

    if(value < ui->MinRadiusSlider->value())
        ui->MinRadiusSlider->setValue(value);

    ui->MaxRadiusLineEdit->setText(QString::number(value));
    if(!(ui->MaxRadiusSlider->isSliderDown()))
        _updatePreview_Inclusion();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_MaxRadiusSlider_sliderReleased()
{
        _updatePreview_Inclusion();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_TransitionLayerSlider_valueChanged(int value)
{
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_TransitionLayerBackup = value / 100.0f;

    ui->TransitionLayerLineEdit->setText(QString::number(value / 100.0f));
    if(!(ui->TransitionLayerSlider->isSliderDown()))
        _updatePreview_Inclusion();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_TransitionLayerSlider_sliderReleased()
{
        _updatePreview_Inclusion();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_ScaleFactorXSlider_Inclusion_valueChanged(int value)
{
    ui->ScileFactorXLineEdit_Inclusion->setText(QString::number(value / 100.0f));
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_FilterScaleFactorXBackup = value / 100.0f;
    if(!ui->ScaleFactorXSlider_Inclusion->isSliderDown())
        _updatePreview_Inclusion();
}
void UserInterface::VolumeGLRenderRVEEditDialog::on_ScaleFactorXSlider_Inclusion_sliderReleased()
{
    _updatePreview_Inclusion();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_ScaleFactorYSlider_Inclusion_valueChanged(int value)
{
    ui->ScileFactorYLineEdit_Inclusion->setText(QString::number(value / 100.0f));
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_FilterScaleFactorYBackup = value / 100.0f;
    if(!ui->ScaleFactorYSlider_Inclusion->isSliderDown())
        _updatePreview_Inclusion();
}
void UserInterface::VolumeGLRenderRVEEditDialog::on_ScaleFactorYSlider_Inclusion_sliderReleased()
{
    _updatePreview_Inclusion();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_ScaleFactorZSlider_Inclusion_valueChanged(int value)
{
    ui->ScileFactorZLineEdit_Inclusion->setText(QString::number(value / 100.0f));
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_FilterScaleFactorZBackup = value / 100.0f;
    if(!ui->ScaleFactorZSlider_Inclusion->isSliderDown())
        _updatePreview_Inclusion();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_ScaleFactorZSlider_Inclusion_sliderReleased()
{
    _updatePreview_Inclusion();
}
void UserInterface::VolumeGLRenderRVEEditDialog::on_UseRandomOrientationCheckBox_stateChanged(
        int arg1)
{
    if(!(arg1 == Qt::Unchecked))
    {
        ui->RotationOXLabel_Inclusion->setEnabled(false);
        ui->RotationOXLineEdit_Inclusion->setEnabled(false);
        ui->RotationOXSlider_Inclusion->setEnabled(false);

        ui->RotationOYLabel_Inclusion->setEnabled(false);
        ui->RotationOYLineEdit_Inclusion->setEnabled(false);
        ui->RotationOYSlider_Inclusion->setEnabled(false);

        ui->RotationOZLabel_Inclusion->setEnabled(false);
        ui->RotationOZLineEdit_Inclusion->setEnabled(false);
        ui->RotationOZSlider_Inclusion->setEnabled(false);
    }
    else
    {
        ui->RotationOXLabel_Inclusion->setEnabled(true);
        ui->RotationOXLineEdit_Inclusion->setEnabled(true);
        ui->RotationOXSlider_Inclusion->setEnabled(true);

        ui->RotationOYLabel_Inclusion->setEnabled(true);
        ui->RotationOYLineEdit_Inclusion->setEnabled(true);
        ui->RotationOYSlider_Inclusion->setEnabled(true);

        ui->RotationOZLabel_Inclusion->setEnabled(true);
        ui->RotationOZLineEdit_Inclusion->setEnabled(true);
        ui->RotationOZSlider_Inclusion->setEnabled(true);
    }
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_RotationOXSlider_Inclusion_valueChanged(int value)
{
    ui->RotationOXLineEdit_Inclusion->setText(QString::number(value));
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_FilterRotationOXBackup = value;
    if(!ui->RotationOXSlider_Inclusion->isSliderDown())
        _updatePreview_Inclusion();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_RotationOXSlider_Inclusion_sliderReleased()
{
    _updatePreview_Inclusion();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_RotationOYSlider_Inclusion_valueChanged(int value)
{
    ui->RotationOYLineEdit_Inclusion->setText(QString::number(value));
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_FilterRotationOYBackup = value;
    if(!ui->RotationOYSlider_Inclusion->isSliderDown())
        _updatePreview_Inclusion();
}
void UserInterface::VolumeGLRenderRVEEditDialog::on_RotationOYSlider_Inclusion_sliderReleased()
{
    _updatePreview_Inclusion();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_RotationOZSlider_Inclusion_valueChanged(int value)
{
    ui->RotationOZLineEdit_Inclusion->setText(QString::number(value));
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_FilterRotationOZBackup = value;
    if(!ui->RotationOZSlider_Inclusion->isSliderDown())
        _updatePreview_Inclusion();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_RotationOZSlider_Inclusion_sliderReleased()
{
    _updatePreview_Inclusion();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_CoreIntensitySlider_valueChanged(int value)
{
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_CoreIntensityBackup = value / 100.0f;

    ui->CoreIntensityLineEdit->setText(QString::number(value / 100.0f));
    if(!(ui->CoreIntensitySlider->isSliderDown()))
        _updatePreview_Inclusion();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_CoreIntensitySlider_sliderReleased()
{
    _updatePreview_Inclusion();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_GenerateInclusionsButton_clicked()
{
    _disableWigget();

    Q_EMIT signal_generateOverlappingRandomEllipsoidsIntenseRVE(
                ui->NumberOfInclusionsLineEdit->text().toInt(),
                ui->MinRadiusSlider->value(),
                ui->MaxRadiusSlider->value(),
                ui->TransitionLayerSlider->value() / 100.0f,
                ui->ScaleFactorXSlider_Inclusion->value() / 100.0f,
                ui->ScaleFactorYSlider_Inclusion->value() / 100.0f,
                ui->ScaleFactorZSlider_Inclusion->value() / 100.0f,
                ui->UseRandomOrientationCheckBox->checkState(),
                ui->RotationOXSlider_Inclusion->value(),
                ui->RotationOYSlider_Inclusion->value(),
                ui->RotationOZSlider_Inclusion->value(),
                ui->CoreIntensitySlider->value() / 100.0f);
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_LoadRVEButton_clicked()
{
    _disableWigget();

    Q_EMIT signal_loadRVE(
                QFileDialog::getOpenFileName(
                    this, tr("Load RVE"), tr(""), tr("RVE Files (*.RVE)")));
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_SaveRVEButton_clicked()
{
    _disableWigget();

    Q_EMIT signal_saveRVE(
                QFileDialog::getSaveFileName(
                    this, tr("Save RVE"), tr(""), tr("RVE Files (*.RVE)")));
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_BezierCurveOrderSlider_valueChanged(int value)
{
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_CurveOrderBackup = value;

    ui->BezierCurveOrderLineEdit->setText(QString::number(value));
    if(!(ui->BezierCurveOrderSlider->isSliderDown()))
        _updatePreview_Fiber();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_BezierCurveOrderSlider_sliderReleased()
{
    _updatePreview_Fiber();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_ApproximationPointsSlider_valueChanged(int value)
{
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_CurveApproximationPointsBackup = value;

    ui->ApproximationPointsLineEdit->setText(QString::number(value));
    if(!(ui->ApproximationPointsSlider->isSliderDown()))
        _updatePreview_Fiber();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_ApproximationPointsSlider_sliderReleased()
{
    _updatePreview_Fiber();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_FiberLengthSlider_valueChanged(int value)
{
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_FiberLengthBackup = value;

    ui->FiberLengthLineEdit->setText(QString::number(value));
    if(!(ui->FiberLengthSlider->isSliderDown()))
        _updatePreview_Fiber();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_FiberLengthSlider_sliderReleased()
{
    _updatePreview_Fiber();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_MinimalScaleSlider_valueChanged(int value)
{
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_MinScaleBackup = value / 100.0f;

    ui->MinimalScaleLineEdit->setText(QString::number(value / 100.0f));
    if(!(ui->MinimalScaleSlider->isSliderDown()))
        _updatePreview_Fiber();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_MinimalScaleSlider_sliderReleased()
{
    _updatePreview_Fiber();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_FiberRadiusSlider_valueChanged(int value)
{
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_MaxRadiusBackup = value;
    _parent->_MinRadiusBackup = value;

    ui->FiberRadiusLineEdit->setText(QString::number(value));
    if(!(ui->FiberRadiusSlider->isSliderDown()))
        _updatePreview_Fiber();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_FiberRadiusSlider_sliderReleased()
{
    _updatePreview_Fiber();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_PathDeviationSlider_valueChanged(int value)
{
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_PathDeviationBackup = value / 100.0f;

    ui->PathDeviationLineEdit->setText(QString::number(value / 100.0f));
    if(!(ui->PathDeviationSlider->isSliderDown()))
        _updatePreview_Fiber();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_PathDeviationSlider_sliderReleased()
{
    _updatePreview_Fiber();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_TransitionLayerSlider_Fiber_valueChanged(int value)
{
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_TransitionLayerBackup = value / 100.0f;

    ui->TransitionLayerLineEdit_Fiber->setText(QString::number(value / 100.0f));
    if(!(ui->TransitionLayerSlider_Fiber->isSliderDown()))
        _updatePreview_Fiber();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_TransitionLayerSlider_Fiber_sliderReleased()
{
    _updatePreview_Fiber();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_RotationOXSlider_Fiber_valueChanged(int value)
{
    ui->RotationOXLineEdit_Fiber->setText(QString::number(value));
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_FilterRotationOXBackup = value;
    if(!ui->RotationOXSlider_Fiber->isSliderDown())
        _updatePreview_Fiber();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_RotationOXSlider_Fiber_sliderReleased()
{
    _updatePreview_Fiber();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_RotationOYSlider_Fiber_valueChanged(int value)
{
    ui->RotationOYLineEdit_Fiber->setText(QString::number(value));
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_FilterRotationOYBackup = value;
    if(!ui->RotationOYSlider_Fiber->isSliderDown())
        _updatePreview_Fiber();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_RotationOYSlider_Fiber_sliderReleased()
{
    _updatePreview_Fiber();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_RotationOZSlider_Fiber_valueChanged(int value)
{
    ui->RotationOZLineEdit_Fiber->setText(QString::number(value));
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_FilterRotationOZBackup = value;
    if(!ui->RotationOZSlider_Fiber->isSliderDown())
        _updatePreview_Fiber();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_RotationOZSlider_Fiber_sliderReleased()
{
    _updatePreview_Fiber();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_CoreIntensitySlider_Fiber_valueChanged(int value)
{
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_CoreIntensityBackup = value / 100.0f;

    ui->CoreIntensityLineEdit_Fiber->setText(QString::number(value / 100.0f));
    if(!(ui->CoreIntensitySlider_Fiber->isSliderDown()))
        _updatePreview_Fiber();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_CoreIntensitySlider_Fiber_sliderReleased()
{
    _updatePreview_Fiber();
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_UseRandomOrientationCheckBox_Fiber_stateChanged(int arg1)
{
    if(!(arg1 == Qt::Unchecked))
    {
        ui->RotationOXLabel_Fiber->setEnabled(false);
        ui->RotationOXLineEdit_Fiber->setEnabled(false);
        ui->RotationOXSlider_Fiber->setEnabled(false);

        ui->RotationOYLabel_Fiber->setEnabled(false);
        ui->RotationOYLineEdit_Fiber->setEnabled(false);
        ui->RotationOYSlider_Fiber->setEnabled(false);

        ui->RotationOZLabel_Fiber->setEnabled(false);
        ui->RotationOZLineEdit_Fiber->setEnabled(false);
        ui->RotationOZSlider_Fiber->setEnabled(false);
    }
    else
    {
        ui->RotationOXLabel_Fiber->setEnabled(true);
        ui->RotationOXLineEdit_Fiber->setEnabled(true);
        ui->RotationOXSlider_Fiber->setEnabled(true);

        ui->RotationOYLabel_Fiber->setEnabled(true);
        ui->RotationOYLineEdit_Fiber->setEnabled(true);
        ui->RotationOYSlider_Fiber->setEnabled(true);

        ui->RotationOZLabel_Fiber->setEnabled(true);
        ui->RotationOZLineEdit_Fiber->setEnabled(true);
        ui->RotationOZSlider_Fiber->setEnabled(true);
    }
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_GenerateInclusionsButton_Fiber_clicked()
{
    _disableWigget();

    Q_EMIT signal_generateOverlappingRandomBezierCurveIntenseRVE(
                ui->NumberOfFibersLineEdit->text().toInt(),
                ui->BezierCurveOrderSlider->value(),
                ui->ApproximationPointsSlider->value(),
                ui->FiberLengthSlider->value(),
                ui->MinimalScaleSlider->value() / 100.0f,
                ui->FiberRadiusSlider->value(),
                ui->PathDeviationSlider->value() / 100.0f,
                ui->TransitionLayerSlider_Fiber->value() / 100.0f,
                ui->UseRandomOrientationCheckBox_Fiber->checkState(),
                ui->RotationOXSlider_Fiber->value(),
                ui->RotationOYSlider_Fiber->value(),
                ui->RotationOZSlider_Fiber->value(),
                ui->CoreIntensitySlider_Fiber->value() / 100.0f);
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_LayerBottomSideSlider_Layer_valueChanged(int value)
{
    ui->LayerBottomSideLineEdit_Layer->setText(QString::number(value));
    if(value >= ui->LayerTopSideSlider_Layer->value())
        ui->LayerTopSideSlider_Layer->setValue(value+1);
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_LayerTopSideSlider_Layer_valueChanged(int value)
{
    ui->LayerTopSideLineEdit_Layer->setText(QString::number(value));
    if(value <= ui->LayerBottomSideSlider_Layer->value())
        ui->LayerBottomSideSlider_Layer->setValue(value-1);
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_CoreIntensitySlider_Layer_valueChanged(int value)
{
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_CoreIntensityBackup = value / 100.0f;

    ui->CoreIntensityLineEdit_Layer->setText(QString::number(value / 100.0f));
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_addLayerButton_Layer_clicked()
{
    _disableWigget();

    Q_EMIT signal_generateLayerRVE(
                ui->LayerBottomSideSlider_Layer->value(),
                ui->LayerTopSideSlider_Layer->value(),
                ui->CoreIntensitySlider_Layer->value() / 100.0f);
}
