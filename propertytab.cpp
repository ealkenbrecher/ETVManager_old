#include "propertytab.h"
#include "ui_propertytab.h"
#include "database.h"
#include <QtSql>
#include "propertysettings.h"
#include "agendasettings.h"
#include "editadvisersdialog.h"
#include <QMessageBox>
#include "stringreplacer.h"

PropertyTab::PropertyTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PropertyTab)
{
    ui->setupUi(this);

    connect (this, SIGNAL (validEtvSelected (bool)), parent, SLOT (etvSelectionValid (bool)));

    if (0 == ui->liegenschaftSelector->count())
    {
      disableUi (true);
    }
    else
    {
      disableUi (false);
    }
}

PropertyTab::~PropertyTab()
{
    delete ui;
}

void PropertyTab::disableUi (bool aValue)
{
  ui->liegenschaftSelector->setDisabled(aValue);
  ui->editAdvisers->setDisabled(aValue);
  ui->editPropertySettings->setDisabled(aValue);
  ui->years->setDisabled(aValue);
  ui->etvs->setDisabled(aValue);
  ui->changeAgendaSettings->setDisabled(aValue);
  ui->newAgenda->setDisabled(aValue);
  ui->deleteAgenda->setDisabled(aValue);
}

void PropertyTab::refreshOnSelected ()
{
  refreshRealEstateSelector (true);
  refreshView ();
}

void PropertyTab::refreshView ()
{
  updatePropertyNameBox ();
  updateYearBox ();
  updateEtvBox ();
  updateInfoBox ();

  if (0 == ui->liegenschaftSelector->count())
  {
    disableUi (true);
  }
  else
    disableUi (false);
}

void PropertyTab::refreshRealEstateSelector (bool aKeepSelectedIndex)
{
  if (Database::getInstance()->dbIsOk())
  {
    //Liegenschaften aus Datenbank holen
    QSqlQuery query (*Database::getInstance()->getDatabase());
    query.prepare("SELECT obj_name,obj_id from Objekt ORDER BY obj_name ASC");
    query.exec();

    QSqlQueryModel *model = new QSqlQueryModel;

    if(query.isActive())
    {
      int curIndex = 0;

      if (true == aKeepSelectedIndex)
      {
        curIndex = ui->liegenschaftSelector->currentIndex();
      }

      model->setQuery(query);
      //Comboxbox mit Liegenschaften fuellen
      ui->liegenschaftSelector->setModel(model);

      if (aKeepSelectedIndex && curIndex > 0)
      {
        ui->liegenschaftSelector->setCurrentIndex(curIndex);
      }
    }
    else
    {
        qDebug() << "query is not active";
    }
  }
  if (0 == ui->liegenschaftSelector->count())
  {
    disableUi (true);
    QMessageBox::information(this, "Information", "Es sind keine Liegenschaften verfügbar.\nAlle Dialogfelder wurden deaktiviert.\nBitte Datenbankeinstellungen prüfen oder neue Liegenschaft anlegen.");
  }
  else
  {
    disableUi (false);
    setCurrentPropertyId ();
  }
}

void PropertyTab::setCurrentPropertyId ()
{
    //get currently selected entry
    QString currentObject;
    QModelIndex index = ui->liegenschaftSelector->model()->index(ui->liegenschaftSelector->currentIndex(), 1);
    Database::getInstance()->setCurrentPropertyId (index.data ().toInt());
}

