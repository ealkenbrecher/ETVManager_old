#include "agendaitemsettings.h"
#include "ui_agendaitemsettings.h"
#include "database.h"
#include <QSql>
#include <QMessageBox>
#include "qsqlquerymodelrichtext.h"
#include "stringreplacer.h"

AgendaItemSettings::AgendaItemSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AgendaItemSettings)
{
    ui->setupUi(this);
    m_model = 0;
    refresh ();
}

AgendaItemSettings::~AgendaItemSettings()
{
    delete ui;

  if (0 != m_model)
    delete m_model;
}

int AgendaItemSettings::getItemType ()
{
  if (ui->mitBeschlussoption->isChecked())
    return 0;
  if (ui->ohneBeschlussoption->isChecked())
    return 2;

  return 2;
}

void AgendaItemSettings::setItemType (int aType)
{
  if (0 == aType)
    ui->mitBeschlussoption->setChecked(true);
  else if (2 == aType)
    ui->ohneBeschlussoption->setChecked(true);
}

void AgendaItemSettings::refresh ()
{
  //load patterns
  QSqlDatabase* db = Database::getInstance()->getDatabase();

  if (0 != db)
  {
    //verfuegbare Vorlagen aus Datenbank holen
    db->open();
    QSqlQuery query (*db);
    query.prepare("SELECT bezeichnung, id from AgendaPatterns ORDER BY bezeichnung ASC");
    query.exec();

    m_model = new QSqlQueryModelRichtext ();

    if(query.isActive())
    {
      m_model->setQuery(query);
      //Comboxbox mit Daten fuellen
      ui->patterns->setModel(m_model);
    }
    else
    {
      qDebug() << "query is not active";
    }
    db->close();
  }
}

QString AgendaItemSettings::getHeader ()
{
    return ui->header->toHtml();
}

QString AgendaItemSettings::getDescription ()
{
  if ("" == ui->description->toHtml())
    return "";

  return ui->description->toHtml();
}

QString AgendaItemSettings::getSuggestion ()
{
  if ("" == ui->suggestion->toHtml())
    return "";

  return ui->suggestion->toHtml();
}

QString AgendaItemSettings::getSuggestion2 ()
{
  if ("" == ui->suggestion2->toHtml())
    return "";

  return ui->suggestion2->toHtml();
}

QString AgendaItemSettings::getSuggestion3 ()
{
  if ("" == ui->suggestion3->toHtml())
    return "";

  return ui->suggestion3->toHtml();
}

void AgendaItemSettings::setHeader (QString rValue)
{
    ui->header->setText(rValue);
}

void AgendaItemSettings::setDescription (QString rValue)
{
    ui->description->setText(rValue);
}

void AgendaItemSettings::setSuggestion (QString rValue)
{
    ui->suggestion->setText(rValue);
}

void AgendaItemSettings::setSuggestion2 (QString rValue)
{
    ui->suggestion2->setText(rValue);
}

void AgendaItemSettings::setSuggestion3 (QString rValue)
{
    ui->suggestion3->setText(rValue);
}

void AgendaItemSettings::on_insertPattern_clicked()
{
  //check model pointer
  if (0 != ui->patterns->model())
  {
    //get id
    QModelIndex index = ui->patterns->model()->index(ui->patterns->currentIndex(), 1);
    int patternId = index.data ().toInt();

    //get values
    if (Database::getInstance()->dbIsOk())
    {
        //Liegenschaften aus Datenbank holen
        Database::getInstance()->getDatabase()->open();
        QSqlQuery query (*Database::getInstance()->getDatabase());
        query.prepare("SELECT ueberschrift, beschreibung, beschlussvorschlag, beschlussvorschlag2, beschlussvorschlag3, beschlussArt from AgendaPatterns WHERE id=:patternId");
        query.bindValue(":patternId", patternId);
        query.exec();

        if (query.next())
        {
            //header
            setHeader (replaceWildcards(query.value(0).toString()));

            //descr
            setDescription (replaceWildcards(query.value(1).toString()));

            //proposal
            setSuggestion (replaceWildcards(query.value(2).toString()));

            //proposal2
            setSuggestion2 (replaceWildcards(query.value(3).toString()));

            //proposal3
            setSuggestion3 (replaceWildcards(query.value(4).toString()));

            //set checkboxes
            int beschlussArt = query.value(5).toInt ();
            if (0 == beschlussArt || 1 == beschlussArt)
              ui->mitBeschlussoption->setChecked(true);
            else
              ui->ohneBeschlussoption->setChecked(true);
        }
        Database::getInstance()->getDatabase()->close();
    }
    else
      qDebug() << "db error";
  }
}

