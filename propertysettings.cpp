#include "propertysettings.h"
#include "ui_propertysettings.h"
#include <QtCore>
#include "stringreplacer.h"

PropertySettings::PropertySettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PropertySettings)
{
  ui->setupUi(this);
}

void PropertySettings::setPropertyName (QString name)
{
  ui->propertyName->setText(name);
}
void PropertySettings::setMea(float mea)
{
  ui->mea->setValue((double)mea);
}
void PropertySettings::setVotingRule (const QString rVotingRule)
{
  ui->votingType->setText(rVotingRule);
}
void PropertySettings::setOwnerQuantity (long ownerQuantity)
{
  ui->ownerQuantity->setValue(ownerQuantity);
}

void PropertySettings::setpropertyId (long aId)
{
    QString value = QString::number(aId);
    ui->propertyId->setText(value);
}

void PropertySettings::setInvitationDeadline (QString aWeeks)
{
  ui->deadline->setText(aWeeks);
}

QString PropertySettings::getInvitationDeadline ()
{
  return ui->deadline->toHtml();
}

QString PropertySettings::propertyName () const
{
    return ui->propertyName->text();
}

float PropertySettings::mea () const
{
    return ui->mea->value();
}

QString PropertySettings::votingRule () const
{
    return ui->votingType->toHtml();
}

long PropertySettings::ownerQuantity () const
{
    return ui->ownerQuantity->value();
}

PropertySettings::~PropertySettings()
{
    delete ui;
}
