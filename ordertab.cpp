#include "ordertab.h"
#include "ui_ordertab.h"
#include "mainview.h"
#include "database.h"
#include <QtSql>
#include <QString>
#include "agendasettings.h"
#include <string>
#include "orderitemsettings.h"
#include "qsqlquerymodelrichtext.h"
#include <QMenu>
#include "propertysettings.h"
#include "patterneditorreportitemsettings.h"

OrderTab::OrderTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OrderTab)
{
    ui->setupUi(this);

    mQueryModel = 0;
    mQueryModelReportTemplate = 0;
}

OrderTab::~OrderTab()
{
    if (0 != mQueryModel)
    {
      delete mQueryModel;
      mQueryModel = 0;
    }

    if (0 != mQueryModelReportTemplate)
    {
      delete mQueryModelReportTemplate;
      mQueryModelReportTemplate = 0;
    }

    delete ui;
}

void OrderTab::refreshOnSelected ()
{
  if (!this->isEnabled())
  {
    QMessageBox::information(this, "Fehler", "Alle Einstellungen deaktiviert.\nLegen Sie zunächst ein Wirtschaftsjahr und eine Eigentümerversammlung an");
    return;
  }

  updateAgendaTable();
  updateReportTemplateTable();

  ui->tableAgenda->setColumnWidth(0, 40);
  ui->tableAgenda->setColumnWidth(1, 250);

  int stretch = ui->tableAgenda->width() - ui->tableAgenda->columnWidth(0) /*- ui->tableAgenda->columnWidth(1)*/ - ui->tableAgenda->columnWidth(2);
  if (stretch > 0)
  {
      ui->tableAgenda->setColumnWidth(1, ui->tableAgenda->columnWidth(1) + stretch);
  }
}



void OrderTab::updateAgendaTable ()
{
  QSqlDatabase* db = Database::getInstance()->getDatabase();
  if (0 != db)
  {
    Database::getInstance()->getDatabase()->open();
    QSqlQuery query (*db);
    query.prepare("SELECT top_id, top_header, LeerzeilenProtokoll FROM Tagesordnungspunkte WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvnr");
    query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
    query.bindValue(":year", Database::getInstance()->getCurrentYear());
    query.bindValue(":etvnr", Database::getInstance()->getCurrentEtvNumber());
    query.exec();

    mQueryModel = new QSqlQueryModelRichtext ();
    mQueryModel->setQuery(query);
    mQueryModel->setHeaderData(0, Qt::Horizontal, tr("Nr."));
    mQueryModel->setHeaderData(1, Qt::Horizontal, tr("Bezeichnung"));
    mQueryModel->setHeaderData(2, Qt::Horizontal, tr("Leerzeilen Protokollvordruck"));

    ui->tableAgenda->setModel(mQueryModel);
    ui->tableAgenda->horizontalHeader()->resizeSection(0, 50);
    ui->tableAgenda->show();

    db->close();
  }
}

void OrderTab::updateReportTemplateTable ()
{
  QSqlDatabase* db = Database::getInstance()->getDatabase();
  if (0 != db)
  {
      db->open();
      QSqlQuery query (*db);
      query.prepare("SELECT Protokollueberschrift FROM Eigentuemerversammlungen WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvnr");
      query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
      query.bindValue(":year", Database::getInstance()->getCurrentYear());
      query.bindValue(":etvnr", Database::getInstance()->getCurrentEtvNumber());
      query.exec();

      mQueryModelReportTemplate = new QSqlQueryModelRichtext ();
      mQueryModelReportTemplate->setQuery(query);
      //mQueryModelReportTemplate->setHeaderData(0, Qt::Horizontal, tr("Bezeichnung"));

      ui->tableReportTemplate->setModel(mQueryModelReportTemplate);
      ui->tableReportTemplate->show();
      db->close();
  }
}

void OrderTab::on_tableAgenda_doubleClicked(const QModelIndex &index)
{
  if (ui->tableAgenda->selectionModel()->selectedRows().count() == 1)
  {
    int top_id = ui->tableAgenda->model()->index(index.row(),0).data().toInt();
    changeAgendaItemSettings (top_id);
  }
}

