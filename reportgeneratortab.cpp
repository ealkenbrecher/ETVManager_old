#include "reportgeneratortab.h"
#include "ui_reportgeneratortab.h"
#include <QMessageBox>
#include "wizarddialogbox.h"
#include "database.h"
#include <QtCore>
#include <QTextDocument>
#include "votingdialog.h"
#include "global.h"
#include "qsqlquerymodelrichtext.h"
#include "orderitemsettings.h"
#include "patterneditorreportitemsettings.h"
#include "stringreplacer.h"
#include "decissionitemsettings.h"
#include "databasedecissiontableimpl.h"

ReportGeneratorTab::ReportGeneratorTab(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::ReportGeneratorTab)
{
  ui->setupUi(this);
  mQueryModel = 0;
  mQueryModelReportTemplate = 0;
}

ReportGeneratorTab::~ReportGeneratorTab()
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

void ReportGeneratorTab::refreshOnSelected ()
{
  if (!this->isEnabled())
  {
    QMessageBox::information(this, "Fehler", "Alle Einstellungen deaktiviert.\nLegen Sie zunächst ein Wirtschaftsjahr und eine Eigentümerversammlung an");
    return;
  }

  updateAgendaTable();
  updateReportTemplateTable();

  ui->tableAgenda->setColumnWidth(0, 40);

  int stretch = ui->tableAgenda->width() - ui->tableAgenda->columnWidth(0) - ui->tableAgenda->columnWidth(1) - ui->tableAgenda->columnWidth(2);
  stretch = stretch - 10;
  if (stretch > 0)
  {
      ui->tableAgenda->setColumnWidth(1, ui->tableAgenda->columnWidth(1) + stretch);
  }
}

void ReportGeneratorTab::updateAgendaTable ()
{
  QSqlDatabase* db = Database::getInstance()->getDatabase();

  if (0 != db)
  {
    QString request ("SELECT top_id, header, protokoll_id FROM Beschluesse WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvnr ORDER BY protokoll_id ASC");
    QSqlQuery query (*db);
    query.prepare(request);
    query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
    query.bindValue(":year", Database::getInstance()->getCurrentYear());
    query.bindValue(":etvnr", Database::getInstance()->getCurrentEtvNumber());
    query.exec();

    mQueryModel = new QSqlQueryModelRichtext ();
    mQueryModel->setQuery(query);
    mQueryModel->setHeaderData(0, Qt::Horizontal, tr("Nr."));
    mQueryModel->setHeaderData(1, Qt::Horizontal, tr("Bezeichnung"));

    ui->tableAgenda->setModel(mQueryModel);

    ui->tableAgenda->hideColumn(2);
    ui->tableAgenda->show();
  }
}

void ReportGeneratorTab::updateReportTemplateTable ()
{
  QSqlDatabase* db = Database::getInstance()->getDatabase();
  if (0 != db)
  {
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
  }
}

void ReportGeneratorTab::on_tableAgenda_doubleClicked(const QModelIndex &index)
{
  int top_id = ui->tableAgenda->model()->index(index.row(),0).data().toInt();
  changeAgendaItemSettings (top_id);
}

void ReportGeneratorTab::changeAgendaItemSettings (int aId)
{
  QSqlDatabase* db = Database::getInstance()->getDatabase();
  if (0 != db)
  {
    QSqlQuery query (*db);
    query.prepare("SELECT * FROM Beschluesse WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvnr AND top_id = :topid");
    query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
    query.bindValue(":year", Database::getInstance()->getCurrentYear());
    query.bindValue(":etvnr", Database::getInstance()->getCurrentEtvNumber());
    query.bindValue(":topid", aId);
    query.exec();

    DecissionItemSettings itemSettings (this);

    if (query.next())
    {
      itemSettings.setHeader                    (query.value(5).toString());
      itemSettings.setDescription               (query.value(6).toString());
      itemSettings.setDecissionText             (query.value(7).toString());
      itemSettings.setDecissionProclamation     (query.value(8).toString());
      itemSettings.setYesVotes                  (query.value(9).toFloat());
      itemSettings.setNoVotes                   (query.value(10).toFloat());
      itemSettings.setAbsentionVotes            (query.value(11).toFloat());
      itemSettings.setType                      (query.value(12).toInt());
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
        query.prepare("UPDATE Beschluesse SET header =:newHeader, descr =:newDescr, beschlussformulierung =:newBeschlussformulierung, abstimmergebnis =:newAbstimmergebnis, beschlusssammlungVermerke = :vermerke, stimmenJa =:ja, stimmenNein =:nein, stimmenEnthaltung =:enthaltung WHERE obj_id = :id AND wi_jahr = :year AND top_id = :topid AND etv_nr = :etvnum");
        query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
        query.bindValue(":year", Database::getInstance()->getCurrentYear());
        query.bindValue(":etvnum", Database::getInstance()->getCurrentEtvNumber());
        query.bindValue(":topid", aId);
        query.bindValue(":newHeader", itemSettings.getHeader());
        query.bindValue(":newDescr", itemSettings.getDescription());
        query.bindValue(":newBeschlussformulierung", itemSettings.getDecissionText());
        query.bindValue(":newAbstimmergebnis", itemSettings.getDecissionProclamation());
        query.bindValue(":ja", itemSettings.getYesVotes());
        query.bindValue(":nein", itemSettings.getNoVotes());
        query.bindValue(":enthaltung", itemSettings.getAbsentionVotes());
        query.bindValue(":vermerke", itemSettings.getDecissionProclamation());
        //query.bindValue(":type", itemSettings.getType());

        query.exec();

        updateAgendaTable();
      }
    }
  }
}