void PropertyTab::on_editPropertySettings_clicked()
{
    PropertySettings dialog (this);

    //get values
    QSqlQuery query (*Database::getInstance()->getDatabase());
    query.prepare("SELECT * FROM Objekt WHERE obj_id = :id");
    query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
    query.exec();

    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery(query);

    dialog.setPropertyName (model->record(0).value(1).toString());
    dialog.setMea(model->record(0).value(2).toInt());
    dialog.setVotingRule(model->record(0).value(3).toString());
    dialog.setOwnerQuantity(model->record(0).value(4).toInt());
    dialog.setInvitationDeadline(model->record(0).value(5).toString());
    dialog.setpropertyId(Database::getInstance()->getCurrentPropertyId());

    //int oldVotingRule = model->record(0).value(3).toInt();

    //abort -> do not save settings
    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }
    else
    {
      QMessageBox::StandardButton reply = QMessageBox::question(this, "Achtung", "Das nachträgliche Ändern der Eigenschaften einer Liegenschaft wird nicht empfohlen.\nMöchten Sie wirklich fortfahren?", QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes)
      {
        if (Database::getInstance()->dbIsOk())
        {
            //set values
            QSqlQuery query (*Database::getInstance()->getDatabase());
            query.prepare("UPDATE Objekt SET obj_name=:name, obj_mea=:mea, obj_stimmrecht=:votingtype, obj_anz_et=:ownerQuantity, obj_inv_deadline=:deadline WHERE obj_id = :id");
            query.bindValue(":name", dialog.propertyName());
            query.bindValue(":mea", QString::number(dialog.mea()));
            query.bindValue(":votingtype", dialog.votingRule());
            query.bindValue(":ownerQuantity", QString::number(dialog.ownerQuantity()));
            query.bindValue(":deadline", dialog.getInvitationDeadline());
            query.bindValue(":id", QString::number(Database::getInstance()->getCurrentPropertyId()));

            //update string replacement class
            StringReplacer::getInstance()->addPair("%MEAgesamt%", QString::number (dialog.mea()));
            StringReplacer::getInstance()->addPair("%EinladungsfristProtokoll%", dialog.getInvitationDeadline());
            StringReplacer::getInstance()->addPair("%StimmrechtProtokoll%%", dialog.votingRule());

            query.exec();

            refreshView();
            refreshRealEstateSelector (true);
        }
      }
    }
}

void PropertyTab::on_liegenschaftSelector_currentIndexChanged(int index)
{
  setCurrentPropertyId ();
}

void PropertyTab::copyReportPattern ()
{
  QSqlDatabase* db = Database::getInstance()->getDatabase();
  if (0 != db)
  {
    QSqlQuery query (*db);
    query.prepare("SELECT ueberschrift, deckblatt FROM ReportPatterns WHERE id = 3");
    query.exec ();

    QString content;
    QString type;

    if (query.next())
    {
        type = query.value(0).toString();
        content = query.value(1).toString();

        //add wildcards for current year
        query.prepare("SELECT * FROM Objekt WHERE obj_id = :id");
        query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
        query.exec();

        if (query.next())
        {
          content.replace("%StimmrechtProtokoll%", query.value(3).toString());
          content.replace("%EinladungsfristProtokoll%", query.value(5).toString());
          content.replace("%MEAgesamt%", query.value(2).toString());
        }
    }

    query.prepare("UPDATE Eigentuemerversammlungen SET Protokollvorlage =:vorlage, Protokollueberschrift =:vorlagenTyp WHERE obj_id = :id AND wi_jahr =:year AND etv_nr = :etvNum");
    query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
    query.bindValue(":year", Database::getInstance()->getCurrentYear());
    query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
    query.bindValue(":vorlage", content);
    query.bindValue(":vorlagenTyp", type);

    query.exec ();
  }
  else
    QMessageBox::information(this, "Fehler", "Interner Fehler: Deckblattvorlage kann nicht geladen werden.");
}

