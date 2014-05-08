#include "decissionlibraryeditdialog.h"
#include "ui_decissionlibraryeditdialog.h"

DecissionLibraryEditDialog::DecissionLibraryEditDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DecissionLibraryEditDialog)
{
  ui->setupUi(this);
}

DecissionLibraryEditDialog::~DecissionLibraryEditDialog()
{
  delete ui;
}

void DecissionLibraryEditDialog::setVoteType (int aType)
{
  if (0 == aType)
    ui->mitBeschlussoption->setChecked(true);
  else if (1 == aType)
    ui->schriftlicherBeschluss->setChecked(true);
  else if (2 == aType)
    ui->ohneBeschlussoption->setChecked(true);
}

void DecissionLibraryEditDialog::setAnnotation (const QString &rValue)
{
  ui->annotation->setText(rValue);
}

void DecissionLibraryEditDialog::setCourtOrder (const QString &rValue)
{
  ui->courtOrder->setText(rValue);
}

void DecissionLibraryEditDialog::setAddDescriptionFlag (bool aValue)
{
  ui->addDescription->setChecked(aValue);
}

int DecissionLibraryEditDialog::getVoteType (void) const
{
  if (ui->mitBeschlussoption->isChecked())
    return 0;
  else if (ui->schriftlicherBeschluss->isChecked())
    return 1;
  else if (ui->ohneBeschlussoption->isChecked())
    return 2;

  return -1;
}

QString DecissionLibraryEditDialog::getAnnotation (void) const
{
  return ui->annotation->toHtml();
}

QString DecissionLibraryEditDialog::getCourtOrder (void) const
{
  return ui->courtOrder->toHtml();
}

bool DecissionLibraryEditDialog::getAddDescriptionFlag ()
{
  return ui->addDescription->isChecked();
}
