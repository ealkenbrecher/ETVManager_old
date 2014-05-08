#include "orderitemsettings.h"
#include "ui_orderitemsettings.h"

OrderItemSettings::OrderItemSettings(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::OrderItemSettings)
{
  ui->setupUi(this);
}

OrderItemSettings::~OrderItemSettings()
{
  delete ui;
}

void OrderItemSettings::setValue (int aValue)
{
  ui->spinBox->setValue(aValue);
}

int OrderItemSettings::getValue ()
{
  return ui->spinBox->value();
}