void ReportGeneratorTab::on_tableReportTemplate_doubleClicked(const QModelIndex &index)
{
  PatternEditorReportItemSettings itemSettings (this);

  QSqlQuery query (*Database::getInstance()->getDatabase());
  query.prepare("SELECT Protokollueberschrift, Protokollabschrift FROM Eigentuemerversammlungen WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvnr");
  query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
  query.bindValue(":year", Database::getInstance()->getCurrentYear());
  query.bindValue(":etvnr", Database::getInstance()->getCurrentEtvNumber());
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
          query.prepare("UPDATE Eigentuemerversammlungen SET Protokollabschrift =:newBodyText WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvnr");
          query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
          query.bindValue(":year", Database::getInstance()->getCurrentYear());
          query.bindValue(":etvnr", Database::getInstance()->getCurrentEtvNumber());
          query.bindValue(":newBodyText", itemSettings.getBodyText());
          query.exec();

          //updateReportTemplateTable();
      }
  }
}

void ReportGeneratorTab::startAgendaWizard()
{
  if (Database::getInstance()->dbIsOk())
  {
    QSqlQuery query (*Database::getInstance()->getDatabase());

    query.prepare("SELECT top_id, top_header, top_descr, top_vorschlag, top_vorschlag2, top_vorschlag3, beschlussArt FROM Tagesordnungspunkte WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvNum");
    query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
    query.bindValue(":year", Database::getInstance()->getCurrentYear());
    query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
    query.exec();

    int retVal = QDialog::Accepted;

    QString sug1 ("");
    QString sug2 ("");
    QString sug3 ("");
    QString header ("");
    QString descr ("");
    int type = 0;

    QSqlQuery query2 (*Database::getInstance()->getDatabase());
    query2.prepare("SELECT obj_stimmrecht FROM Objekt WHERE obj_id = :id");
    query2.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
    query2.exec();

    int votingType;
    votingType = -1;

    if (query2.next())
    {
      votingType = query2.value(0).toInt();
    }

    QString agendaDecission ("");

    int top_id = -1;

    while (query.next() && QDialog::Accepted == retVal)
    {
      //get decission
      top_id  = query.value(0).toInt();
      header  = query.value(1).toString();
      descr   = query.value(2).toString();
      sug1    = query.value(3).toString();
      sug2    = query.value(4).toString();
      sug3    = query.value(5).toString();
      type    = query.value(6).toInt();

      WizardDialogBox dialog (this, header.toStdString().c_str(), eComplexDialog);

      //check for old decission ->

      QSqlQuery queryOldDecission (*Database::getInstance()->getDatabase());
      queryOldDecission.prepare("SELECT *  FROM Beschluesse WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvNum AND top_id = :top_id");
      queryOldDecission.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
      queryOldDecission.bindValue(":year", Database::getInstance()->getCurrentYear());
      queryOldDecission.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
      queryOldDecission.bindValue(":top_id", top_id);

      bool ok = queryOldDecission.exec();
      bool foundOldRecord = false;

      if (ok && queryOldDecission.next())
      {
        foundOldRecord = true;
        dialog.setSavedSuggestion(queryOldDecission.value(7).toString());
      }
      //<- check for Old decission

      if ("" != sug1)
        dialog.setSuggestion1(sug1);

      if ("" != sug2)
        dialog.setSuggestion2(sug2);

      if ("" != sug3)
        dialog.setSuggestion3(sug3);

      //get voting
      //VotingDialog* votingDlg = new VotingDialog (this, votingType);

      //set old Values ->
      if (foundOldRecord)
      {
        dialog.setVotePreview (queryOldDecission.value(8).toString());
        dialog.setVotingsYes (queryOldDecission.value(9).toFloat());
        dialog.setVotingsNo (queryOldDecission.value(10).toFloat());
        dialog.setVotingsConcordant (queryOldDecission.value(11).toFloat());
      }
      //<- set old Values

      retVal = dialog.exec();

      if (QDialog::Accepted == retVal && -1 != top_id)
      {
        QSqlQuery query3 (*Database::getInstance()->getDatabase());

        //get cover page
        query3.prepare("SELECT top_id FROM Beschluesse WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvNum AND top_id = :top_id");
        query3.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
        query3.bindValue(":year", Database::getInstance()->getCurrentYear());
        query3.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
        query3.bindValue(":top_id", top_id);
        ok = query3.exec();

        bool resultFound = false;
        //record found -> update query
        if (query3.next())
        {
          resultFound = true;

          //set values
          query3.prepare("UPDATE Beschluesse SET beschlussformulierung = :formulierung, header = :headerText, descr = :description, abstimmergebnis = :ergebnis, stimmenJa = :ja, stimmenNein = :nein, stimmenEnthaltung = :enthaltung, beschlusssammlungVermerke = :vermerke, beschlussArt =:type WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvNum AND top_id = :top_id");
        }

        //no record found -> insert query
        if (!resultFound)
        {
          query3.prepare("INSERT INTO Beschluesse (obj_id, wi_jahr, etv_nr, top_id, beschlussformulierung, header, descr, abstimmergebnis, stimmenJa, stimmenNein, stimmenEnthaltung, protokoll_id, beschlusssammlungVermerke, beschlussArt) VALUES (:id, :year, :etvNum, :top_id, :formulierung, :headerText, :description, :ergebnis, :ja, :nein, :enthaltung, :protokollid, :vermerke, :type)");
        }

        query3.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
        query3.bindValue(":year", Database::getInstance()->getCurrentYear());
        query3.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
        query3.bindValue(":top_id", top_id);
        query3.bindValue(":protokollid", top_id);
        query3.bindValue(":headerText", header);
        query3.bindValue(":description", descr);
        query3.bindValue(":formulierung", dialog.getResult());
        query3.bindValue(":ergebnis", dialog.getVoteText());
        query3.bindValue(":ja", dialog.getVotingsYes());
        query3.bindValue(":nein", dialog.getVotingsNo());
        query3.bindValue(":enthaltung", dialog.getVotingsConcordant());
        query3.bindValue(":vermerke", dialog.getVoteText());
        query3.bindValue(":type", type);
        query3.exec ();
      }
    }
  }
}

