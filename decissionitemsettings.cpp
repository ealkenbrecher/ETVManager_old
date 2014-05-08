#include "decissionitemsettings.h"
#include "ui_decissionitemsettings.h"

DecissionItemSettings::DecissionItemSettings(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DecissionItemSettings)
{
  ui->setupUi(this);
}

DecissionItemSettings::~DecissionItemSettings()
{
  delete ui;
}

void DecissionItemSettings::setHeader (const QString aValue)
{
  ui->header->setText(aValue);
}

void DecissionItemSettings::setDescription (const QString aValue)
{
  ui->description->setText(aValue);
}

void DecissionItemSettings::setDecissionText (const QString aValue)
{
  ui->decissionText->setText(aValue);
}

void DecissionItemSettings::setDecissionProclamation (const QString aValue)
{
  ui->decissionProclamation->setText(aValue);
}

void DecissionItemSettings::setYesVotes (float aValue)
{
  ui->yes->setValue(aValue);
}

void DecissionItemSettings::setNoVotes (float aValue)
{
  ui->no->setValue(aValue);
}

void DecissionItemSettings::setAbsentionVotes (float aValue)
{
  ui->abstention->setValue(aValue);
}

void DecissionItemSettings::setType (int aType)
{
  if (0 == aType)
    ui->mitBeschlussoption->setChecked(true);
  else if (1 == aType)
    ui->schriftlicherBeschluss->setChecked(true);
  else if (2 == aType)
    ui->ohneBeschlussoption->setChecked(true);
}

int DecissionItemSettings::getType(void)
{
  if (ui->mitBeschlussoption->isChecked())
    return 0;
  else if (ui->schriftlicherBeschluss->isChecked())
    return 1;
  else if (ui->ohneBeschlussoption->isChecked ())
    return 2;

  return 0;
}

const QString DecissionItemSettings::getHeader (void)
{
  return ui->header->toHtml();
}

const QString DecissionItemSettings::getDescription (void)
{
  return ui->description->toHtml();
}

const QString DecissionItemSettings::getDecissionText (void)
{
  return ui->decissionText->toHtml();
}

const QString DecissionItemSettings::getDecissionProclamation (void)
{
  return ui->decissionProclamation->toHtml();
}

float DecissionItemSettings::getYesVotes (void)
{
  return ui->yes->value();
}

float DecissionItemSettings::getNoVotes (void)
{
  return ui->no->value();
}

float DecissionItemSettings::getAbsentionVotes (void)
{
  return ui->abstention->value();
}