void OrderTab::changeAgendaItemSettings (int aId)
{
  int selectedRow = ui->tableAgenda->selectionModel()->selection().indexes().value(0).row();
  OrderItemSettings itemSettings (this);
  QSqlDatabase* db = Database::getInstance()->getDatabase();

  if (0 != db)
  {
    db->open();
    QSqlQuery query (*db);
    query.prepare("SELECT LeerzeilenProtokoll FROM Tagesordnungspunkte WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvnr AND top_id = :top_id");
    query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
    query.bindValue(":year", Database::getInstance()->getCurrentYear());
    query.bindValue(":etvnr", Database::getInstance()->getCurrentEtvNumber());
    query.bindValue(":top_id", aId);
    query.exec();
    db->close();

    while (query.next())
    {
      itemSettings.setValue(query.value(0).toInt());
    }
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
      Database::getInstance()->getDatabase()->open();
      QSqlQuery query (*Database::getInstance()->getDatabase());
      //set values
      query.prepare("UPDATE Tagesordnungspunkte SET LeerzeilenProtokoll =:lines WHERE obj_id = :id AND wi_jahr = :year AND top_id = :topid AND etv_nr = :etvnum");
      query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
      query.bindValue(":year", Database::getInstance()->getCurrentYear());
      query.bindValue(":etvnum", Database::getInstance()->getCurrentEtvNumber());
      query.bindValue(":topid", aId);
      query.bindValue(":lines", itemSettings.getValue());
      query.exec();
      Database::getInstance()->getDatabase()->close();

      updateAgendaTable();
      ui->tableAgenda->setFocus();
      ui->tableAgenda->selectRow(selectedRow);
    }
  }
}

void OrderTab::on_tableReportTemplate_doubleClicked(const QModelIndex &index)
{
  //int top_id = ui->tableReportTemplate->model()->index(index.row(),0).data().toInt();

  PatternEditorReportItemSettings itemSettings (this);

  Database::getInstance()->getDatabase()->open();
  QSqlQuery query (*Database::getInstance()->getDatabase());
  query.prepare("SELECT Protokollueberschrift, Protokollvorlage FROM Eigentuemerversammlungen WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvnr");
  query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
  query.bindValue(":year", Database::getInstance()->getCurrentYear());
  query.bindValue(":etvnr", Database::getInstance()->getCurrentEtvNumber());
  query.exec();

  while (query.next())
  {
    itemSettings.setPatternName(query.value(0).toString());
    itemSettings.setBodyText(query.value(1).toString());
  }

  Database::getInstance()->getDatabase()->close();

  //abort -> do not save settings
  if (itemSettings.exec() != QDialog::Accepted)
  {
      return;
  }
  else
  {
    if (Database::getInstance()->dbIsOk())
    {
      Database::getInstance()->getDatabase()->open();
      QSqlQuery query (*Database::getInstance()->getDatabase());
      //set values
      query.prepare("UPDATE Eigentuemerversammlungen SET Protokollvorlage =:newBodyText WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvnr");
      query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
      query.bindValue(":year", Database::getInstance()->getCurrentYear());
      query.bindValue(":etvnr", Database::getInstance()->getCurrentEtvNumber());
      query.bindValue(":newBodyText", itemSettings.getBodyText());
      query.exec();
      Database::getInstance()->getDatabase()->close();

      //updateReportTemplateTable();
    }
  }
}

void OrderTab::on_editButton_clicked()
{
  if (0 != ui->tableAgenda->selectionModel())
  {
    if (ui->tableAgenda->selectionModel()->selectedRows().count() == 1)
    {
      int selectedRow = ui->tableAgenda->selectionModel()->selection().indexes().value(0).row();
      int id = ui->tableAgenda->model()->index(selectedRow,0).data().toInt();

      changeAgendaItemSettings(id);
    }
    else
    {
      QMessageBox::information(this, "Fehler", "Es wurde kein Eintrag ausgewählt.");
    }
  }
  else
      QMessageBox::information(this, "Fehler", "Interner Fehler");
}