void ReportGeneratorTab::startCoverpageWizard()
{
  if (Database::getInstance()->dbIsOk())
  {
    QSqlQuery query (*Database::getInstance()->getDatabase());

    //get cover page
    query.prepare("SELECT Protokollvorlage FROM Eigentuemerversammlungen WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvNum");
    query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
    query.bindValue(":year", Database::getInstance()->getCurrentYear());
    query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
    query.exec();

    bool coverPageFound = false;
    int retCode = QDialog::Accepted;

    QString coverPageText;

    while (query.next() && retCode == QDialog::Accepted)
    {
      coverPageText = query.value(0).toString();
      coverPageFound = true;

      if (coverPageText.contains("%DatumEinladungsschreiben%"))
      {
        retCode = processWizardDialog ("Datum des Einladungsschreibens?", "%DatumEinladungsschreiben%");
        if (retCode != QDialog::Accepted)
          break;
      }
      if (coverPageText.contains("%UhrzeitStartVersammlung%"))
      {
        retCode = processWizardDialog ("Wann wurde die Versammlung eröffnet?", "%UhrzeitStartVersammlung%");
        if (retCode != QDialog::Accepted)
          break;
      }
      if (coverPageText.contains("%UhrzeitEndeVersammlung%"))
      {
        retCode = processWizardDialog ("Wann wurde die Versammlung beendet?", "%UhrzeitEndeVersammlung%");
        if (retCode != QDialog::Accepted)
          break;
      }
      if (coverPageText.contains("%BeschlussfaehigMarker%"))
      {
        QStringList list;
        list << "Ja";
        retCode = processWizardDialog ("Wurde die Beschlussfähigkeit geprüft und\nwaren mehr als 50% der MEA durch Anwesenheit\noder Vollmachten vertreten?", "%BeschlussfaehigMarker%", eCheckBoxDialog, false, list);
        if (retCode != QDialog::Accepted)
          break;
      }
      if (coverPageText.contains("%MEAAnwesend%"))
      {
        retCode = processWizardDialog ("Wie viele MEA waren zur Eröffnung\nder Versammlung anwesend?", "%MEAAnwesend%");
        if (retCode != QDialog::Accepted)
          break;
      }
      if (coverPageText.contains("%Versammlungsleiter%"))
      {
        retCode = processWizardDialog ("Wer war der Versammlungsleiter?", "%Versammlungsleiter%");
        if (retCode != QDialog::Accepted)
          break;
      }

      //this is a bit tricky
      mMarkerVersammlungsleiterFktSet = false;
      mMarkerProtokollfuehrerFktSet = false;

      if (coverPageText.contains("%VersammlungsleiterFktVerwalterMarker%") && mMarkerVersammlungsleiterFktSet == false)
      {
        QStringList list;
        list << "Ja";
        retCode = processWizardDialog ("War der Versammlungsleiter der Verwalter?", "%VersammlungsleiterFktVerwalterMarker%", eCheckBoxDialog, false, list);
        if (retCode != QDialog::Accepted)
          break;
      }
      if (coverPageText.contains("%VersammlungsleiterFktMitarbeiterMarker%") && mMarkerVersammlungsleiterFktSet == false)
      {
        QStringList list;
        list << "Ja";
        retCode = processWizardDialog ("War der Versammlungsleiter ein Mitarbeiter der Verwaltung?", "%VersammlungsleiterFktMitarbeiterMarker%", eCheckBoxDialog, false, list);
        if (retCode != QDialog::Accepted)
          break;
      }
      if (coverPageText.contains("%VersammlungsleiterFktWEGMarker%") && mMarkerVersammlungsleiterFktSet == false)
      {
        QStringList list;
        list << "Ja";
        retCode = processWizardDialog ("War der Versammlungsleiter ein durch die WEG bestellter Vertreter?", "%VersammlungsleiterFktWEGMarker%", eCheckBoxDialog, false, list);
        if (retCode != QDialog::Accepted)
          break;
      }

      if (coverPageText.contains("%Protokollfuehrung%"))
      {
        retCode = processWizardDialog ("Wer hat das Protokoll geschrieben?", "%Protokollfuehrung%");
        if (retCode != QDialog::Accepted)
          break;
      }

      if (coverPageText.contains("%ProtokollfuehrerFktChef%") && mMarkerProtokollfuehrerFktSet == false)
      {
        QStringList list;
        list << "Ja";
        retCode = processWizardDialog ("War der Protokollführer der Verwalter?", "%ProtokollfuehrerFktChef%", eCheckBoxDialog, false, list);
        if (retCode != QDialog::Accepted)
          break;
      }
      if (coverPageText.contains("%ProtokollfuehrerFktMitarbeiter%") && mMarkerProtokollfuehrerFktSet == false)
      {
        QStringList list;
        list << "Ja";
        retCode = processWizardDialog ("War der Protokollführer ein Mitarbeiter der Verwaltung?", "%ProtokollfuehrerFktMitarbeiter%", eCheckBoxDialog, false, list);
        if (retCode != QDialog::Accepted)
          break;
      }
      if (coverPageText.contains("%ProtokollfuehrerFktWEG%") && mMarkerProtokollfuehrerFktSet == false)
      {
        QStringList list;
        list << "Ja";
        retCode = processWizardDialog ("War der Protokollführer ein durch die WEG bestellter Vertreter?", "%ProtokollfuehrerFktWEG%", eCheckBoxDialog, false, list);
        if (retCode != QDialog::Accepted)
          break;
      }

      if (coverPageText.contains("%Abstimmungsregelung%"))
      {
        QStringList list;
        list << "Handzeichen";
        list << "Auszählung der Stimmen";
        retCode = processWizardDialog ("Wie erfolgte die Abstimmung?", "%Abstimmungsregelung%", eCheckBoxDialog, false, list);
        if (retCode != QDialog::Accepted)
          break;
      }

      //ask for advisers
      QMessageBox::StandardButton reply = QMessageBox::question(this, "Protokollunterschrift", "Wurde das Protokoll unterschrieben?", QMessageBox::Yes|QMessageBox::No);
      int i = 0;
      while (reply == QMessageBox::Yes)
      {
        i++;
        QString nameNumber ("%Protokollunterschrift");
        nameNumber.append (QString::number(i));
        nameNumber.append ("%");
        retCode = processWizardDialog("Wer hat das Protokoll unterschrieben (Name)?", nameNumber);

        if (retCode == QDialog::Accepted)
        {
          QString nameFktNumber ("%ProtokollunterschriftFkt");
          nameFktNumber.append (QString::number(i));
          nameFktNumber.append ("%");
          retCode = processWizardDialog("In welcher Funktion (Beirat, Eigentümer, Verwaltung...)?", nameFktNumber);
        }

        reply = QMessageBox::question(this, "Protokollunterschrift", "Wurde das Protokoll von einer weiteren Person unterschrieben?", QMessageBox::Yes|QMessageBox::No);
      }
    }

    //replace %Einladungsfrist%
    query.prepare("SELECT obj_inv_deadline FROM Objekt WHERE obj_id = :id");
    query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
    query.exec();

    if (query.next())
    {
      if (2 == query.value(0).toInt())
        StringReplacer::getInstance()->addPair("%Einladungsfrist%", "24 Abs. 4  WEG (2 Wochen)");
      else
      {
        QString value;
        value.append("Vereinbarung gemäß Gemeinschaftsordnung: ");
        value.append (QString::number(query.value(0).toInt()));
        value.append (" Wochen");

        StringReplacer::getInstance()->addPair("%Einladungsfrist%", value);
      }
    }

    //find and replace generic wildcards
    //StringReplacer::getInstance()->findAndReplaceWildcards(coverPageText);

    if (false == coverPageFound)
      QMessageBox::information(this, "Fehler", "Interner Fehler: kein Protokolldeckblatt gefunden.");
    else
    {
      QMessageBox::StandardButton reply = QMessageBox::question(this, "Fertig!", "Änderungen speichern?", QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes)
      {
        coverPageText = StringReplacer::getInstance()->findAndReplaceWildcards(coverPageText);

        //set values
        query.prepare("UPDATE Eigentuemerversammlungen SET Protokollabschrift =:report WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvNum");
        query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
        query.bindValue(":year", Database::getInstance()->getCurrentYear());
        query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
        query.bindValue(":report", coverPageText);
        query.exec();
      }
    }
  }
}

