#include "clmanagergui.h"
#include "ui_clmanagergui.h"

#include "CLMANAGER/clmanager.h"

using namespace UserInterface;

CLManagerGUI::CLManagerGUI(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CLManagerGUI)
{
    ui->setupUi(this);

    for(unsigned i=0; i<OpenCL::CLManager::instance().getPlatforms().size(); ++i)
    {
        cl::STRING_CLASS _data;
        OpenCL::CLManager::instance().getPlatforms()[i].getInfo(CL_PLATFORM_NAME, &_data);
        ui->PlatformComboBox->addItem(QString(_data.data()).trimmed());
    }
    ui->PlatformComboBox->setCurrentIndex(
                OpenCL::CLManager::instance().getCurrentPlatformIndex());
    ui->PlatformInfoPlainTextEdit->setFont(QFont("Courier New"));
    ui->PlatformInfoPlainTextEdit->setPlainText(
                OpenCL::CLManager::instance().printPlatformInfo(
                    OpenCL::CLManager::instance().getCurrentPlatformIndex()).data());

    ui->DeviceComboBox->setCurrentIndex(OpenCL::CLManager::instance().getCurrentDeviceIndex());
}

CLManagerGUI::~CLManagerGUI()
{
    delete ui;
}

int CLManagerGUI::getPlatformIndex() const
{
    return ui->PlatformComboBox->currentIndex();
}

int CLManagerGUI::getDeviceIndex() const
{
    return ui->DeviceComboBox->currentIndex();
}

void CLManagerGUI::on_PlatformComboBox_currentIndexChanged(int index)
{
    ui->DeviceComboBox->clear();
    for(unsigned i=0; i<OpenCL::CLManager::instance().getDevices()[index].size(); ++i)
    {
        cl::STRING_CLASS _data;
        OpenCL::CLManager::instance().getDevices()[index][i].getInfo(CL_DEVICE_NAME, &_data);;
        ui->DeviceComboBox->addItem(QString(_data.data()).trimmed());
    }
    ui->DeviceComboBox->setCurrentIndex(0);

    ui->DeviceInfoPlainTextEdit->setFont(QFont("Courier New"));
    ui->PlatformInfoPlainTextEdit->setPlainText(
                OpenCL::CLManager::instance().printPlatformInfo(index).data());
}

void CLManagerGUI::on_DeviceComboBox_currentIndexChanged(int index)
{
    ui->DeviceInfoPlainTextEdit->setFont(QFont("Courier New"));
    ui->DeviceInfoPlainTextEdit->setPlainText(
                OpenCL::CLManager::instance().printDeviceInfo(
                    ui->PlatformComboBox->currentIndex(), index).data());
}