void PropertyTab::updateInfoBox()
{
  QSqlQuery query (*Database::getInstance()->getDatabase());

  //get common data
  query.prepare("SELECT * FROM Objekt WHERE obj_id = :id");
  query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
  query.exec();

  QString dump;

  QTextDocument plainText;


  if (query.next())
  {
    dump.append("\n_______________________");
    dump.append("\nObjektdaten");
    dump.append("\nID: ");
    dump.append(query.value(0).toString());
    dump.append("\nName: ");
    dump.append(query.value(1).toString());
    dump.append("\nMEA: ");
    dump.append(query.value(2).toString());
    dump.append("\nStimmrecht: \n");
    plainText.setHtml(query.value(3).toString());
    dump.append(plainText.toPlainText());
    dump.append("\nAnzahl Eigentümer: ");
    dump.append(query.value(4).toString());
  }

  //get advisers
  query.prepare("SELECT Beirat_Vorsitz, Beirat_Mitglied1, Beirat_Mitglied2 FROM Beiratsmitglieder WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvNum");
  query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
  query.bindValue(":year", Database::getInstance()->getCurrentYear());
  query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
  query.exec();

  if (query.next())
  {
    dump.append("\n_______________________");
    dump.append("\nBeiratsvorsitz:\t\t");
    dump.append(query.value(0).toString());
    dump.append("\nBeiratsmitglied 1:\t");
    dump.append(query.value(1).toString());
    dump.append("\nBeiratsmitglied 2:\t");
    dump.append(query.value(2).toString());
  }

  //get data etv
  query.prepare("SELECT obj_id, wi_jahr, etv_nr, etv_datum, etv_uhrzeit, etv_ort FROM Eigentuemerversammlungen WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvNum");
  query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
  query.bindValue(":year", Database::getInstance()->getCurrentYear());
  query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
  query.exec();

  if (query.next())
  {
    dump.append("\n_______________________");
    dump.append("\nDaten gewählte ETV");
    dump.append("\nWirtschaftsjahr: ");
    dump.append(query.value(1).toString());
    dump.append("\nETV #: ");
    dump.append(query.value(2).toString());
    dump.append("\nETV Datum: ");
    dump.append(query.value(3).toString());
    dump.append("\nETV Uhrzeit: ");
    dump.append(query.value(4).toString());
    dump.append("\nETV Ort: ");

    plainText.setHtml(query.value(5).toString());
    dump.append(plainText.toPlainText());
  }

  ui->infoBox->setText(dump);
}

void PropertyTab::updatePropertyNameBox ()
{
    //get current property name
    /*QSqlQuery query (*Database::getInstance()->getDatabase());
    query.prepare("SELECT * FROM Objekt WHERE obj_id = :id");
    query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
    query.exec();

    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery(query);

    ui->selectedProperty->setText(model->record(0).value(1).toString());*/
}

void PropertyTab::updateYearBox()
{
    QSqlQuery query (*Database::getInstance()->getDatabase());

    //get years
    query.prepare("SELECT DISTINCT wi_jahr FROM Eigentuemerversammlungen WHERE obj_id = :id ORDER BY wi_jahr DESC");
    query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
    query.exec();

    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery(query);
    ui->years->setModel(model);

    //Database::getInstance()->setCurrentYear(ui->years->currentText().toInt());
}

void PropertyTab::updateEtvBox()
{
    QSqlQuery query (*Database::getInstance()->getDatabase());

    //get etvs
    query.prepare("SELECT etv_nr FROM Eigentuemerversammlungen WHERE obj_id = :id AND wi_jahr = :year ORDER BY etv_nr DESC");
    query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
    query.bindValue(":year", Database::getInstance()->getCurrentYear());
    query.exec();

    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery(query);
    ui->etvs->setModel(model);
}

void PropertyTab::on_newAgenda_clicked()
{
  AgendaSettings newSettings (this);
  newSettings.setEtvLocation(StringReplacer::getInstance()->getReplacementString("%ETVStandardTagungsraum%"));

  //abort -> do not save settings
  if (newSettings.exec() != QDialog::Accepted)
  {
    return;
  }
  else
  {
    if (Database::getInstance()->dbIsOk())
    {
      QSqlQuery query (*Database::getInstance()->getDatabase());
      //set values
      query.prepare("INSERT INTO Eigentuemerversammlungen (obj_id, wi_jahr, etv_nr, etv_datum, etv_uhrzeit, etv_ort) VALUES (:id, :year, :etvnum, :etvdate, :etvtime, :etvloc)");
      query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
      query.bindValue(":year", newSettings.getYear());
      query.bindValue(":etvnum", newSettings.getEtvNum());
      query.bindValue(":etvdate", newSettings.getEtvDate().toString("dd.MM.yyyy"));
      query.bindValue(":etvtime", newSettings.getEtvTime().toString("hh:mm"));
      query.bindValue(":etvloc", newSettings.getEtvLocation());
      query.exec();

      refreshOnSelected ();
      copyReportPattern ();
      checkAdvisers (Database::getInstance()->getCurrentPropertyId(), newSettings.getYear(), newSettings.getEtvNum());
    }
  }
}