void ReportGeneratorTab::on_startWizard_clicked()
{
  startCoverpageWizard ();
}

int ReportGeneratorTab::processWizardDialog (QString aDialogText, QString aWildcard, eDlgType aType, bool aMultipleChoice, QStringList aList)
{
  WizardDialogBox dialog (this, 0, aType);
  dialog.setDialogText (aDialogText);
  int retVal = QDialog::Rejected;

  switch (aType)
  {
    case eCheckBoxDialog:
      if (0 != aList.size())
      {
        qDebug () << aList.size();

        if (aList.size() > 3)
          qDebug () << "too many arguments";
        //else if (aList.contains(aWildcard))
        //  qDebug () << "wildcard already known";
        else
        {
          int i;
          for (i = 0; i < aList.size(); i++)
          {
            dialog.setCheckBox ((eCheckBoxIndex)i, aList.at(i));
            dialog.setMultipleChoice (aMultipleChoice);
          }

          retVal = dialog.exec();
          if (QDialog::Accepted == retVal)
          {
            if (aWildcard == "%BeschlussfaehigMarker%")
            {
                if (Qt::Checked == dialog.getCheckState(eCheckBox1))
                {
                  StringReplacer::getInstance()->addPair(aWildcard, "X");
                }
                else
                {
                  StringReplacer::getInstance()->addPair(aWildcard, " ");
                }
            }
            if (aWildcard == "%VersammlungsleiterFktVerwalterMarker%")
            {
              if (Qt::Checked == dialog.getCheckState(eCheckBox1))
              {
                StringReplacer::getInstance()->addPair(aWildcard, "X");
                StringReplacer::getInstance()->addPair("%VersammlungsleiterFktMitarbeiterMarker%", " ");
                StringReplacer::getInstance()->addPair("%VersammlungsleiterFktWEGMarker%", " ");
                mMarkerVersammlungsleiterFktSet = true;
              }
              else
              {
                StringReplacer::getInstance()->addPair(aWildcard, " ");
                mMarkerVersammlungsleiterFktSet = false;
              }
            }
            if (aWildcard == "%VersammlungsleiterFktMitarbeiterMarker%")
            {
              if (Qt::Checked == dialog.getCheckState(eCheckBox1))
              {
                StringReplacer::getInstance()->addPair(aWildcard, "X");
                StringReplacer::getInstance()->addPair("%VersammlungsleiterFktVerwalterMarker%", " ");
                StringReplacer::getInstance()->addPair("%VersammlungsleiterFktWEGMarker%", " ");
                mMarkerVersammlungsleiterFktSet = true;
              }
              else
              {
                StringReplacer::getInstance()->addPair(aWildcard, " ");
                mMarkerVersammlungsleiterFktSet = false;
              }
            }
            if (aWildcard == "%VersammlungsleiterFktWEGMarker%")
            {
              if (Qt::Checked == dialog.getCheckState(eCheckBox1))
              {
                StringReplacer::getInstance()->addPair(aWildcard, "X");
                StringReplacer::getInstance()->addPair("%VersammlungsleiterFktMitarbeiterMarker%", " ");
                StringReplacer::getInstance()->addPair("%VersammlungsleiterFktVerwalterMarker%", " ");
                mMarkerVersammlungsleiterFktSet = true;
              }
              else
              {
                StringReplacer::getInstance()->addPair(aWildcard, " ");
                mMarkerVersammlungsleiterFktSet = false;
              }
            }
            if (aWildcard == "%ProtokollfuehrerFktChef%")
            {
              if (Qt::Checked == dialog.getCheckState(eCheckBox1))
              {
                StringReplacer::getInstance()->addPair(aWildcard, "X");
                StringReplacer::getInstance()->addPair("%ProtokollfuehrerFktMitarbeiter%", " ");
                StringReplacer::getInstance()->addPair("%ProtokollfuehrerFktWEG%", " ");
                mMarkerProtokollfuehrerFktSet = true;
              }
              else
              {
                StringReplacer::getInstance()->addPair(aWildcard, " ");
                mMarkerProtokollfuehrerFktSet = false;
              }
            }
            if (aWildcard == "%ProtokollfuehrerFktMitarbeiter%")
            {
              if (Qt::Checked == dialog.getCheckState(eCheckBox1))
              {
                StringReplacer::getInstance()->addPair(aWildcard, "X");
                StringReplacer::getInstance()->addPair("%ProtokollfuehrerFktChef%", " ");
                StringReplacer::getInstance()->addPair("%ProtokollfuehrerFktWEG%", " ");
                mMarkerProtokollfuehrerFktSet = true;
              }
              else
              {
                StringReplacer::getInstance()->addPair(aWildcard, " ");
                mMarkerProtokollfuehrerFktSet = false;
              }
            }
            if (aWildcard == "%ProtokollfuehrerFktWEG%")
            {
              if (Qt::Checked == dialog.getCheckState(eCheckBox1))
              {
                StringReplacer::getInstance()->addPair(aWildcard, "X");
                StringReplacer::getInstance()->addPair("%ProtokollfuehrerFktChef%", " ");
                StringReplacer::getInstance()->addPair("%ProtokollfuehrerFktMitarbeiter%", " ");
                mMarkerProtokollfuehrerFktSet = true;
              }
              else
              {
                StringReplacer::getInstance()->addPair(aWildcard, " ");
                mMarkerProtokollfuehrerFktSet = false;
              }
            }
            if (aWildcard == "%Abstimmungsregelung%")
            {
              if (Qt::Checked == dialog.getCheckState(eCheckBox1))
              {
                StringReplacer::getInstance()->addPair(aWildcard, "Handzeichen");
              }
              else if (Qt::Checked == dialog.getCheckState(eCheckBox2))
              {
                StringReplacer::getInstance()->addPair(aWildcard, "Auszählung der Stimmen");
              }
            }
          }
          else
          {
            retVal = QDialog::Rejected;
          }
        }
      }
      break;
    case eComplexDialog:
    case eSimpleDialog:
      retVal = dialog.exec();
      if (QDialog::Accepted == retVal)
      {
        if ("" != aWildcard)
        {
          StringReplacer::getInstance()->addPair(aWildcard, dialog.getResult ());
        }
      }
      else
      {
        retVal = QDialog::Rejected;
      }
      break;
    default:
      break;
  }
  return retVal;
}

