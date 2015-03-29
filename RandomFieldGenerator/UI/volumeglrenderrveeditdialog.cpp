#include "volumeglrenderrveeditdialog.h"
#include "ui_volumeglrenderrveeditdialog.h"

#include "volumeglrenderrve.h"

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

    _previewRender = new FilterPreviewGLRender(
                _parent->_ptrToRVE, ui->groupBox);
    _previewRender->resize(261,261);
    _previewRender->move(510, 20);
    //_previewRender->move(520, 90);

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
    if(_previewRender)
    {
        _previewRender->loadDataIntoTexture();
        _previewRender->updateGL();
    }
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_ScaleFactorXSlider_valueChanged(int value)
{
    ui->ScileFactorXLineEdit->setText(QString::number(value / 100.0f));
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_FilterScaleFactorXBackup = value / 100.0f;
    if(_previewRender)
    {
        _previewRender->loadDataIntoTexture();
        _previewRender->updateGL();
    }
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_ScaleFactorYSlider_valueChanged(int value)
{
    ui->ScileFactorYLineEdit->setText(QString::number(value / 100.0f));
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_FilterScaleFactorYBackup = value / 100.0f;
    if(_previewRender)
    {
        _previewRender->loadDataIntoTexture();
        _previewRender->updateGL();
    }
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_ScaleFactorZSlider_valueChanged(int value)
{
    ui->ScileFactorZLineEdit->setText(QString::number(value / 100.0f));
    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());
    _parent->_FilterScaleFactorZBackup = value / 100.0f;
    if(_previewRender)
    {
        _previewRender->loadDataIntoTexture();
        _previewRender->updateGL();
    }
}

void UserInterface::VolumeGLRenderRVEEditDialog::on_GenerateRandomFieldPushButton_clicked()
{
    /// \todo bind console command

    VolumeGLRenderRVE* _parent = static_cast<VolumeGLRenderRVE*>(this->parent());

    ui->progressBar->show();
    ui->progressBar->setValue(0);

    this->blockSignals(true);
    _parent->_ptrToRVE->generateRandomField();
    _parent->_ptrToRVE->applyGaussianFilterCL(
                ui->FilterRadiusSlider->value(),
                ui->ScaleFactorXSlider->value() / 100.0f,
                ui->ScaleFactorYSlider->value() / 100.0f,
                ui->ScaleFactorZSlider->value() / 100.0f);
    this->blockSignals(false);

    _previewRender->doneCurrent();
    _parent->makeCurrent();
    _parent->_loadRVEDataIntoTexture();
    _parent->updateGL();

    ui->progressBar->setValue(100);
    ui->progressBar->hide();
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
