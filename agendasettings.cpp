#include "agendasettings.h"
#include "ui_agendasettings.h"

AgendaSettings::AgendaSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AgendaSettings)
{
    ui->setupUi(this);
}

AgendaSettings::~AgendaSettings()
{
    delete ui;
}

int AgendaSettings::getYear () const
{
    return ui->year->value();
}

int AgendaSettings::getEtvNum () const
{
    return ui->etvNum->value();
}

QDate AgendaSettings::getEtvDate () const
{
    return ui->etvDate->date();
}

QTime AgendaSettings::getEtvTime () const
{
    return ui->etvTime->time();
}

QString AgendaSettings::getEtvLocation () const
{
    return ui->etvLoc->toHtml();
}

void AgendaSettings::setYear (int aYear)
{
  ui->year->setValue(aYear);
}

void AgendaSettings::setEtvNum (int aNum)
{
  ui->etvNum->setValue(aNum);
}

void AgendaSettings::setEtvDate (QDate aDate)
{
  ui->etvDate->setDate(aDate);
}

void AgendaSettings::setEtvTime (QTime aTime)
{
  ui->etvTime->setTime(aTime);
}

void AgendaSettings::setEtvLocation (QString aLocation)
{
  ui->etvLoc->setHtml(aLocation);
}

void AgendaSettings::disableYearSetting (bool aDisable)
{
  ui->year->setDisabled(aDisable);
}

void AgendaSettings::disableEtvNumSetting (bool aDisable)
{
  ui->etvNum->setDisabled(aDisable);
}