void ReportGeneratorTab::on_startAgendaWizard_clicked()
{
   startAgendaWizard();
   updateAgendaTable();
}

void ReportGeneratorTab::on_addEntry_clicked()
{
  QString aDialogText ("Antrag zur Änderung der Geschäftsordnung");
  WizardDialogBox dialog (this, 0, eOrderDialog);
  dialog.setDialogText (aDialogText);
  dialog.setResult("Antrag zur Änderung der Geschäftsordnung:");

  //abort -> do not save settings
  if (dialog.exec() == QDialog::Accepted)
  {
    if (Database::getInstance()->dbIsOk())
    {
      //get highest id
      QSqlQuery query (*Database::getInstance()->getDatabase());
      query.prepare ("SELECT top_id FROM Beschluesse WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvNum ORDER BY top_id DESC");
      query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
      query.bindValue(":year", Database::getInstance()->getCurrentYear());
      query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
      query.exec();

      int maxIndex = -1;

      if (query.next())
        maxIndex = query.value(0).toInt();

      if (-1 != maxIndex)
      {
        query.prepare("INSERT INTO Beschluesse (obj_id, wi_jahr, etv_nr, top_id, protokoll_id, header, descr, beschlussArt) VALUES (:id, :year, :etvNum, :topid, :protokollid, :headerText, :description, :beschlussArt)");
        query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
        query.bindValue(":year", Database::getInstance()->getCurrentYear());
        query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
        query.bindValue(":headerText", aDialogText);
        query.bindValue(":description", dialog.getResult());
        query.bindValue(":topid", maxIndex+1);
        query.bindValue(":protokollid", maxIndex+1);
        query.bindValue(":beschlussArt", 2);
        query.exec ();

        updateAgendaTable();
      }
    }
  }
}

