#include "patterneditor.h"
#include "ui_patterneditor.h"
#include "database.h"
#include "qsqlquerymodelrichtext.h"
#include "PatternItemSettings.h"
#include <QMessageBox>

PatternEditor::PatternEditor(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::PatternEditor)
{
  ui->setupUi(this);

  mQueryModel = 0;

  updatePatternTable ();
}

void PatternEditor::updatePatternTable ()
{
    QSqlDatabase* db = Database::getInstance()->getDatabase();

    if (0 != db)
    {
        if (Database::getInstance()->dbIsOk ())
        {
          QSqlQuery query (*db);
          query.prepare("SELECT id, bezeichnung FROM AgendaPatterns ORDER BY bezeichnung ASC");
          query.exec();

          mQueryModel = new QSqlQueryModelRichtext ();
          mQueryModel->setQuery(query);
          mQueryModel->setHeaderData(0, Qt::Horizontal, tr("Bezeichnung"));

          ui->tablePatterns->setModel(mQueryModel);
          ui->tablePatterns->horizontalHeader()->resizeSection(0, 50);
          // do not show id, is only necessary for internal use
          ui->tablePatterns->hideColumn(0);
          ui->tablePatterns->show();
        }
    }
}

PatternEditor::~PatternEditor()
{
    delete ui;
    if (0 != mQueryModel)
        delete mQueryModel;
    mQueryModel = 0;
}

void PatternEditor::on_editEntry_clicked()
{
  if (0 != ui->tablePatterns->selectionModel())
  {
    //check if selection in tableview is valid
    if (ui->tablePatterns->selectionModel()->selectedRows().count() == 1)
    {
        int selectedRow = ui->tablePatterns->selectionModel()->selection().indexes().value(0).row();
        int patternId = ui->tablePatterns->model()->index(selectedRow,0).data().toInt();

        changePatternItemSettings(patternId);
    }
    else if (ui->tablePatterns->selectionModel()->selection().indexes().count() > 1)
        QMessageBox::information(this, "Fehler", "Bitte nur eine Zeile auswählen.");
    else if (ui->tablePatterns->selectionModel()->selection().indexes().count() == 0)
        QMessageBox::information(this, "Fehler", "Bitte eine Zeile auswählen.");
  }
  else
      QMessageBox::information(this, "Fehler", "Interner Fehler");

}


void PatternEditor::on_tablePatterns_doubleClicked(const QModelIndex &index)
{
  if (ui->tablePatterns->selectionModel()->selectedRows().count() == 1)
  {
    int top_id = ui->tablePatterns->model()->index(index.row(),0).data().toInt();
    changePatternItemSettings(top_id);
  }
}

void PatternEditor::changePatternItemSettings (int aId)
{
    PatternItemSettings itemSettings (this);

    QSqlQuery query (*Database::getInstance()->getDatabase());
    query.prepare("SELECT bezeichnung, ueberschrift, beschreibung, beschlussvorschlag, beschlussvorschlag2, beschlussvorschlag3, LeerzeilenProtokoll, beschlussArt FROM AgendaPatterns WHERE id = :id");

    query.bindValue(":id", aId);
    query.exec();

    while (query.next())
    {
      itemSettings.setPatternName (query.value(0).toString());
      itemSettings.setHeader(query.value(1).toString());
      itemSettings.setDescription(query.value(2).toString());
      itemSettings.setSuggestion(query.value(3).toString());
      itemSettings.setSuggestion2(query.value(4).toString());
      itemSettings.setSuggestion3(query.value(5).toString());
      itemSettings.setNumberOfLines(query.value(6).toInt());
      itemSettings.setType(query.value(7).toInt());
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
            query.prepare("UPDATE AgendaPatterns SET bezeichnung = :patternName, ueberschrift =:header, beschreibung =:descr, beschlussvorschlag =:suggestion, beschlussvorschlag2 =:suggestion2, beschlussvorschlag3 =:suggestion3, LeerzeilenProtokoll =:numberOfLines, beschlussArt =:type WHERE id = :id");
            query.bindValue(":id", aId);
            query.bindValue(":header", itemSettings.getHeader());
            query.bindValue(":patternName", itemSettings.getPatternName());
            query.bindValue(":descr", itemSettings.getDescription());
            query.bindValue(":suggestion", itemSettings.getSuggestion());
            query.bindValue(":suggestion2", itemSettings.getSuggestion2());
            query.bindValue(":suggestion3", itemSettings.getSuggestion3());
            query.bindValue(":numberOfLines", itemSettings.getNumberOfLines());
            query.bindValue(":type", itemSettings.getType());
            query.exec();

            updatePatternTable();
        }
    }
}

void PatternEditor::on_addEntry_clicked()
{
  PatternItemSettings itemSettings (this);

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
        query.prepare("INSERT INTO AgendaPatterns (ueberschrift, beschreibung, beschlussvorschlag, beschlussvorschlag2, beschlussvorschlag3, LeerzeilenProtokoll, bezeichnung, beschlussArt) VALUES (:header, :descr, :suggestion,:suggestion2, :suggestion3, :lines, :patternName, :type)");
        query.bindValue(":header", itemSettings.getHeader());
        query.bindValue(":descr", itemSettings.getDescription());
        query.bindValue(":suggestion", itemSettings.getSuggestion());
        query.bindValue(":suggestion2", itemSettings.getSuggestion2());
        query.bindValue(":suggestion3", itemSettings.getSuggestion3());
        query.bindValue(":type", itemSettings.getType());
        query.bindValue(":lines", itemSettings.getNumberOfLines());
        query.bindValue(":patternName", itemSettings.getPatternName());
        query.exec();

        updatePatternTable();
    }
  }
}

void PatternEditor::on_deleteEntry_clicked()
{
  if (ui->tablePatterns->selectionModel()->selectedRows().count() == 1)
  {
      QMessageBox::StandardButton reply = QMessageBox::question(this, "Achtung", "Vorlage wirklich löschen?", QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes)
      {
          int selectedRow = ui->tablePatterns->selectionModel()->selection().indexes().value(0).row();
          int id = ui->tablePatterns->model()->index(selectedRow,0).data().toInt();

          QSqlQuery query (*Database::getInstance()->getDatabase());
          query.prepare("DELETE FROM AgendaPatterns WHERE id = :id");
          query.bindValue(":id", id);
          query.exec();

          updatePatternTable();
      }
  }
  else if (ui->tablePatterns->selectionModel()->selection().indexes().count() > 1)
      QMessageBox::information(this, "Fehler", "Bitte nur eine Zeile auswählen.");
  else if (ui->tablePatterns->selectionModel()->selection().indexes().count() == 0)
      QMessageBox::information(this, "Fehler", "Bitte eine Zeile auswählen.");
}
