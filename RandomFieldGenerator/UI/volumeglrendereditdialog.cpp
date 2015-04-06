#include "volumeglrendereditdialog.h"
#include "ui_volumeglrendereditdialog.h"

#include "volumeglrender.h"

using namespace UserInterface;

VolumeGLRenderEditDialog::VolumeGLRenderEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VolumeGLRenderEditDialog)
{
    ui->setupUi(this);

    VolumeGLRender* _parent = static_cast<VolumeGLRender*>(this->parent());

    ui->PotentialFieldTransparencySlider->setValue(_parent->_potentialFieldAlphaLevel);

    ui->MaxPotentialFieldValueSlider->setValue(
                (_parent->_maxPotentialValue - _parent->_minPotentialValueBackup) /
                (_parent->_maxPotentialValueBackup - _parent->_minPotentialValueBackup) * 100.0f);
    ui->MinPotentialFieldValueSlider->setValue(
                (_parent->_minPotentialValue - _parent->_minPotentialValueBackup) /
                (_parent->_maxPotentialValueBackup - _parent->_minPotentialValueBackup) * 100.0f);
}

VolumeGLRenderEditDialog::~VolumeGLRenderEditDialog()
{
    delete ui;
}

void VolumeGLRenderEditDialog::on_PotentialFieldTransparencySlider_valueChanged(int value)
{
    VolumeGLRender* _parent = static_cast<VolumeGLRender*>(this->parent());
    _parent->_potentialFieldAlphaLevel = value;
    _parent->_loadPotentialFieldIntoTexture();
    _parent->updateGL();
}

void VolumeGLRenderEditDialog::on_MaxPotentialFieldValueSlider_valueChanged(int value)
{
    if(value <= ui->MinPotentialFieldValueSlider->value())
        ui->MinPotentialFieldValueSlider->setValue(value-1);

    VolumeGLRender* _parent = static_cast<VolumeGLRender*>(this->parent());
    _parent->_maxPotentialValue = _parent->_minPotentialValueBackup + value / 100.0f *
            (_parent->_maxPotentialValueBackup - _parent->_minPotentialValueBackup);
    _parent->_loadPotentialFieldIntoTexture();
    _parent->updateGL();
}

void VolumeGLRenderEditDialog::on_MinPotentialFieldValueSlider_valueChanged(int value)
{
    if(value >= ui->MaxPotentialFieldValueSlider->value())
        ui->MaxPotentialFieldValueSlider->setValue(value+1);

    VolumeGLRender* _parent = static_cast<VolumeGLRender*>(this->parent());
    _parent->_minPotentialValue = _parent->_minPotentialValueBackup + value / 100.0f *
            (_parent->_maxPotentialValueBackup - _parent->_minPotentialValueBackup);
    _parent->_loadPotentialFieldIntoTexture();
    _parent->updateGL();
}
