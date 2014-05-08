#include "decissionlibrarytab.h"
#include "ui_decissionlibrarytab.h"
#include "qsqlquerymodelrichtext.h"
#include <QMessageBox>
#include "database.h"
#include "decissionlibraryeditdialog.h"

DecissionLibraryTab::DecissionLibraryTab(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::DecissionLibraryTab)
{
  ui->setupUi(this);

  mQueryModel = 0;
  mQueryModelReportTemplate = 0;
}

DecissionLibraryTab::~DecissionLibraryTab()
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

void DecissionLibraryTab::refreshOnSelected ()
{
  if (!this->isEnabled())
  {
    QMessageBox::information(this, "Fehler", "Alle Einstellungen deaktiviert.\nLegen Sie zunächst ein Wirtschaftsjahr und eine Eigentümerversammlung an");
    return;
  }

  updateAgendaTable();

  ui->tableAgenda->setColumnWidth(0, 40);

  int stretch = ui->tableAgenda->width() - ui->tableAgenda->columnWidth(0) - ui->tableAgenda->columnWidth(1) - ui->tableAgenda->columnWidth(2);
  if (stretch > 0)
  {
      ui->tableAgenda->setColumnWidth(1, ui->tableAgenda->columnWidth(1) + stretch);
  }
}

void DecissionLibraryTab::updateAgendaTable ()
{
  QSqlDatabase* db = Database::getInstance()->getDatabase();
  if (0 != db)
  {
    QSqlQuery query (*db);
    query.prepare("SELECT top_id, header FROM Beschluesse WHERE beschlussArt IS NOT 2 AND obj_id = :id AND wi_jahr = :year AND etv_nr = :etvnr");
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

void DecissionLibraryTab::on_tableAgenda_doubleClicked(const QModelIndex &index)
{
  int top_id = ui->tableAgenda->model()->index(index.row(),0).data().toInt();
  changeAgendaItemSettings (top_id);
}

void DecissionLibraryTab::changeAgendaItemSettings (int aId)
{
  DecissionLibraryEditDialog editDlg (this);

  QSqlDatabase* db = Database::getInstance()->getDatabase();
  if (0 != db)
  {
    QSqlQuery query (*db);
    query.prepare("SELECT beschlussArt, beschlusssammlungVermerke, beschlusssammlungGerichtsentscheidung, beschlusssammlungPlusBeschreibung, beschlussArt FROM Beschluesse WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvnr AND top_id = :top_id");
    query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
    query.bindValue(":year", Database::getInstance()->getCurrentYear());
    query.bindValue(":etvnr", Database::getInstance()->getCurrentEtvNumber());
    query.bindValue(":top_id", aId);
    query.exec();

    if (query.next())
    {
      editDlg.setVoteType(query.value(0).toInt ());
      editDlg.setAnnotation(query.value(1).toString());
      editDlg.setCourtOrder(query.value(2).toString());
      editDlg.setAddDescriptionFlag (query.value(3).toBool());
      editDlg.setVoteType (query.value(4).toInt());
    }
  }

  if (editDlg.exec() == QDialog::Accepted)
  {
    if (Database::getInstance()->dbIsOk())
    {
      QSqlQuery query (*Database::getInstance()->getDatabase());
      //set values
      query.prepare("UPDATE Beschluesse SET beschlusssammlungVermerke =:beschlussVermerke, beschlusssammlungGerichtsentscheidung =:beschlussGericht, beschlusssammlungPlusBeschreibung =:plusBeschreibung WHERE obj_id = :id AND wi_jahr = :year AND top_id = :topid AND etv_nr = :etvnum");
      query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
      query.bindValue(":year", Database::getInstance()->getCurrentYear());
      query.bindValue(":etvnum", Database::getInstance()->getCurrentEtvNumber());
      query.bindValue(":topid", aId);
      query.bindValue(":beschlussVermerke", editDlg.getAnnotation());
      query.bindValue(":beschlussGericht", editDlg.getCourtOrder());
      query.bindValue(":plusBeschreibung", editDlg.getAddDescriptionFlag());

      query.exec();

      updateAgendaTable();
    }
  }
}

void DecissionLibraryTab::on_editEntry_clicked()
{
  if (0 != ui->tableAgenda->selectionModel())
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
  else
    QMessageBox::information(this, "Fehler", "Interner Fehler");
}