void ReportGeneratorTab::on_editEntry_clicked()
{
  if (0 != ui->tableAgenda->selectionModel())
  {
    {
      int selectedRow = ui->tableAgenda->selectionModel()->selection().indexes().value(0).row();
      int top_id = ui->tableAgenda->model()->index(selectedRow,0).data().toInt();

      changeAgendaItemSettings(top_id);
    }
  }
  else
    QMessageBox::information(this, "Fehler", "Interner Fehler");
}

void ReportGeneratorTab::on_moveAgendaItemUp_clicked()
{
  int selectedRow = ui->tableAgenda->selectionModel()->selection().indexes().value(0).row();
  int protokoll_id = ui->tableAgenda->model()->index(selectedRow,2).data().toInt();

  if (1 == protokoll_id)
  {
    QMessageBox::information(this, "Fehler", "Beschluss kann nicht nach oben verschoben werden.");
  }
  else
  {
      if (Database::getInstance()->dbIsOk())
      {
          QSqlQuery query (*Database::getInstance()->getDatabase());
          //set the id of the top before to -999
          query.prepare("UPDATE Beschluesse SET protokoll_id = -999 WHERE obj_id = :id AND wi_jahr = :year AND protokoll_id = :protokollid AND etv_nr = :etvnum");
          query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
          query.bindValue(":year", Database::getInstance()->getCurrentYear());
          query.bindValue(":etvnum", Database::getInstance()->getCurrentEtvNumber());
          query.bindValue(":protokollid", (protokoll_id - 1));
          query.exec();

          //set the chosen top to the correct id
          query.prepare("UPDATE Beschluesse SET protokoll_id = :protokollid_new WHERE obj_id = :id AND wi_jahr = :year AND protokoll_id =:protokollid AND etv_nr = :etvnum");
          query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
          query.bindValue(":year", Database::getInstance()->getCurrentYear());
          query.bindValue(":etvnum", Database::getInstance()->getCurrentEtvNumber());
          query.bindValue(":protokollid", protokoll_id);
          query.bindValue(":protokollid_new", protokoll_id-1);
          query.exec();

          //set the '-999' topid to the correct topid
          query.prepare("UPDATE Beschluesse SET protokoll_id = :protokollid WHERE obj_id = :id AND wi_jahr = :year AND protokoll_id = -999 AND etv_nr = :etvnum");
          query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
          query.bindValue(":year", Database::getInstance()->getCurrentYear());
          query.bindValue(":etvnum", Database::getInstance()->getCurrentEtvNumber());
          query.bindValue(":protokollid", protokoll_id);
          query.exec();

          updateAgendaTable();
          ui->tableAgenda->selectRow(protokoll_id - 2);
      }
  }
}

