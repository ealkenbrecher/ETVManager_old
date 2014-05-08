#include "patternitemsettings.h"
#include "ui_patternitemsettings.h"
#include <QtCore>

PatternItemSettings::PatternItemSettings(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::PatternItemSettings)
{
  ui->setupUi(this);

  mFocussedView = eNone;

  ui->header->installEventFilter(this);
  ui->description->installEventFilter(this);
  ui->suggestion->installEventFilter(this);
  ui->suggestion2->installEventFilter(this);
  ui->suggestion3->installEventFilter(this);

  //default option (if not overwritten after constructor call)
  ui->mitBeschlussoption->setChecked(true);

  refresh ();
}

PatternItemSettings::~PatternItemSettings()
{
  delete ui;
}

void PatternItemSettings::refresh ()
{
    ui->wildcards->addItem("%DatumHeute%");
    ui->wildcards->addItem("%Wirtschaftsjahr%");
    ui->wildcards->addItem("%NächstesWirtschaftsjahr%");
    ui->wildcards->addItem("%VergangenesWirtschaftsjahr%");
    ui->wildcards->addItem("%ETVNummer%");
    ui->wildcards->addItem("%ETVDatum%");
    ui->wildcards->addItem("%ETVUhrzeit%");
    ui->wildcards->addItem("%DatumNachAnfechtungsfrist%");
    ui->wildcards->addItem("%ZahlungslaufNachAnfechtungsfrist%");
    ui->wildcards->addItem("%BeiratVorsitz%");
    ui->wildcards->addItem("%BeiratMitglied1%");
    ui->wildcards->addItem("%BeiratMitglied2%");

    //sort entries
    QSortFilterProxyModel* proxy = new QSortFilterProxyModel(ui->wildcards);
    proxy->setSourceModel(ui->wildcards->model()); // <--
    // combo's current model must be reparented,
    // otherwise QComboBox::setModel() will delete it
    ui->wildcards->model()->setParent(proxy);
    ui->wildcards->setModel(proxy); // <--
    // sort
    ui->wildcards->model()->sort(0);

    ui->wildcards->setCurrentIndex(0);
}

QString PatternItemSettings::getPatternName ()
{
    return ui->patternName->toPlainText();
}

QString PatternItemSettings::getHeader ()
{
    return ui->header->toPlainText();
}

QString PatternItemSettings::getDescription ()
{
    return ui->description->toPlainText();
}

QString PatternItemSettings::getSuggestion ()
{
    return ui->suggestion->toPlainText();
}

QString PatternItemSettings::getSuggestion2 ()
{
    return ui->suggestion2->toPlainText();
}

QString PatternItemSettings::getSuggestion3 ()
{
    return ui->suggestion3->toPlainText();
}

int PatternItemSettings::getNumberOfLines()
{
    return ui->numberOfLines->value();
}

int PatternItemSettings::getType ()
{
  if (ui->mitBeschlussoption->isChecked())
    return 0;
  else if (ui->ohneBeschlussoption->isChecked())
    return 2;

  return 0;
}

void PatternItemSettings::setNumberOfLines(int aNumber)
{
    ui->numberOfLines->setValue(aNumber);
}

void PatternItemSettings::setPatternName (const QString &rValue)
{
    ui->patternName->setText(rValue);
}

void PatternItemSettings::setHeader (QString rValue)
{
    ui->header->setText(rValue);
}

void PatternItemSettings::setDescription (QString rValue)
{
    ui->description->setText(rValue);
}

void PatternItemSettings::setSuggestion (QString rValue)
{
    ui->suggestion->setText(rValue);
}

void PatternItemSettings::setSuggestion2 (QString rValue)
{
    ui->suggestion2->setText(rValue);
}

void PatternItemSettings::setSuggestion3 (QString rValue)
{
    ui->suggestion3->setText(rValue);
}

void PatternItemSettings::setType (int aType)
{
  if (0 == aType)
    ui->mitBeschlussoption->setChecked(true);
  if (2 == aType)
    ui->ohneBeschlussoption->setChecked(true);
}

bool PatternItemSettings::eventFilter(QObject *object, QEvent *event)
{
  switch (event->type())
  {
    case QEvent::FocusIn:
      if (object == ui->description)
        mFocussedView = eDescription;
      else if (object == ui->header)
        mFocussedView = eHeader;
      else if (object == ui->suggestion)
        mFocussedView = eSuggestion;
      else if (object == ui->suggestion2)
        mFocussedView = eSuggestion2;
      else if (object == ui->suggestion3)
        mFocussedView = eSuggestion3;
      break;
    default:
      break;
  }
  return false;
}

void PatternItemSettings::on_insertWildcard_clicked()
{
  QTextCursor cursor;
  QModelIndex index;

  switch (mFocussedView)
  {
    case eDescription:
      cursor = ui->description->textCursor();
      ui->description->setFocus();
      break;
    case eHeader:
      cursor = ui->header->textCursor();
      ui->header->setFocus();
      break;
    case eSuggestion:
      cursor = ui->suggestion->textCursor();
      ui->suggestion->setFocus();
      break;
    case eSuggestion2:
      cursor = ui->suggestion2->textCursor();
      ui->suggestion2->setFocus();
      break;
    case eSuggestion3:
      cursor = ui->suggestion3->textCursor();
      ui->suggestion3->setFocus();
      break;
    default:
      qDebug () << "Cannot insert wildcard: No focussed view found";
      return;
      break;
  }
  cursor.insertText(ui->wildcards->currentText());
}
