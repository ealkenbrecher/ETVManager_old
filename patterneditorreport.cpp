#include "patterneditorreport.h"
#include "ui_patterneditorreport.h"
#include "database.h"
#include "patterneditorreportitemsettings.h"
#include <QMessageBox>

patternEditorReport::patternEditorReport(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::patternEditorReport)
{
  ui->setupUi(this);

  mQueryModel = 0;

  updatePatternTable ();
}

patternEditorReport::~patternEditorReport()
{
  delete ui;
}

void patternEditorReport::updatePatternTable()
{
  QSqlDatabase* db = Database::getInstance()->getDatabase();

  if (0 != db)
  {
    if (Database::getInstance()->dbIsOk ())
    {
      QSqlQuery query (*db);
      query.prepare("SELECT id, ueberschrift, deckblatt FROM ReportPatterns");
      query.exec();

      mQueryModel = new QSqlQueryModelRichtext ();
      mQueryModel->setQuery(query);

      mQueryModel->setHeaderData(1, Qt::Horizontal, tr("Bezeichnung"));

      ui->tablePatterns->setModel(mQueryModel);
      ui->tablePatterns->horizontalHeader()->resizeSection(0, 50);
      // do not show id, is only necessary for internal use
      ui->tablePatterns->hideColumn(0);
      ui->tablePatterns->hideColumn(2);
      ui->tablePatterns->show();
    }
  }
}

void patternEditorReport::on_editEntry_clicked()
{
  if (0 != ui->tablePatterns->selectionModel())
  {
    if (ui->tablePatterns->selectionModel()->selectedRows().count() == 1)
    {
      int selectedRow = ui->tablePatterns->selectionModel()->selection().indexes().value(0).row();
      int patternId = ui->tablePatterns->model()->index(selectedRow,0).data().toInt();

      changePatternItemSettings(patternId);
    }
    else
    {
      QMessageBox::information(this, "Fehler", "Es wurde kein Eintrag ausgewählt.");
    }
  }
  else
      QMessageBox::information(this, "Fehler", "Interner Fehler");
}

void patternEditorReport::on_tablePatterns_doubleClicked(const QModelIndex &index)
{
  if (ui->tablePatterns->selectionModel()->selectedRows().count() == 1)
  {
    int top_id = ui->tablePatterns->model()->index(index.row(),0).data().toInt();
    changePatternItemSettings(top_id);
  }
  else
    QMessageBox::information(this, "Fehler", "Es wurde kein Eintrag ausgewählt.");
}

void patternEditorReport::changePatternItemSettings (int aId)
{
  int selectedRow = ui->tablePatterns->selectionModel()->selection().indexes().value(0).row();
  PatternEditorReportItemSettings itemSettings (this);

  QSqlQuery query (*Database::getInstance()->getDatabase());
  query.prepare("SELECT ueberschrift, deckblatt FROM ReportPatterns WHERE id = :id");

  query.bindValue(":id", aId);
  query.exec();

  while (query.next())
  {
    itemSettings.setPatternName(query.value(0).toString());
    itemSettings.setBodyText(query.value(1).toString());
  }

  //abort -> do not save settings
  if (itemSettings.exec() != QDialog::Accepted)
  {
    return;
  }
  else
  {
    if (Database::getInstance()->dbIsOk())
    {
      QSqlQuery query (*Database::getInstance()->getDatabase());
      //set values
      query.prepare("UPDATE ReportPatterns SET deckblatt =:newBodyText WHERE id = :id");
      query.bindValue(":id", aId);
      query.bindValue(":newBodyText", itemSettings.getBodyText());
      query.exec();

      updatePatternTable();
      ui->tablePatterns->setFocus();
      ui->tablePatterns->selectRow(selectedRow);
    }
  }
}