void ReportGeneratorTab::on_moveAgendaItemDown_clicked()
{
  int selectedRow = ui->tableAgenda->selectionModel()->selection().indexes().value(0).row();
  int protokoll_id = ui->tableAgenda->model()->index(selectedRow,2).data().toInt();

  int rows = ui->tableAgenda->model()->rowCount();
  if (rows == selectedRow + 1 || rows == protokoll_id)
    QMessageBox::information(this, "Fehler", "Beschluss kann nicht nach unten verschoben werden.");
  else
  {
    if (Database::getInstance()->dbIsOk())
    {
        QSqlQuery query (*Database::getInstance()->getDatabase());
        //set the id of the top before to -999
        query.prepare("UPDATE Beschluesse SET protokoll_id = -999 WHERE obj_id = :id AND wi_jahr = :year AND protokoll_id = :protokollid AND etv_nr = :etvnum");
        query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
        query.bindValue(":year", Database::getInstance()->getCurrentYear());
        query.bindValue(":etvnum", Database::getInstance()->getCurrentEtvNumber());
        query.bindValue(":protokollid", (protokoll_id + 1));
        query.exec();

        //set the chosen top to the correct id
        query.prepare("UPDATE Beschluesse SET protokoll_id = :protokollid_new WHERE obj_id = :id AND wi_jahr = :year AND protokoll_id =:protokollid AND etv_nr = :etvnum");
        query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
        query.bindValue(":year", Database::getInstance()->getCurrentYear());
        query.bindValue(":etvnum", Database::getInstance()->getCurrentEtvNumber());
        query.bindValue(":protokollid", protokoll_id);
        query.bindValue(":protokollid_new", protokoll_id + 1);
        query.exec();

        //set the '-999' topid to the correct topid
        query.prepare("UPDATE Beschluesse SET protokoll_id = :protokollid WHERE obj_id = :id AND wi_jahr = :year AND protokoll_id = -999 AND etv_nr = :etvnum");
        query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
        query.bindValue(":year", Database::getInstance()->getCurrentYear());
        query.bindValue(":etvnum", Database::getInstance()->getCurrentEtvNumber());
        query.bindValue(":protokollid", protokoll_id);
        query.exec();

        updateAgendaTable();
        ui->tableAgenda->selectRow(protokoll_id);
    }
  }
}

