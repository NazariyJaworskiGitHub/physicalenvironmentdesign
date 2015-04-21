#include "volumeglrenderrveeditdialog.h"
#include "ui_volumeglrenderrveeditdialog.h"

#include "volumeglrenderrve.h"

#include "userinterfacemanager.h"

#include <QDoubleValidator>
#include <QIntValidator>

using namespace UserInterface;

VolumeGLRenderRVEEditDialog::VolumeGLRenderRVEEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VolumeGLRenderRVEEditDialog)
{
    ui->setupUi(this);

    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());

    ui->BottomCutLevelSlider->setValue(_parent->_innerBottomCutLevel * 1000);
    ui->BottomCutLevelLineEdit->setText(
                QString::number(ui->BottomCutLevelSlider->value() / 1000.0f));

    ui->TopCutLevelSlider->setValue(_parent->_innerTopCutLevel * 1000);
    ui->TopCutLevelLineEdit->setText(
                QString::number(ui->TopCutLevelSlider->value() / 1000.0f));

    ui->FilterRadiusSlider->setMaximum(_parent->_ptrToRVE->getSize());
    ui->FilterRadiusSlider->setValue(_parent->_ptrToRVE->getDiscreteRadius());
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

    _previewRender = new FilterPreviewGLRender(
                _parent->_ptrToRVE, ui->GaussianFilter);
    _previewRender->resize(261,261);
    _previewRender->move(510, 20);

    QIntValidator *_intValidator = new QIntValidator(this);
    _intValidator->setBottom(2);
    ui->NumberOfCellsLineEdit->setValidator(_intValidator);

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

    connect(this,SIGNAL(signal_applyGaussianFilterRVE(int,float,float,float,float,float,float,bool,float)),
            &UserInterfaceManager::instance(),
            SIGNAL(signal_applyGaussianFilterRVE_T(int,float,float,float,float,float,float,bool,float)),
            Qt::QueuedConnection);
    connect(&UserInterfaceManager::instance(), SIGNAL(signal_applyGaussianFilterRVEDone_T()),
            this, SLOT(_enableWidget()), Qt::QueuedConnection);

    connect(this,SIGNAL(signal_generateVoronoiRandomCellsRVE(int)),
            &UserInterfaceManager::instance(),
            SIGNAL(signal_generateVoronoiRandomCellsRVE_T(int)),
            Qt::QueuedConnection);
    connect(&UserInterfaceManager::instance(), SIGNAL(signal_generateVoronoiRandomCellsRVEDone_T()),
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
                ui->RotationOXSlider->value(),
                ui->RotationOYSlider->value(),
                ui->RotationOZSlider->value(),
                ui->UseDataAsIntensityCheckBox->checkState(),
                ui->IntensityFactorLineEdit->text().toFloat());
}

float VolumeGLRenderRVEEditDialog::getFilterRadiusValue() const
{
    return ui->FilterRadiusSlider->value();
}

float UserInterface::VolumeGLRenderRVEEditDialog::getFilterScaleFactorXValue() const {
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

void UserInterface::VolumeGLRenderRVEEditDialog::on_ClenRVEButton_clicked()
{
    _disableWigget();
    Q_EMIT signal_cleanRVE();
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
