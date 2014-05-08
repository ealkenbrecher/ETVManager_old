#include "agendatab.h"
#include "ui_agendatab.h"
#include "mainview.h"
#include "database.h"
#include <QtSql>
#include <QString>
#include "agendasettings.h"
#include <string>
#include "agendaitemsettings.h"
#include "qsqlquerymodelrichtext.h"
#include <QMenu>

AgendaTab::AgendaTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AgendaTab)
{
    ui->setupUi(this);
    mQueryModel = 0;

    ui->tableAgenda->horizontalHeader()->setStretchLastSection(true);
}

AgendaTab::~AgendaTab()
{
    if (0 != mQueryModel)
        delete mQueryModel;

    delete ui;
}

void AgendaTab::refreshOnSelected ()
{
  if (!this->isEnabled())
  {
    QMessageBox::information(this, "Fehler", "Alle Einstellungen deaktiviert.\nLegen Sie zunächst ein Wirtschaftsjahr und eine Eigentümerversammlung an");
    return;
  }

  ui->tableAgenda->setColumnWidth(0, 40);

  updateAgendaTable();
}

void AgendaTab::updateAgendaTable ()
{
    QSqlDatabase* db = Database::getInstance()->getDatabase();

    if (0 != db)
    {
        if (Database::getInstance()->dbIsOk ())
        {
          QSqlQuery query (*db);
          query.prepare("SELECT top_id, top_header FROM Tagesordnungspunkte WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvnr");
          query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
          query.bindValue(":year", Database::getInstance()->getCurrentYear());
          query.bindValue(":etvnr", Database::getInstance()->getCurrentEtvNumber());
          query.exec();

          mQueryModel = new QSqlQueryModelRichtext ();
          mQueryModel->setQuery(query);
          mQueryModel->setHeaderData(0, Qt::Horizontal, tr("Nr."));
          mQueryModel->setHeaderData(1, Qt::Horizontal, tr("Bezeichnung"));

          ui->tableAgenda->setModel(mQueryModel);
          ui->tableAgenda->horizontalHeader()->resizeSection(0, 50);
          ui->tableAgenda->show();
        }
    }
}

void AgendaTab::changeAgendaItemSettings (int aTopId)
{
    AgendaItemSettings itemSettings (this);

    QSqlQuery query (*Database::getInstance()->getDatabase());
    query.prepare("SELECT top_header, top_descr, top_vorschlag, top_vorschlag2, top_vorschlag3, beschlussArt FROM Tagesordnungspunkte WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvnr AND top_id = :topid");

    query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
    query.bindValue(":year", Database::getInstance()->getCurrentYear());
    query.bindValue(":etvnr", Database::getInstance()->getCurrentEtvNumber());
    query.bindValue(":topid", aTopId);
    query.exec();

    if (query.next())
    {
      itemSettings.setHeader(query.value(0).toString());
      itemSettings.setDescription(query.value(1).toString());
      itemSettings.setSuggestion(query.value(2).toString());
      itemSettings.setSuggestion2(query.value(3).toString());
      itemSettings.setSuggestion3(query.value(4).toString());
      itemSettings.setItemType(query.value(5).toInt());
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
            query.prepare("UPDATE Tagesordnungspunkte SET top_header =:header, top_descr =:descr, top_vorschlag =:suggestion, top_vorschlag2 =:suggestion2, top_vorschlag3 =:suggestion3, beschlussArt =:itemType WHERE obj_id = :id AND wi_jahr = :year AND top_id = :topid AND etv_nr = :etvnum");
            query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
            query.bindValue(":year", Database::getInstance()->getCurrentYear());
            query.bindValue(":etvnum", Database::getInstance()->getCurrentEtvNumber());
            query.bindValue(":header", itemSettings.getHeader());
            query.bindValue(":descr", itemSettings.getDescription());
            query.bindValue(":suggestion", itemSettings.getSuggestion());
            query.bindValue(":suggestion2", itemSettings.getSuggestion2());
            query.bindValue(":suggestion3", itemSettings.getSuggestion3());
            query.bindValue(":topid", aTopId);
            query.bindValue(":itemType", itemSettings.getItemType());
            query.exec();

            updateAgendaTable();
        }
    }
}

void AgendaTab::on_tableAgenda_doubleClicked(const QModelIndex &index)
{
  if (ui->tableAgenda->selectionModel()->selectedRows().count() == 1)
  {
    int top_id = ui->tableAgenda->model()->index(index.row(),0).data().toInt();
    changeAgendaItemSettings(top_id);
  }
}