void PropertyTab::checkAdvisers (int aPropertyId, int aYear, int aEtvNum)
{
  if (1 == aEtvNum)
  {
    if (Database::getInstance()->dbIsOk())
    {
      QSqlQuery query (*Database::getInstance()->getDatabase());

      //select advisers of 1st etv from the year before
      query.prepare("SELECT * FROM Beiratsmitglieder WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = 1");
      query.bindValue(":id", aPropertyId);
      query.bindValue(":year", aYear - 1);
      query.exec();

      while (query.next())
      {
          QString vorsitz (query.value(3).toString());
          QString mitglied1 (query.value(4).toString());
          QString mitglied2 (query.value(5).toString());
          QString year (QString::number(aYear - 1));

          QString notification;
          notification.append ("Beirat aus Vorjahr (");
          notification.append (year);
          notification.append (") für dieses Jahr für Wiederwahl vormerken?");
          notification.append ("\nVorsitz: ");
          notification.append (vorsitz);
          notification.append ("\nMitglied 1: ");
          notification.append (mitglied1);
          notification.append ("\nMitglied 2: ");
          notification.append (mitglied2);
          notification.append ("\n?");

          QMessageBox::StandardButton reply = QMessageBox::question(this, "Hinweis", notification, QMessageBox::Yes|QMessageBox::No);
          if (reply == QMessageBox::Yes)
          {
            QSqlQuery insertQuery (*Database::getInstance()->getDatabase());
            //set values
            insertQuery.prepare("INSERT INTO Beiratsmitglieder (obj_id, wi_jahr, etv_nr, Beirat_Vorsitz, Beirat_Mitglied1, Beirat_Mitglied2) VALUES (:id, :year, :etvNum, :vorsitz, :mitglied1, :mitglied2)");
            insertQuery.bindValue(":id", aPropertyId);
            insertQuery.bindValue(":year", aYear);
            insertQuery.bindValue(":etvNum", aEtvNum);
            insertQuery.bindValue(":vorsitz", vorsitz);
            insertQuery.bindValue(":mitglied1", mitglied1);
            insertQuery.bindValue(":mitglied2", mitglied2);
            insertQuery.exec();

            //update string replacement class
            StringReplacer::getInstance()->addPair("%BeiratVorsitz%", vorsitz);
            StringReplacer::getInstance()->addPair("%BeiratMitglied1%", mitglied1);
            StringReplacer::getInstance()->addPair("%BeiratMitglied2%", mitglied2);
          }

          updateInfoBox();
      }
    }
  }
}

void PropertyTab::on_liegenschaftSelector_currentTextChanged(const QString &arg1)
{
  updateEtvBox ();
  updateYearBox();
  updateInfoBox ();
}

void PropertyTab::on_etvs_currentTextChanged(const QString &arg1)
{
    Database::getInstance()->setCurrentEtvNumber(arg1.toInt());
}

void PropertyTab::on_changeAgendaSettings_clicked()
{
  QSqlQuery query (*Database::getInstance()->getDatabase());

  AgendaSettings newSettings (this);

  //get years
  query.prepare("SELECT * FROM Eigentuemerversammlungen WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvNum");
  query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
  query.bindValue(":year", Database::getInstance()->getCurrentYear());
  query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
  query.exec();

  if (query.next())
  {
    //causes trouble. todo: update relevant tables to do year or etv# change
    newSettings.disableYearSetting(true);
    newSettings.disableEtvNumSetting(true);

    newSettings.setEtvNum (query.value(2).toInt());
    newSettings.setEtvLocation (query.value(5).toString());
    QDate date = QDate::fromString(query.value(3).toString(),"dd.MM.yyyy");
    newSettings.setEtvDate (date);

    QTime time = QTime::fromString(query.value(4).toString(), "hh:mm");
    newSettings.setEtvTime (time);
  }

  newSettings.setYear (Database::getInstance()->getCurrentYear());

  //abort -> do not save settings
  if (newSettings.exec() != QDialog::Accepted)
  {
      return;
  }
  else
  {
      if (Database::getInstance()->dbIsOk())
      {
          QSqlQuery query (*Database::getInstance()->getDatabase());
          //set values
          query.prepare("UPDATE Eigentuemerversammlungen SET wi_jahr = :newYear, etv_nr = :newEtvNum, etv_datum = :etvdate, etv_uhrzeit = :etvtime, etv_ort = :etvloc WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvNum");
          query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
          query.bindValue(":year", Database::getInstance()->getCurrentYear());
          query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
          query.bindValue(":newYear", newSettings.getYear());
          query.bindValue(":newEtvNum", newSettings.getEtvNum());
          query.bindValue(":etvdate", newSettings.getEtvDate().toString("dd.MM.yyyy"));
          query.bindValue(":etvtime", newSettings.getEtvTime().toString("hh:mm"));
          query.bindValue(":etvloc", newSettings.getEtvLocation());
          query.exec();

          refreshOnSelected ();
      }
  }
}

