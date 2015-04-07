#include "xyglrenderformatdialog.h"
#include "ui_xyglrenderformatdialog.h"

#include "xyglrender.h"

#include <QColorDialog>

using namespace UserInterface;

XYGLRenderFormatDialog::XYGLRenderFormatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XYGLRenderFormatDialog)
{
    ui->setupUi(this);

    XYGLRender* _parent = static_cast<XYGLRender*>(this->parent());

    ui->BackgroundColorButton->setStyleSheet(
                "background-color: " + _parent->_BackgroundColor.name(QColor::HexArgb));

    ui->TextColorButton->setStyleSheet(
                "background-color: " + _parent->_TextColor.name(QColor::HexArgb));

    ui->TextFontComboBox->setCurrentFont(_parent->_TextFont);
    ui->TextFontSizeSpinBox->setValue(QFontInfo(_parent->_TextFont).pointSize());

    ui->GridColorButton->setStyleSheet(
                "background-color: " + _parent->_gridColor.name(QColor::HexArgb));

    ui->NumberOfAxisSpinBox->setValue(_parent->_OXLinesNum);

    ui->NumberPlotNodesSpinBox->setValue(_parent->_nodesNum);

    // Prepare table (10, 160)
    for(int i=0; i<_parent->_functions.size(); ++i)
    {
        QLabel *_functionLable = new QLabel("todo Function lable", this);
        _functionLable->setGeometry(QRect(10, 190 + i* 30, 141, 21));

        _colorButtons.push_back(new QPushButton(this));
        _colorButtons[i]->setStyleSheet(
                    "background-color: " + _parent->_functionColors[i].name(QColor::HexArgb));
        _colorButtons[i]->setGeometry(QRect(160, 190 + i* 30, 311, 21));

        connect(_colorButtons[i], SIGNAL(clicked()),
            this, SLOT(functionColorButtonClicked()));
    }
}

XYGLRenderFormatDialog::~XYGLRenderFormatDialog()
{
    delete ui;
}

void XYGLRenderFormatDialog::on_BackgroundColorButton_clicked()
{
    XYGLRender* _parent = static_cast<
            XYGLRender*>(this->parent());
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

void XYGLRenderFormatDialog::on_TextColorButton_clicked()
{
    XYGLRender* _parent = static_cast<
            XYGLRender*>(this->parent());
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

void XYGLRenderFormatDialog::on_TextFontComboBox_currentFontChanged(const QFont &font)
{
    XYGLRender* _parent = static_cast<
            XYGLRender*>(this->parent());
    _parent->_TextFont = font;
    _parent->updateGL();
}

void XYGLRenderFormatDialog::on_TextFontSizeSpinBox_valueChanged(int value)
{
    XYGLRender* _parent = static_cast<
            XYGLRender*>(this->parent());
    _parent->_TextFont.setPointSize(value);
    _parent->updateGL();
}

void UserInterface::XYGLRenderFormatDialog::on_GridColorButton_clicked()
{
    XYGLRender* _parent = static_cast<
            XYGLRender*>(this->parent());
    QColor _color = QColorDialog::getColor(
                _parent->_gridColor,
                this,
                "Select grid color",
                QColorDialog::ShowAlphaChannel);
    if(_color.isValid())
    {
        _parent->_gridColor = _color;
        ui->GridColorButton->setStyleSheet(
                    "background-color: " + _color.name(QColor::HexArgb));
        _parent->updateGL();
    }
}

void UserInterface::XYGLRenderFormatDialog::on_NumberOfAxisSpinBox_valueChanged(int arg1)
{
    XYGLRender* _parent = static_cast<
            XYGLRender*>(this->parent());
    _parent->_OXLinesNum = arg1;
    _parent->updateGL();
}

void UserInterface::XYGLRenderFormatDialog::functionColorButtonClicked()
{
    XYGLRender* _parent = static_cast<
            XYGLRender*>(this->parent());

    int _index;
    for(_index = 0; _index<_parent->_functions.size(); ++_index)
        if(_colorButtons[_index] == sender())
            break;

    QColor _color = QColorDialog::getColor(
                _parent->_functionColors[_index],
                this,
                "Select function plot color",
                QColorDialog::ShowAlphaChannel);
    if(_color.isValid())
    {
        _parent->_functionColors[_index] = _color;
        _colorButtons[_index]->setStyleSheet(
                    "background-color: " + _color.name(QColor::HexArgb));
        _parent->updateGL();
    }
}

void UserInterface::XYGLRenderFormatDialog::on_NumberPlotNodesSpinBox_valueChanged(int arg1)
{
    XYGLRender* _parent = static_cast<
            XYGLRender*>(this->parent());
    _parent->_nodesNum = arg1;
    _parent->updateGL();
}