void AgendaTab::on_addEntry_clicked()
{
    AgendaItemSettings itemSettings (this);

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
            query.prepare("INSERT INTO Tagesordnungspunkte (obj_id, wi_jahr, top_id, etv_nr, top_header, top_descr, top_vorschlag, top_vorschlag2, top_vorschlag3, beschlussArt) VALUES (:id, :year, :topid, :etvnum, :header, :descr, :suggestion, :suggestion2, :suggestion3, :type)");
            query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
            query.bindValue(":year", Database::getInstance()->getCurrentYear());
            query.bindValue(":etvnum", Database::getInstance()->getCurrentEtvNumber());
            query.bindValue(":header", itemSettings.getHeader());
            query.bindValue(":descr", itemSettings.getDescription());
            query.bindValue(":suggestion", itemSettings.getSuggestion());
            query.bindValue(":suggestion2", itemSettings.getSuggestion2());
            query.bindValue(":suggestion3", itemSettings.getSuggestion3());
            query.bindValue(":type", itemSettings.getItemType());

            //fix empty table crash
            if (0 != ui->tableAgenda->model())
            {
              query.bindValue(":topid", ui->tableAgenda->model()->rowCount() + 1);
              query.exec();
            }
            else
              qDebug () << "top id cannot be set. Abort";

            updateAgendaTable();
        }
    }
}

void AgendaTab::on_editEntry_clicked()
{
  //check if selection in tableview is valid
  if (ui->tableAgenda->selectionModel()->selectedRows().count() == 1)
  {
      int selectedRow = ui->tableAgenda->selectionModel()->selection().indexes().value(0).row();
      int top_id = ui->tableAgenda->model()->index(selectedRow,0).data().toInt();

      changeAgendaItemSettings(top_id);
  }
  else if (ui->tableAgenda->selectionModel()->selection().indexes().count() == 0)
    QMessageBox::information(this, "Fehler", "Bitte eine Zeile auswählen.");
}

void AgendaTab::on_deleteEntry_clicked()
{
  if (ui->tableAgenda->selectionModel()->selectedRows().count() == 1)
  {
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Achtung", "Tagesordnungspunkt wirklich löschen?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        int selectedRow = ui->tableAgenda->selectionModel()->selection().indexes().value(0).row();
        int top_id = ui->tableAgenda->model()->index(selectedRow,0).data().toInt();

        QSqlQuery query (*Database::getInstance()->getDatabase());
        query.prepare("DELETE FROM Tagesordnungspunkte WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvnr AND top_id = :topid");

        query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
        query.bindValue(":year", Database::getInstance()->getCurrentYear());
        query.bindValue(":etvnr", Database::getInstance()->getCurrentEtvNumber());
        query.bindValue(":topid", top_id);
        query.exec();

        bool next = true;

        while (true == next)
        {
          top_id++;

          //check, if there is a top_id with a higher id
          query.prepare("SELECT top_id FROM Tagesordnungspunkte WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvnr AND top_id = :topid");
          query.bindValue(":topid", top_id);
          query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
          query.bindValue(":year", Database::getInstance()->getCurrentYear());
          query.bindValue(":etvnr", Database::getInstance()->getCurrentEtvNumber());
          query.exec();

          //higher id available -> set new id
          if (query.next ())
          {
            query.prepare("UPDATE Tagesordnungspunkte SET top_id = :newid WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvnr AND top_id = :oldid");
            query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
            query.bindValue(":year", Database::getInstance()->getCurrentYear());
            query.bindValue(":etvnr", Database::getInstance()->getCurrentEtvNumber());
            query.bindValue(":oldid", top_id);
            query.bindValue(":newid", top_id - 1);
            query.exec();
          }
          else
            next = false;
        }
        updateAgendaTable();
        ui->tableAgenda->setFocus();

        //set focussed item
        int newRowSelection = 0;
        if (ui->tableAgenda->model()->rowCount() > selectedRow)
          newRowSelection = selectedRow;
        else
          newRowSelection = ui->tableAgenda->model()->rowCount();

        ui->tableAgenda->selectRow(newRowSelection);
    }
  }
  else
  QMessageBox::information(this, "Fehler", "Es wurde kein Eintrag zum Löschen ausgewählt.");
}