QString AgendaItemSettings::replaceWildcards (QString in)
{
  //check if the string contains any wildcards at all
  if (in.contains ("%"))
  {
      //not too smart to do a database query here, but for now ok:
      Database::getInstance()->getDatabase()->open();
      QSqlQuery query (*Database::getInstance()->getDatabase());

      //get years
      query.prepare("SELECT * FROM Eigentuemerversammlungen WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvNum");
      query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
      query.bindValue(":year", Database::getInstance()->getCurrentYear());
      query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
      query.exec();

      QString etvNum;
      QString etvDate;
      QString etvTime;
      QString etvLoc;

      if (query.next())
      {
          etvNum = query.value(2).toString();
          etvDate = query.value(3).toString();
          etvTime = query.value(4).toString();
          etvLoc = query.value(5).toString();
      }

      if (in.contains("%VergangenesWirtschaftsjahr%"))
      {
          int lastYear = Database::getInstance()->getCurrentYear() - 1;
          in.replace(QString("%VergangenesWirtschaftsjahr%"), QString::number (lastYear));
      }

      if (in.contains("%Wirtschaftsjahr%"))
      {
          int year = Database::getInstance()->getCurrentYear();
          in.replace(QString("%Wirtschaftsjahr%"), QString::number (year));
      }

      if (in.contains("%DatumHeute%"))
      {
          QDate curDate = QDate::currentDate();
          in.replace(QString("%DatumHeute%"), curDate.toString("dddd, dd.MM.yyyy"));
      }

      if (in.contains("%NächstesWirtschaftsjahr%"))
      {
          int year = Database::getInstance()->getCurrentYear() + 1;
          in.replace(QString("%NächstesWirtschaftsjahr%"), QString::number(year));
      }

      if (in.contains("%ETVNummer%"))
      {
          in.replace(QString("%ETVNummer%"), etvNum);
      }

      if (in.contains("%ETVDatum%"))
      {
          in.replace(QString("%ETVDatum%"), etvDate);
      }

      if (in.contains("%ETVUhrzeit%"))
      {
          in.replace(QString("%ETVUhrzeit%"), etvTime);
      }

      if (in.contains("%DatumNachAnfechtungsfrist%"))
      {
          QDate date = QDate::fromString(etvDate,"dd.MM.yyyy");
          date = date.addMonths(1);
          in.replace(QString("%DatumNachAnfechtungsfrist%"), date.toString("dd.MM.yyyy"));
      }

      if (in.contains("%ZahlungslaufNachAnfechtungsfrist%"))
      {
          QDate date = QDate::fromString(etvDate,"dd.MM.yyyy");
          date = date.addMonths(1);

          if (date.day() >= 3)
            date = date.addMonths(1);

          in.replace(QString("%ZahlungslaufNachAnfechtungsfrist%"), date.toString("MMMM yyyy"));
      }

      in.replace ("%BeiratVorsitz%", StringReplacer::getInstance()->getReplacementString("%BeiratVorsitz%"));
      in.replace ("%BeiratMitglied1%", StringReplacer::getInstance()->getReplacementString("%BeiratMitglied1%"));
      in.replace ("%BeiratMitglied2%", StringReplacer::getInstance()->getReplacementString("%BeiratMitglied2%"));
      in.replace(QString("%BeiratVorsitz%"), "-");
      in.replace(QString("%BeiratMitglied1%"), "-");
      in.replace(QString("%BeiratMitglied2%"), "-");

      Database::getInstance()->getDatabase()->close();
  }
  return in;
}