void ReportGeneratorTab::on_deleteEntry_clicked()
{
  QMessageBox::StandardButton reply = QMessageBox::question(this, "Achtung", "Beschluss wirklich löschen?", QMessageBox::Yes|QMessageBox::No);
  if (reply == QMessageBox::Yes)
  {
      int selectedRow = ui->tableAgenda->selectionModel()->selection().indexes().value(0).row();
      int top_id = ui->tableAgenda->model()->index(selectedRow, 0).data().toInt();

      QSqlQuery query (*Database::getInstance()->getDatabase());
      query.prepare("DELETE FROM Beschluesse WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvnr AND top_id = :topid");

      query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
      query.bindValue(":year", Database::getInstance()->getCurrentYear());
      query.bindValue(":etvnr", Database::getInstance()->getCurrentEtvNumber());
      query.bindValue(":topid", top_id);
      query.exec();

      //update top ids
      query.prepare("SELECT top_id FROM Beschluesse WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvnr AND top_id > :topid");
      query.bindValue(":topid", top_id);
      query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
      query.bindValue(":year", Database::getInstance()->getCurrentYear());
      query.bindValue(":etvnr", Database::getInstance()->getCurrentEtvNumber());
      query.exec();

      int old_Id = 0;
      int new_Id = 0;
      while (query.next ())
      {
          old_Id = query.value(0).toInt();
          new_Id = old_Id - 1;

          query.prepare("UPDATE Beschluesse SET top_id = :newid WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvnr AND top_id = :oldid");
          query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
          query.bindValue(":year", Database::getInstance()->getCurrentYear());
          query.bindValue(":etvnr", Database::getInstance()->getCurrentEtvNumber());
          query.bindValue(":oldid", old_Id);
          query.bindValue(":newid", new_Id);
          query.exec();

          query.prepare("SELECT top_id FROM Beschluesse WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvnr AND top_id > :topid");
          query.bindValue(":topid", new_Id);
          query.exec();
      }

      updateAgendaTable();
  }
}

void ReportGeneratorTab::on_addDecission_clicked()
{
  QSqlDatabase* db = Database::getInstance()->getDatabase();
  if (0 != db)
  {
    DecissionItemSettings itemSettings (this);
    itemSettings.setHeader (QString("Schriftlicher Beschluss"));
    itemSettings.setType(1);

    //abort -> do not save settings
    if (itemSettings.exec() != QDialog::Accepted)
    {
        return;
    }
    else
    {
      if (Database::getInstance()->dbIsOk())
      {
        //get highest id
        QSqlQuery query (*Database::getInstance()->getDatabase());
        query.prepare ("SELECT top_id FROM Beschluesse WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvNum ORDER BY top_id DESC");
        query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
        query.bindValue(":year", Database::getInstance()->getCurrentYear());
        query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
        query.exec();

        int maxIndex = -1;

        if (query.next())
          maxIndex = query.value(0).toInt();

        if (-1 != maxIndex)
        {
          query.prepare("INSERT INTO Beschluesse (obj_id, wi_jahr, etv_nr, top_id, protokoll_id, header, descr, beschlussformulierung, abstimmergebnis, beschlusssammlungVermerke, stimmenJa, stimmenNein, stimmenEnthaltung, beschlussArt) VALUES (:id, :year, :etvNum, :topid, :protokollid, :headerText, :description, :formulierung, :abstimmergebnis, :vermerke, :ja, :nein, :enthaltung, :beschlussArt)");
          query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
          query.bindValue(":year", Database::getInstance()->getCurrentYear());
          query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
          query.bindValue(":headerText", itemSettings.getHeader());
          query.bindValue(":description", itemSettings.getDescription());
          query.bindValue(":topid", maxIndex+1);
          query.bindValue(":protokollid", maxIndex+1);
          query.bindValue(":formulierung", itemSettings.getDecissionText());
          query.bindValue(":abstimmergebnis", itemSettings.getDecissionProclamation());
          query.bindValue(":ja", itemSettings.getYesVotes());
          query.bindValue(":nein", itemSettings.getNoVotes());
          query.bindValue(":enthaltung", itemSettings.getAbsentionVotes());
          query.bindValue(":vermerke", itemSettings.getDecissionProclamation());
          query.bindValue(":beschlussArt", 1);

          query.exec ();

          updateAgendaTable();
        }
      }
    }
  }
}
