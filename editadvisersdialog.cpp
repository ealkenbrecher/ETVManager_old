#include "editadvisersdialog.h"
#include "ui_editadvisersdialog.h"

EditAdvisersDialog::EditAdvisersDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::EditAdvisersDialog)
{
  ui->setupUi(this);
}

EditAdvisersDialog::~EditAdvisersDialog()
{
  delete ui;
}

void EditAdvisersDialog::setAdviserPresident (const QString &rValue)
{
  ui->adviserPresident->setText(rValue);
}

void EditAdvisersDialog::setAdviser1 (const QString &rValue)
{
  ui->adviser1->setText(rValue);
}

void EditAdvisersDialog::setAdviser2 (const QString &rValue)
{
  ui->adviser2->setText(rValue);
}

QString EditAdvisersDialog::getAdviserPresident ()
{
  return ui->adviserPresident->text();
}

QString EditAdvisersDialog::getAdviser1 ()
{
  return ui->adviser1->text();
}

QString EditAdvisersDialog::getAdviser2 ()
{
  return ui->adviser2->text();
}
