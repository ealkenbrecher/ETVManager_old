#include "patterneditorreportitemsettings.h"
#include "ui_patterneditorreportitemsettings.h"
#include "database.h"

PatternEditorReportItemSettings::PatternEditorReportItemSettings(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::PatternEditorReportItemSettings)
{
  ui->setupUi(this);

  refresh ();
}

PatternEditorReportItemSettings::~PatternEditorReportItemSettings()
{
  delete ui;
}

void PatternEditorReportItemSettings::refresh()
{
  ui->wildcards->addItem("%MEAgesamt%");
  ui->wildcards->addItem("%DatumEinladungsschreiben%");
  ui->wildcards->addItem("%UhrzeitStartVersammlung%");
  ui->wildcards->addItem("%UhrzeitEndeVersammlung%");
  ui->wildcards->addItem("%BeschlussfaehigMarker%");
  ui->wildcards->addItem("%MEAAnwesend%");
  ui->wildcards->addItem("%Versammlungsleiter%");
  ui->wildcards->addItem("%VersammlungsleiterFktVerwalterMarker%");
  ui->wildcards->addItem("%VersammlungsleiterFktMitarbeiterMarker%");
  ui->wildcards->addItem("%VersammlungsleiterFktWEGMarker%");
  ui->wildcards->addItem("%Protokollfuehrung%");
  ui->wildcards->addItem("%ProtokollfuehrerFktChef%");
  ui->wildcards->addItem("%ProtokollfuehrerFktMitarbeiter%");
  ui->wildcards->addItem("%ProtokollfuehrerFktWEG%");
  ui->wildcards->addItem("%Abstimmungsregelung%");
  ui->wildcards->addItem("%BeiratVorsitz%");
  ui->wildcards->addItem("%BeiratMitglied1%");
  ui->wildcards->addItem("%BeiratMitglied2%");
  ui->wildcards->addItem("%Einladungsfrist%");

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

void PatternEditorReportItemSettings::on_insertWildcard_clicked()
{
  QTextCursor cursor;
  QModelIndex index;

  cursor = ui->textBody->textCursor();
  ui->textBody->setFocus();

  cursor.insertText(ui->wildcards->currentText());
}

void PatternEditorReportItemSettings::setBodyText (QString aText)
{
  ui->textBody->setText(aText);
}

void PatternEditorReportItemSettings::setPatternName (QString aText)
{
  ui->PatternName->setText(aText);
}
QString PatternEditorReportItemSettings::getBodyText ()
{
  return ui->textBody->toHtml();
}

QString PatternEditorReportItemSettings::getPatternName ()
{
  return ui->PatternName->text();
}
