#include "systemsettings.h"
#include "ui_systemsettings.h"

SystemSettings::SystemSettings(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SystemSettings)
{
  ui->setupUi(this);
}

SystemSettings::~SystemSettings()
{
  delete ui;
}