void AgendaTab::on_moveAgendaItemUp_clicked()
{
  int selectedRow = ui->tableAgenda->selectionModel()->selection().indexes().value(0).row();
  int top_id = ui->tableAgenda->model()->index(selectedRow,0).data().toInt();

  if (1 == top_id)
  {
    QMessageBox::information(this, "Fehler", "Tagesordnungspunkt kann nicht nach oben verschoben werden.");
  }
  else
  {
      if (Database::getInstance()->dbIsOk())
      {
          QSqlQuery query (*Database::getInstance()->getDatabase());
          //set the id of the top before to -999
          query.prepare("UPDATE Tagesordnungspunkte SET top_id = -999 WHERE obj_id = :id AND wi_jahr = :year AND top_id = :topid AND etv_nr = :etvnum");
          query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
          query.bindValue(":year", Database::getInstance()->getCurrentYear());
          query.bindValue(":etvnum", Database::getInstance()->getCurrentEtvNumber());
          query.bindValue(":topid", (top_id - 1));
          query.exec();

          //set the chosen top to the correct id
          query.prepare("UPDATE Tagesordnungspunkte SET top_id = :topid_new WHERE obj_id = :id AND wi_jahr = :year AND top_id =:topid AND etv_nr = :etvnum");
          query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
          query.bindValue(":year", Database::getInstance()->getCurrentYear());
          query.bindValue(":etvnum", Database::getInstance()->getCurrentEtvNumber());
          query.bindValue(":topid", top_id);
          query.bindValue(":topid_new", top_id-1);
          query.exec();

          //set the '-999' topid to the correct topid
          query.prepare("UPDATE Tagesordnungspunkte SET top_id = :topid WHERE obj_id = :id AND wi_jahr = :year AND top_id = -999 AND etv_nr = :etvnum");
          query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
          query.bindValue(":year", Database::getInstance()->getCurrentYear());
          query.bindValue(":etvnum", Database::getInstance()->getCurrentEtvNumber());
          query.bindValue(":topid", top_id);
          query.exec();

          updateAgendaTable();
          ui->tableAgenda->setFocus();
          ui->tableAgenda->selectRow(top_id - 2);
      }
  }
}

void AgendaTab::on_moveAgendaItemDown_clicked()
{
  int selectedRow = ui->tableAgenda->selectionModel()->selection().indexes().value(0).row();
  int top_id = ui->tableAgenda->model()->index(selectedRow,0).data().toInt();

  int rows = ui->tableAgenda->model()->rowCount();
  if (rows == selectedRow + 1 || rows == top_id)
    QMessageBox::information(this, "Fehler", "Tagesordnungspunkt kann nicht nach unten verschoben werden.");
  else
  {
    if (Database::getInstance()->dbIsOk())
    {
        QSqlQuery query (*Database::getInstance()->getDatabase());
        //set the id of the top before to -999
        query.prepare("UPDATE Tagesordnungspunkte SET top_id = -999 WHERE obj_id = :id AND wi_jahr = :year AND top_id = :topid AND etv_nr = :etvnum");
        query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
        query.bindValue(":year", Database::getInstance()->getCurrentYear());
        query.bindValue(":etvnum", Database::getInstance()->getCurrentEtvNumber());
        query.bindValue(":topid", (top_id + 1));
        query.exec();

        //set the chosen top to the correct id
        query.prepare("UPDATE Tagesordnungspunkte SET top_id = :topid_new WHERE obj_id = :id AND wi_jahr = :year AND top_id =:topid AND etv_nr = :etvnum");
        query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
        query.bindValue(":year", Database::getInstance()->getCurrentYear());
        query.bindValue(":etvnum", Database::getInstance()->getCurrentEtvNumber());
        query.bindValue(":topid", top_id);
        query.bindValue(":topid_new", top_id + 1);
        query.exec();

        //set the '-999' topid to the correct topid
        query.prepare("UPDATE Tagesordnungspunkte SET top_id = :topid WHERE obj_id = :id AND wi_jahr = :year AND top_id = -999 AND etv_nr = :etvnum");
        query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
        query.bindValue(":year", Database::getInstance()->getCurrentYear());
        query.bindValue(":etvnum", Database::getInstance()->getCurrentEtvNumber());
        query.bindValue(":topid", top_id);
        query.exec();

        updateAgendaTable();
        ui->tableAgenda->setFocus();
        ui->tableAgenda->selectRow(top_id);
    }
  }
}