void PropertyTab::on_deleteAgenda_clicked()
{
    QMessageBox::information(this, "Fehler", "Diese Funktion ist in der aktuellen Softwareversion leider noch nicht verfügbar.");
}

void PropertyTab::on_years_currentTextChanged(const QString &arg1)
{
    Database::getInstance()->setCurrentYear(arg1.toInt());
    updateEtvBox ();
    //updateAgendaTable();
    updateInfoBox ();
}

void PropertyTab::on_etvs_activated(int index)
{
  updateInfoBox();
}


void PropertyTab::on_editAdvisers_clicked()
{
  EditAdvisersDialog dialog (this);

  QSqlDatabase* db = Database::getInstance()->getDatabase();

  if (0 != db)
  {
    QSqlQuery query (*db);

    //get years
    query.prepare("SELECT Beirat_Vorsitz, Beirat_Mitglied1, Beirat_Mitglied2 FROM Beiratsmitglieder WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvNum");
    query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
    query.bindValue(":year", Database::getInstance()->getCurrentYear());
    query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
    query.exec();

    bool noAdviserSet = true;

    while (query.next())
    {
        noAdviserSet = false;
        dialog.setAdviserPresident (query.value(0).toString ());
        dialog.setAdviser1 (query.value(1).toString ());
        dialog.setAdviser2 (query.value(2).toString ());
    }

    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }
    else
    {
        if (Database::getInstance()->dbIsOk())
        {
            QSqlQuery query (*db);

            if (noAdviserSet == false)
            {
              //update values
              query.prepare("UPDATE Beiratsmitglieder SET Beirat_Vorsitz = :vorsitz, Beirat_Mitglied1 = :mitglied1, Beirat_Mitglied2 = :mitglied2 WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvNum");
              query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
              query.bindValue(":year", Database::getInstance()->getCurrentYear());
              query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
              query.bindValue(":vorsitz", dialog.getAdviserPresident());
              query.bindValue(":mitglied1", dialog.getAdviser1());
              query.bindValue(":mitglied2", dialog.getAdviser2());
              query.exec();
            }
            else
            {
              //set values
              query.prepare("INSERT INTO Beiratsmitglieder (obj_id, wi_jahr, etv_nr, Beirat_Vorsitz, Beirat_Mitglied1, Beirat_Mitglied2) VALUES (:id, :year, :etvNum, :vorsitz, :mitglied1, :mitglied2)");
              query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
              query.bindValue(":year", Database::getInstance()->getCurrentYear());
              query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
              query.bindValue(":vorsitz", dialog.getAdviserPresident());
              query.bindValue(":mitglied1", dialog.getAdviser1());
              query.bindValue(":mitglied2", dialog.getAdviser2());
              query.exec();
            }

            updateInfoBox();

            //update string replacement class
            StringReplacer::getInstance()->addPair("%BeiratVorsitz%", dialog.getAdviserPresident());
            StringReplacer::getInstance()->addPair("%BeiratMitglied1%", dialog.getAdviser1());
            StringReplacer::getInstance()->addPair("%BeiratMitglied2%", dialog.getAdviser2());
        }
    }
  }
}

void PropertyTab::on_etvs_currentIndexChanged(const QString &arg1)
{
  if ("" != arg1)
  {
    emit validEtvSelected (true);
  }
  else
    emit validEtvSelected (false);
}
