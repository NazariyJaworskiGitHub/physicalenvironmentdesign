#include "volumeglrenderformatdialog.h"
#include "ui_volumeglrenderformatdialog.h"

#include "volumeglrenderbasecontroller.h"

#include "QColorDialog"

using namespace UserInterface;

VolumeGLRenderFormatDialog::VolumeGLRenderFormatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VolumeGLRenderFormatDialog)
{
    ui->setupUi(this);

    VolumeGLRenderBaseController* _parent = static_cast<
            VolumeGLRenderBaseController*>(this->parent());

    ui->BackgroundColorButton->setStyleSheet(
                "background-color: " + _parent->_BackgroundColor.name(QColor::HexArgb));

    ui->TextColorButton->setStyleSheet(
                "background-color: " + _parent->_TextColor.name(QColor::HexArgb));

    ui->TextFontComboBox->setCurrentFont(_parent->_TextFont);
    ui->TextFontSizeSpinBox->setValue(QFontInfo(_parent->_TextFont).pointSize());
}

VolumeGLRenderFormatDialog::~VolumeGLRenderFormatDialog()
{
    delete ui;
}

void VolumeGLRenderFormatDialog::on_BackgroundColorButton_clicked()
{
    VolumeGLRenderBaseController* _parent = static_cast<
            VolumeGLRenderBaseController*>(this->parent());
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
    VolumeGLRenderBaseController* _parent = static_cast<
            VolumeGLRenderBaseController*>(this->parent());
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
    VolumeGLRenderBaseController* _parent = static_cast<
            VolumeGLRenderBaseController*>(this->parent());
    _parent->_TextFont = font;
    _parent->updateGL();
}

void VolumeGLRenderFormatDialog::on_TextFontSizeSpinBox_valueChanged(int value)
{
    VolumeGLRenderBaseController* _parent = static_cast<
            VolumeGLRenderBaseController*>(this->parent());
    _parent->_TextFont.setPointSize(value);
    _parent->updateGL();
}
