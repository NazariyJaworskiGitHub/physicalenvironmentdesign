#include "volumeglrenderformatdialog.h"
#include "ui_volumeglrenderformatdialog.h"

#include "volumeglrender.h"

using namespace UserInterface;

VolumeGLRenderFormatDialog::VolumeGLRenderFormatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VolumeGLRenderFormatDialog)
{
    ui->setupUi(this);

    UserInterface::VolumeGLRender* _parent =
            static_cast<UserInterface::VolumeGLRender*>(this->parent());
    ui->PotentialFieldTransparencySlider->setValue(_parent->_potentialFieldAlphaLevel);
    ui->PotentialFieldTransparencyLineEdit->setText(
                QString::number(_parent->_potentialFieldAlphaLevel));

    setModal(true);
    setWindowOpacity(0.75);
}

VolumeGLRenderFormatDialog::~VolumeGLRenderFormatDialog()
{
    delete ui;
}

void VolumeGLRenderFormatDialog::on_PotentialFieldTransparencySlider_valueChanged(int value)
{
    ui->PotentialFieldTransparencyLineEdit->setText(QString::number(value));

    UserInterface::VolumeGLRender* _parent =
            static_cast<UserInterface::VolumeGLRender*>(this->parent());
    _parent->_potentialFieldAlphaLevel = value;
    _parent->_loadPotentialFieldIntoTexture();
    _parent->updateGL();
}
