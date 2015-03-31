#include "volumeglrenderformatdialog.h"
#include "ui_volumeglrenderformatdialog.h"

#include "volumeglrender.h"

#include "QColorDialog"

using namespace UserInterface;

VolumeGLRenderFormatDialog::VolumeGLRenderFormatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VolumeGLRenderFormatDialog)
{
    ui->setupUi(this);

    VolumeGLRender* _parent = static_cast<VolumeGLRender*>(this->parent());

    ui->PotentialFieldTransparencySlider->setValue(_parent->_potentialFieldAlphaLevel);

    ui->BackgroundColorButton->setStyleSheet(
                "background-color: " + _parent->_BackgroundColor.name(QColor::HexArgb));

    ui->TextColorButton->setStyleSheet(
                "background-color: " + _parent->_TextColor.name(QColor::HexArgb));

    ui->TextFontComboBox->setCurrentFont(_parent->_TextFont);
    ui->TextFontSizeSpinBox->setValue(QFontInfo(_parent->_TextFont).pointSize());

//    /// \warning setup global Locale for correct usage
//    _validator = new QDoubleValidator(
//                _parent->_minPotentialValue,
//                _parent->_maxPotentialValue,
//                100,
//                this);
//    _validator->setNotation(QDoubleValidator::ScientificNotation);

    ui->MaxPotentialFieldValueSlider->setValue(
                (_parent->_maxPotentialValue - _parent->_minPotentialValueBackup) /
                (_parent->_maxPotentialValueBackup - _parent->_minPotentialValueBackup) * 100.0f);
    ui->MinPotentialFieldValueSlider->setValue(
                (_parent->_minPotentialValue - _parent->_minPotentialValueBackup) /
                (_parent->_maxPotentialValueBackup - _parent->_minPotentialValueBackup) * 100.0f);

    //setWindowOpacity(0.75);
}

VolumeGLRenderFormatDialog::~VolumeGLRenderFormatDialog()
{
    delete ui;
}

void VolumeGLRenderFormatDialog::on_PotentialFieldTransparencySlider_valueChanged(int value)
{
    VolumeGLRender* _parent = static_cast<VolumeGLRender*>(this->parent());
    _parent->_potentialFieldAlphaLevel = value;
    _parent->_loadPotentialFieldIntoTexture();
    _parent->updateGL();
}

void VolumeGLRenderFormatDialog::on_BackgroundColorButton_clicked()
{
    VolumeGLRender* _parent = static_cast<VolumeGLRender*>(this->parent());
    QColor _color = QColorDialog::getColor(
                _parent->_BackgroundColor,
                this,
                "Select background color");
    if(_color.isValid())
    {
        _parent->_BackgroundColor = _color;
        ui->BackgroundColorButton->setStyleSheet(
                    "background-color: " + _color.name(QColor::HexRgb));
        _parent->updateGL();
    }
}

void VolumeGLRenderFormatDialog::on_TextColorButton_clicked()
{
    VolumeGLRender* _parent = static_cast<VolumeGLRender*>(this->parent());
    QColor _color = QColorDialog::getColor(
                _parent->_TextColor,
                this,
                "Select text color",
                QColorDialog::ShowAlphaChannel);
    if(_color.isValid())
    {
        _parent->_TextColor = _color;
        ui->TextColorButton->setStyleSheet(
                    "background-color: " + _color.name(QColor::HexArgb));
        _parent->updateGL();
    }
}

void VolumeGLRenderFormatDialog::on_TextFontComboBox_currentFontChanged(const QFont &font)
{
    VolumeGLRender* _parent = static_cast<VolumeGLRender*>(this->parent());
    _parent->_TextFont = font;
    _parent->updateGL();
}

void VolumeGLRenderFormatDialog::on_TextFontSizeSpinBox_valueChanged(int value)
{
    VolumeGLRender* _parent = static_cast<VolumeGLRender*>(this->parent());
    _parent->_TextFont.setPointSize(value);
    _parent->updateGL();
}

void VolumeGLRenderFormatDialog::on_MaxPotentialFieldValueSlider_valueChanged(int value)
{
    if(value <= ui->MinPotentialFieldValueSlider->value())
        ui->MinPotentialFieldValueSlider->setValue(value-1);

    VolumeGLRender* _parent = static_cast<VolumeGLRender*>(this->parent());
    _parent->_maxPotentialValue = _parent->_minPotentialValueBackup + value / 100.0f *
            (_parent->_maxPotentialValueBackup - _parent->_minPotentialValueBackup);
    _parent->_loadPotentialFieldIntoTexture();
    _parent->updateGL();
}

void VolumeGLRenderFormatDialog::on_MinPotentialFieldValueSlider_valueChanged(int value)
{
    if(value >= ui->MaxPotentialFieldValueSlider->value())
        ui->MaxPotentialFieldValueSlider->setValue(value+1);

    VolumeGLRender* _parent = static_cast<VolumeGLRender*>(this->parent());
    _parent->_minPotentialValue = _parent->_minPotentialValueBackup + value / 100.0f *
            (_parent->_maxPotentialValueBackup - _parent->_minPotentialValueBackup);
    _parent->_loadPotentialFieldIntoTexture();
    _parent->updateGL();
}
