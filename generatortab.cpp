#include "generatortab.h"
#include "ui_generatortab.h"
#include "database.h"
#include <QTextDocument>
#include <QTextEdit>
#include <QTextDocumentWriter>
#include <QPrinter>
#include <QFileDialog>
#include <QTextFrame>
#include <QMessageBox>
#include <stringreplacer.h>
#include "wizarddialogbox.h"

GeneratorTab::GeneratorTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeneratorTab)
{
    ui->setupUi(this);
}

GeneratorTab::~GeneratorTab()
{
    delete ui;
}

void GeneratorTab::refreshOnSelected()
{
  if (!this->isEnabled())
  {
    QMessageBox::information(this, "Fehler", "Alle Einstellungen deaktiviert.\nLegen Sie zunächst ein Wirtschaftsjahr und eine Eigentümerversammlung an");
    return;
  }
}

void GeneratorTab::on_doOdt_clicked()
{
  //tagesordnung generieren
  if (Qt::Checked == ui->agenda->checkState())
  {
      ;//generateAgendaAsOdt ();
  }
}

void GeneratorTab::setTableFormat (QTextTableFormat* pTableFormat)
{
  if (0 != pTableFormat)
  {
    QBrush blackBrush(Qt::SolidPattern);
    pTableFormat->setBorderBrush(blackBrush);
    pTableFormat->setBorder(5);
    pTableFormat->setCellSpacing(0);
    pTableFormat->setCellPadding(5);
    pTableFormat->setHeaderRowCount(0);
    pTableFormat->setBorderStyle(QTextFrameFormat::BorderStyle_Groove);
    pTableFormat->setWidth(QTextLength(QTextLength::PercentageLength, 100));
  }
}

QString GeneratorTab::generateProcurationAsPdf (const QString &rFilePath)
{
  QSqlDatabase* db = Database::getInstance()->getDatabase();
  if (0 != db)
  {
      QSqlQuery query (*Database::getInstance()->getDatabase());

      //get years
      query.prepare("SELECT * FROM Eigentuemerversammlungen WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvNum");
      query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
      query.bindValue(":year", Database::getInstance()->getCurrentYear());
      query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
      query.exec();

      QString text;
      QTextDocument* doc = new QTextDocument ();
      QTextCursor cursor (doc);
      QTextTableFormat tableFormat;
      setTableFormat (&tableFormat);

      while (query.next())
      {
        //insert header text
        cursor.movePosition(QTextCursor::End);

        cursor.insertHtml (StringReplacer::getInstance()->getReplacementString("%vollmachtHeader%"));
        cursor.insertHtml ("<br>");

        cursor.insertTable (3, 1, tableFormat);
        cursor.insertHtml("<b>V O L L M A C H T</b>");
        cursor.movePosition(QTextCursor::NextRow);
        cursor.insertText("Eigentümerversammlung ");
        cursor.insertText(query.value(2).toString());
        cursor.insertText("/");
        cursor.insertText(query.value(1).toString());
        cursor.insertText(" der WEG ");

        QSqlQuery queryPropertyName (*Database::getInstance()->getDatabase());
        //get current property name
        queryPropertyName.prepare("SELECT * FROM Objekt WHERE obj_id = :id");
        queryPropertyName.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
        queryPropertyName.exec();

        if (queryPropertyName.next())
        {
          cursor.insertText(queryPropertyName.value(1).toString());
        }

        cursor.insertHtml("<br>");
        cursor.insertText("Datum: ");
        cursor.insertText(query.value(3).toString());
        cursor.insertHtml("<br>");
        cursor.insertText("Uhrzeit: ");
        cursor.insertText(query.value(4).toString());

        cursor.movePosition(QTextCursor::NextRow);
        cursor.insertText (StringReplacer::getInstance()->getReplacementString("%vollmachtBevollmaechtigter%"));
        cursor.movePosition(QTextCursor::End);

        cursor.insertHtml ("<br>");
      }

      //insert agenda items
      query.prepare("SELECT top_id, top_header, top_descr, top_vorschlag, top_vorschlag2, top_vorschlag3, beschlussArt FROM Tagesordnungspunkte WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvnr");
      query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
      query.bindValue(":year", Database::getInstance()->getCurrentYear());
      query.bindValue(":etvnr", Database::getInstance()->getCurrentEtvNumber());
      query.exec();

      while (query.next())
      {
          QString topNumber (query.value(0).toString());
          QString topHeader (query.value(1).toString());
          //QString topDescription (query.value(2).toString());
          QString topSuggestion (query.value(3).toString());
          QString topSuggestion2 (query.value(4).toString());
          QString topSuggestion3 (query.value(5).toString());
          int type (query.value(6).toInt());

          QTextDocument htmlConverter;

          htmlConverter.setHtml(topSuggestion);
          QString topSuggestionPlain = htmlConverter.toPlainText();

          htmlConverter.setHtml(topSuggestion2);
          QString topSuggestion2Plain = htmlConverter.toPlainText();

          htmlConverter.setHtml(topSuggestion3);
          QString topSuggestion3Plain = htmlConverter.toPlainText();

          //count needed rows for table
          int neededRows = 8;
          if ("" == topSuggestionPlain)
            neededRows--;
          if ("" == topSuggestion2Plain)
            neededRows--;
          if ("" == topSuggestion3Plain)
            neededRows--;

          cursor.movePosition(QTextCursor::End);
          tableFormat.setHeaderRowCount(neededRows);
          cursor.insertTable (neededRows, 1, tableFormat);

          cursor.insertHtml("TOP ");
          cursor.insertHtml(topNumber);
          cursor.insertHtml(": ");
          cursor.insertHtml(topHeader);

          if ("" != topSuggestionPlain)
          {
              cursor.movePosition(QTextCursor::NextRow);
              cursor.insertHtml("[ ] Weisung gemäß Beschlussvorschlag");
          }

          if ("" != topSuggestion2Plain)
          {
              cursor.movePosition(QTextCursor::NextRow);
              cursor.insertHtml("[ ] Weisung gemäß Beschlussvorschlag Alternative 1");
          }

          if ("" != topSuggestion3Plain)
          {
              cursor.movePosition(QTextCursor::NextRow);
              cursor.insertHtml("[ ] Weisung gemäß Beschlussvorschlag Alternative 2");
          }

          cursor.movePosition(QTextCursor::NextRow);

          if (2 != type)
            cursor.insertHtml("[ ] Weisung gemäß folgendem Wortlaut:");
          else
            cursor.insertHtml("Anmerkung:<br>");

          //cursor.insertHtml("<hr><br><hr>");

          cursor.movePosition(QTextCursor::End);
          cursor.insertHtml ("<br>");
      }

      cursor.insertTable (4, 1, tableFormat);
      cursor.insertHtml ("Datum:");
      cursor.movePosition(QTextCursor::NextRow);
      cursor.insertHtml ("Wohnung Nr.:");
      cursor.movePosition(QTextCursor::NextRow);
      cursor.insertHtml ("Eigentümername (Druckbuchstaben):");
      cursor.movePosition(QTextCursor::NextRow);
      cursor.insertHtml ("Unterschrift:");

      QPrinter printer(QPrinter::HighResolution);
      printer.setPageSize(QPrinter::A4);
      printer.setOutputFormat(QPrinter::PdfFormat);

      QFileDialog fileDialog;
      fileDialog.setDirectory(rFilePath);
      QString file = fileDialog.getSaveFileName(this, tr("Speichern als PDF"), "/tmp/Vollmacht.pdf", tr("PDF Datei (*.pdf)"));

      printer.setOutputFileName(file);

      doc->print(&printer);
      delete doc;

      return QFileInfo(file).path();
  }
  return "";
}

QString GeneratorTab::generateAgendaAsPdf (const QString &rFilePath)
{
    QSqlDatabase* db = Database::getInstance()->getDatabase();
    if (0 != db)
    {
        QSqlQuery query (*Database::getInstance()->getDatabase());

        //get years
        query.prepare("SELECT * FROM Eigentuemerversammlungen WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvNum");
        query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
        query.bindValue(":year", Database::getInstance()->getCurrentYear());
        query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
        query.exec();

        QString text;
        QTextDocument* doc = new QTextDocument ();
        QTextCursor cursor (doc);
        QTextTableFormat tableFormat;
        setTableFormat (&tableFormat);

        if (query.next())
        {
            //insert header text
            cursor.movePosition(QTextCursor::End);
            cursor.insertTable (2, 1, tableFormat);
            cursor.insertHtml("<b>T A G E S O R D N U N G</b>");
            cursor.movePosition(QTextCursor::NextRow);
            cursor.insertText("Eigentümerversammlung ");
            cursor.insertText(query.value(2).toString());
            cursor.insertText("/");
            cursor.insertText(query.value(1).toString());
            cursor.insertText(" der WEG ");

            QSqlQuery queryPropertyName (*Database::getInstance()->getDatabase());
            //get current property name
            queryPropertyName.prepare("SELECT * FROM Objekt WHERE obj_id = :id");
            queryPropertyName.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
            queryPropertyName.exec();

            while (queryPropertyName.next())
            {
              cursor.insertText(queryPropertyName.value(1).toString());
            }

            cursor.insertHtml("<br><b>Ort: </b>");
            cursor.insertHtml(query.value(5).toString());
            cursor.insertHtml("<br><b>Datum: </b>");
            cursor.insertText(query.value(3).toString());
            cursor.insertHtml(", <b>Uhrzeit: </b>");
            cursor.insertText(query.value(4).toTime().toString());
            cursor.movePosition(QTextCursor::End);
            cursor.insertText("\n\n");
        }

        //insert agenda items
        query.prepare("SELECT top_id, top_header, top_descr, top_vorschlag, top_vorschlag2, top_vorschlag3 FROM Tagesordnungspunkte WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvnr");
        query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
        query.bindValue(":year", Database::getInstance()->getCurrentYear());
        query.bindValue(":etvnr", Database::getInstance()->getCurrentEtvNumber());
        query.exec();

        while (query.next())
        {
            QString topNumber (query.value(0).toString());
            QString topHeader (query.value(1).toString());
            QString topDescription (query.value(2).toString());
            QString topSuggestion (query.value(3).toString());
            QString topSuggestion2 (query.value(4).toString());
            QString topSuggestion3 (query.value(5).toString());

            QTextDocument htmlConverter;

            htmlConverter.setHtml(topDescription);
            QString topDescriptionPlain = htmlConverter.toPlainText();

            htmlConverter.setHtml(topSuggestion);
            QString topSuggestionPlain = htmlConverter.toPlainText();

            htmlConverter.setHtml(topSuggestion2);
            QString topSuggestion2Plain = htmlConverter.toPlainText();

            htmlConverter.setHtml(topSuggestion3);
            QString topSuggestion3Plain = htmlConverter.toPlainText();

            //count needed rows for table
            int neededRows = 5;
            if ("" == topDescriptionPlain)
              neededRows--;
            if ("" == topSuggestionPlain)
              neededRows--;
            if ("" == topSuggestion2Plain)
              neededRows--;
            if ("" == topSuggestion3Plain)
              neededRows--;

            cursor.movePosition(QTextCursor::End);
            tableFormat.setHeaderRowCount(neededRows);
            cursor.insertTable (neededRows, 1, tableFormat);

            cursor.insertText("TOP ");
            cursor.insertHtml(topNumber);
            cursor.insertText(": ");
            cursor.insertHtml(topHeader);

            if ("" != topDescriptionPlain)
            {
                cursor.movePosition(QTextCursor::NextRow);
                cursor.insertHtml(topDescription);
            }

            if ("" != topSuggestionPlain)
            {
                cursor.movePosition(QTextCursor::NextRow);
                cursor.insertHtml("<b>Beschlussvorschlag:</b><br>");
                cursor.insertHtml(topSuggestion);
            }

            if ("" != topSuggestion2Plain)
            {
                cursor.movePosition(QTextCursor::NextRow);
                cursor.insertHtml("<b>Beschlussvorschlag Alternative 1:</b><br>");
                cursor.insertHtml(topSuggestion2);
            }

            if ("" != topSuggestion3Plain)
            {
                cursor.movePosition(QTextCursor::NextRow);
                cursor.insertHtml("<b>Beschlussvorschlag Alternative 2:</b><br>");
                cursor.insertHtml(topSuggestion3);
            }

            cursor.movePosition(QTextCursor::End);
            cursor.insertText("\n\n");
        }

        QPrinter printer(QPrinter::HighResolution);
        printer.setPageSize(QPrinter::A4);
        printer.setOutputFormat(QPrinter::PdfFormat);

        QFileDialog fileDialog;
        fileDialog.setDirectory(rFilePath);
        QString file = fileDialog.getSaveFileName(this, tr("Speichern als PDF"), "/tmp/Tagesordnung.pdf", tr("PDF Datei (*.pdf)"));

        printer.setOutputFileName(file);

        doc->print(&printer);
        delete doc;

        return QFileInfo(file).path();
    }
  return "";
}

QString GeneratorTab::generateReportTemplateAsPdf (const QString &rFilePath)
{
  QSqlDatabase* db = Database::getInstance()->getDatabase();
  if (0 != db)
  {
      QSqlQuery query (*Database::getInstance()->getDatabase());

      //get years
      query.prepare("SELECT * FROM Eigentuemerversammlungen WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvNum");
      query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
      query.bindValue(":year", Database::getInstance()->getCurrentYear());
      query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
      query.exec();

      QString text;
      QTextDocument* doc = new QTextDocument ();
      QTextCursor cursor (doc);

      QTextTableFormat tableFormat;
      setTableFormat (&tableFormat);
      //tableFormat.setPageBreakPolicy (QTextFormat::PageBreak_AlwaysAfter);

      if (query.next())
      {
          //insert header text
          cursor.movePosition(QTextCursor::End);
          cursor.insertTable (2, 1, tableFormat);

          cursor.insertHtml("<b>P R O T O K O L L</b>");
          cursor.movePosition(QTextCursor::NextRow);
          cursor.insertText("Der WEG ");

          QSqlQuery query2 (*Database::getInstance()->getDatabase());
          //get current property name
          query2.prepare("SELECT * FROM Objekt WHERE obj_id = :id");
          query2.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
          query2.exec();

          while (query2.next())
          {
              cursor.insertText(query2.value(1).toString());
          }

          cursor.insertHtml("<br><b>Ort: </b>");
          cursor.insertHtml(query.value(5).toString());
          cursor.insertHtml("<br><b>Datum: </b>");
          cursor.insertText(query.value(3).toString());
          cursor.movePosition(QTextCursor::End);
          cursor.insertText("\n\n");

          //get report cover page
          query2.prepare("SELECT Protokollvorlage FROM Eigentuemerversammlungen WHERE obj_id = :id AND wi_jahr =:year AND etv_nr =:etvNum");
          query2.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
          query2.bindValue(":year", Database::getInstance()->getCurrentYear());
          query2.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
          query2.exec();

          cursor.movePosition(QTextCursor::End);
          cursor.insertText("\n");

          while (query2.next())
            cursor.insertHtml(replaceCoverPageTagsByUnderscore (query2.value(0).toString()));

          cursor.movePosition(QTextCursor::End);
          cursor.insertText("\n");
      }

      //insert agenda items
      query.prepare("SELECT top_id, top_header, top_descr, top_vorschlag, top_vorschlag2, top_vorschlag3, LeerzeilenProtokoll, beschlussArt FROM Tagesordnungspunkte WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvnr");
      query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
      query.bindValue(":year", Database::getInstance()->getCurrentYear());
      query.bindValue(":etvnr", Database::getInstance()->getCurrentEtvNumber());
      query.exec();

      cursor.movePosition(QTextCursor::End);
      tableFormat.setPageBreakPolicy (QTextFormat::PageBreak_AlwaysBefore);
      cursor.insertTable(1,1,tableFormat);
      cursor.insertHtml("<b>B E S C H L U E S S E</b>");
      cursor.movePosition(QTextCursor::End);
      cursor.insertText("\n\n");

      while (query.next())
      {
          QString descriptionText = query.value(2).toString ();
          QString beschlussvorschlag = query.value(3).toString();
          QString beschlussvorschlagAlt1 = query.value(4).toString();
          QString beschlussvorschlagAlt2 = query.value(5).toString();
          int beschlussArt = query.value(7).toInt();

          QTextDocument htmlConverter;

          htmlConverter.setHtml(beschlussvorschlag);
          QString beschlussvorschlagPlain = htmlConverter.toPlainText();

          htmlConverter.setHtml(beschlussvorschlagAlt1);
          QString beschlussvorschlagAlt1Plain = htmlConverter.toPlainText();

          htmlConverter.setHtml(beschlussvorschlagAlt2);
          QString beschlussvorschlagAlt2Plain = htmlConverter.toPlainText();

          htmlConverter.setHtml(descriptionText);
          QString descriptionTextPlain = htmlConverter.toPlainText();

          int spacerLines = query.value(6).toInt();

          //count needed rows for table

          int rowsVotingText = 0;

          if (2 != beschlussArt)
            rowsVotingText = 5;

          int neededRows = 6 + spacerLines + rowsVotingText;
          if ("" == beschlussvorschlagPlain)
            neededRows--;
          if ("" == beschlussvorschlagAlt1Plain)
            neededRows--;
          if ("" == beschlussvorschlagAlt2Plain)
            neededRows--;
          if ("" == descriptionTextPlain)
            neededRows--;

          cursor.movePosition(QTextCursor::End);
          tableFormat.setPageBreakPolicy(QTextFormat::PageBreak_Auto);
          tableFormat.setHeaderRowCount(neededRows);
          cursor.insertTable (neededRows, 1, tableFormat);
          cursor.insertHtml("TOP ");
          cursor.insertHtml(query.value(0).toString());
          cursor.insertText(": ");
          cursor.insertHtml(query.value(1).toString());

          if ("" != descriptionTextPlain)
          {
            cursor.movePosition(QTextCursor::NextRow);
            cursor.insertHtml(query.value(2).toString());
          }

          if ("" != beschlussvorschlagPlain)
          {
            cursor.movePosition(QTextCursor::NextRow);
            cursor.insertHtml("<b>Beschlussvorschlag:</b><br>");
            cursor.insertHtml(beschlussvorschlag);
          }

          if ("" != beschlussvorschlagAlt1Plain)
          {
              cursor.movePosition(QTextCursor::NextRow);
              cursor.insertHtml("<b>Beschlussvorschlag Alternative 1:</b><br>");
              cursor.insertHtml(beschlussvorschlagAlt1);
          }
          if ("" != beschlussvorschlagAlt2Plain)
          {
              cursor.movePosition(QTextCursor::NextRow);
              cursor.insertHtml("<b>Beschlussvorschlag Alternative 2:</b><br>");
              cursor.insertHtml(beschlussvorschlagAlt2);
          }

          cursor.movePosition(QTextCursor::NextRow);

          if (2 != beschlussArt)
            cursor.insertHtml("<b>Beschluss:</b>");
          else
            cursor.insertHtml("<b>Anmerkungen:</b>");

          for (int i = 0; i < spacerLines; i++)
            cursor.movePosition(QTextCursor::NextRow);

          if (2 != beschlussArt)
          {
            cursor.insertHtml("<b>Abstimmung<b>");
            cursor.movePosition(QTextCursor::NextRow);
            cursor.insertText("Ja-Stimmen: ");
            cursor.movePosition(QTextCursor::NextRow);
            cursor.insertText("Nein-Stimmen: ");
            cursor.movePosition(QTextCursor::NextRow);
            cursor.insertText("Enthaltungen: ");
            cursor.movePosition(QTextCursor::NextRow);
            cursor.insertHtml("<b>Verkündung durch den Versammlungsleiter:<b>");
          }

          cursor.movePosition(QTextCursor::End);
          cursor.insertText("\n\n");
      }

      cursor.insertText("Die Richtigkeit und Vollständigkeit des Protokolls wurde geprüft durch folgende zur Versammlung anwesende Personen:\n\n");
      cursor.insertText("Unterschrift Miteigentümer 1: _______________________, Name in Druckbuchstaben: _______________________\n\n");
      cursor.insertText("Unterschrift Miteigentümer 2: _______________________, Name in Druckbuchstaben: _______________________\n\n");
      cursor.insertText("Unterschrift Miteigentümer 3: _______________________, Name in Druckbuchstaben: _______________________\n\n");
      cursor.insertText("Unterschrift Versammlungsleiter: _______________________");

      QPrinter printer(QPrinter::HighResolution);
      printer.setPageSize(QPrinter::A4);
      printer.setOutputFormat(QPrinter::PdfFormat);

      QFileDialog fileDialog;
      fileDialog.setDirectory(rFilePath);
      QString file = fileDialog.getSaveFileName(this, tr("Speichern als PDF"), "/tmp/Protokollvorlage.pdf", tr("PDF Datei (*.pdf)"));

      printer.setOutputFileName(file);
      doc->print(&printer);
      delete doc;

      return QFileInfo(file).path();
  }
  return "";
}

QString GeneratorTab::generateReportTranscriptAsPdf (const QString &rFilePath)
{
  QSqlDatabase* db = Database::getInstance()->getDatabase();
  if (0 != db)
  {
      QSqlQuery query (*Database::getInstance()->getDatabase());

      //get years
      query.prepare("SELECT * FROM Eigentuemerversammlungen WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvNum");
      query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
      query.bindValue(":year", Database::getInstance()->getCurrentYear());
      query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
      query.exec();

      QString text;
      QTextDocument* doc = new QTextDocument ();
      QTextCursor cursor (doc);

      QTextTableFormat tableFormat;
      setTableFormat (&tableFormat);

      if (query.next())
      {
          //insert header text
          cursor.movePosition(QTextCursor::End);
          cursor.insertTable (2, 1, tableFormat);

          cursor.insertHtml("<b>P R O T O K O L L A B S C H R I F T</b>");
          cursor.movePosition(QTextCursor::NextRow);
          cursor.insertText("Der WEG ");

          QSqlQuery query2 (*Database::getInstance()->getDatabase());
          //get current property name
          query2.prepare("SELECT * FROM Objekt WHERE obj_id = :id");
          query2.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
          query2.exec();

          while (query2.next())
          {
              cursor.insertText(query2.value(1).toString());
          }

          cursor.insertHtml("<br><b>Ort: </b>");
          cursor.insertHtml(query.value(5).toString());
          cursor.insertHtml("<br><b>Datum: </b>");
          cursor.insertText(query.value(3).toString());
          cursor.movePosition(QTextCursor::End);
          cursor.insertText("\n\n");

          //get report cover page
          query2.prepare("SELECT Protokollabschrift FROM Eigentuemerversammlungen WHERE obj_id = :id AND wi_jahr =:year AND etv_nr =:etvNum");
          query2.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
          query2.bindValue(":year", Database::getInstance()->getCurrentYear());
          query2.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
          query2.exec();

          cursor.movePosition(QTextCursor::End);
          cursor.insertText("\n");

          if (query2.next())
            cursor.insertHtml(query2.value(0).toString());

          cursor.movePosition(QTextCursor::End);
          cursor.insertText("\n");
      }

      //insert agenda items
      query.prepare("SELECT * FROM Beschluesse WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvnr");
      query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
      query.bindValue(":year", Database::getInstance()->getCurrentYear());
      query.bindValue(":etvnr", Database::getInstance()->getCurrentEtvNumber());
      query.exec();

      cursor.movePosition(QTextCursor::End);
      tableFormat.setPageBreakPolicy (QTextFormat::PageBreak_AlwaysBefore);
      cursor.insertTable(1,1,tableFormat);
      cursor.insertHtml("<b>B E S C H L U E S S E</b>");
      cursor.movePosition(QTextCursor::End);
      cursor.insertText("\n\n");

      while (query.next())
      {
        QString topId         = query.value(3).toString();
        QString protokollId   = query.value(4).toString();
        QString header        = query.value(5).toString();
        QString description   = query.value(6).toString();
        QString decission     = query.value(7).toString();
        QString voteResult    = query.value(8).toString();
        float yes             = query.value(9).toFloat();
        float no              = query.value(10).toFloat();
        float abstention      = query.value(11).toFloat();
        int type              = query.value(12).toInt();

        QTextDocument htmlConverter;

        htmlConverter.setHtml(description);
        QString descriptionPlain = htmlConverter.toPlainText();

        htmlConverter.setHtml(decission);
        QString decissionPlain = htmlConverter.toPlainText();

        htmlConverter.setHtml(voteResult);
        QString voteResultPlain = htmlConverter.toPlainText();

        //count needed rows for table
        int neededRows = 4;

        if ("" == descriptionPlain)
          neededRows--;
        if ("" == decissionPlain)
          neededRows--;
        if ("" == voteResultPlain)
          neededRows--;

        cursor.movePosition(QTextCursor::End);
        tableFormat.setPageBreakPolicy(QTextFormat::PageBreak_Auto);
        tableFormat.setHeaderRowCount(neededRows);
        cursor.insertTable (neededRows, 1, tableFormat);
        cursor.insertHtml("TOP ");
        cursor.insertHtml(topId);
        cursor.insertText(": ");
        cursor.insertHtml(header);

        if ("" != descriptionPlain)
        {
          cursor.movePosition(QTextCursor::NextRow);
          cursor.insertHtml(description);
        }

        if ("" != decissionPlain)
        {
          cursor.movePosition(QTextCursor::NextRow);
          if (2 != type)
          {
            cursor.insertHtml("<b>Beschluss:</b><br>");
          }
          else
          {
            cursor.insertHtml("<b>Anmerkungen:</b><br>");
          }
          cursor.insertHtml(decission);
        }

        if ("" != voteResultPlain)
        {
            cursor.movePosition(QTextCursor::NextRow);
            cursor.insertHtml(voteResult);
        }

        cursor.movePosition(QTextCursor::End);

        if (0 != yes || 0 != no || 0 != abstention)
        {
          cursor.insertTable (1, 3, tableFormat);
          cursor.insertText(StringReplacer::getInstance()->getReplacementString("%StimmenJa%"));
          cursor.insertText(": ");
          cursor.insertHtml(QString::number(yes));

          cursor.movePosition(QTextCursor::NextCell);
          cursor.insertText(StringReplacer::getInstance()->getReplacementString("%StimmenNein%"));
          cursor.insertText(": ");
          cursor.insertHtml(QString::number(no));

          cursor.movePosition(QTextCursor::NextCell);
          cursor.insertText(StringReplacer::getInstance()->getReplacementString("%StimmenEnthaltung%"));
          cursor.insertText(": ");
          cursor.insertHtml(QString::number(abstention));
          cursor.movePosition(QTextCursor::End);
        }
        cursor.insertHtml("<br>");
    }

    cursor.insertHtml("<br><br><br>");
    cursor.insertHtml(StringReplacer::getInstance()->getReplacementString("%protokollabschriftUnterschriftenVorwort%"));
    cursor.insertHtml("<br><br>");

    QString wildcardSignature ("%Protokollunterschrift");
    QString wildcardPerson ("%ProtokollunterschriftFkt");

    int counter = 1;
    bool stop = false;
    QString retVal ("");

    while (!stop)
    {
      QString temp1;
      temp1.append(wildcardSignature);
      temp1.append(QString::number(counter));
      temp1.append("%");

      QString temp2;
      temp2.append(wildcardPerson);
      temp2.append(QString::number(counter));
      temp2.append("%");

      retVal = StringReplacer::getInstance()->getReplacementString(temp1);

      if (retVal == "")
        stop = true;
      else
      {
        cursor.insertText(retVal);
        cursor.insertText (" (");
      }

      retVal = StringReplacer::getInstance()->getReplacementString(temp2);

      if(retVal == "")
        stop = true;
      else
      {
        cursor.insertText(retVal);
        cursor.insertText(")");
        cursor.insertHtml("<br>");
      }

      counter++;
    }


    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPrinter::A4);
    printer.setOutputFormat(QPrinter::PdfFormat);

    QFileDialog fileDialog;
    fileDialog.setDirectory(rFilePath);
    QString file = fileDialog.getSaveFileName(this, tr("Speichern als PDF"), "/tmp/Protokollabschrift.pdf", tr("PDF Datei (*.pdf)"));

    printer.setOutputFileName(file);
    doc->print(&printer);
    delete doc;

    return QFileInfo(file).path();
  }
  return "";
}

QString GeneratorTab::generateDecissionLibraryAsPdf (const QString &rFilePath)
{
  WizardDialogBox dialog (this, 0, eSimpleDialog);
  dialog.setTitle (QString("Beschlusssammlung Startnummer"));
  QString dialogText (StringReplacer::getInstance()->getReplacementString("%beschlusssammlungStartNummer%"));
  dialog.setDialogText(dialogText);

  int startNumber = 0;
  if (QDialog::Accepted == dialog.exec())
  {
    QString value = dialog.getResult();
    startNumber = value.toInt();

    // get decissions
    QSqlDatabase* db = Database::getInstance()->getDatabase();
    if (0 != db)
    {
      QString text ("");
      QTextDocument* doc = new QTextDocument ();
      QTextCursor cursor (doc);

      //insert agenda items
      QSqlQuery query (*Database::getInstance()->getDatabase());
      query.prepare("SELECT * FROM Beschluesse WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvnr AND beschlussArt <= 1");
      query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
      query.bindValue(":year", Database::getInstance()->getCurrentYear());
      query.bindValue(":etvnr", Database::getInstance()->getCurrentEtvNumber());
      query.exec();

      while (query.next())
      {
        QString header                = query.value(5).toString();
        QString description           = query.value(6).toString();
        QString decission             = query.value(7).toString();
        QString voteResult            = query.value(8).toString();
        int voteType                  = query.value(12).toInt();
        QString vermerke              = query.value(13).toString();
        QString gerichtsentscheidung  = query.value(14).toString();
        bool plusBeschreibung         = query.value(15).toBool();

        QTextDocument htmlConverter;

        htmlConverter.setHtml(description);
        QString descriptionPlain = htmlConverter.toPlainText();

        htmlConverter.setHtml(decission);
        QString decissionPlain = htmlConverter.toPlainText();

        htmlConverter.setHtml(voteResult);
        QString voteResultPlain = htmlConverter.toPlainText();

        if ("" != decissionPlain && 3 != voteType)
        {
          QTextTableFormat tableFormat;
          setTableFormat (&tableFormat);

          cursor.movePosition(QTextCursor::End);
          tableFormat.setPageBreakPolicy(QTextFormat::PageBreak_Auto);
          cursor.insertTable (1, 2, tableFormat);
          cursor.insertText(StringReplacer::getInstance()->getReplacementString("%laufendeNummerBeschlusssammlung%"));
          cursor.insertText(": ");
          cursor.insertText(QString::number(startNumber));
          cursor.movePosition(QTextCursor::NextCell);
          QString headerHtml ("<b>");
          headerHtml.append(header);
          headerHtml.append("</b>");
          cursor.insertHtml(headerHtml);
          cursor.movePosition(QTextCursor::End);

          if (plusBeschreibung && descriptionPlain != "")
          {
            tableFormat.setHeaderRowCount(3);
            cursor.insertTable (3, 1, tableFormat);
            cursor.insertHtml(description);
            cursor.movePosition(QTextCursor::NextRow);
          }
          else
          {
            tableFormat.setHeaderRowCount(3);
            cursor.insertTable (2, 1, tableFormat);
          }

          cursor.insertHtml("<b>Beschlussformulierung:</b><br>");
          cursor.insertHtml(decission);
          cursor.movePosition(QTextCursor::NextRow);
          cursor.insertHtml("<b>Vermerke:</b><br>");
          cursor.insertHtml(vermerke);
          cursor.movePosition(QTextCursor::End);

          cursor.insertTable(2, 1, tableFormat);

          QString voteTypeString ("");
          if (0 == voteType)
            voteTypeString = StringReplacer::getInstance()->getReplacementString("%beschlussTypVersammlung%");
          else if (1 == voteType)
            voteTypeString = StringReplacer::getInstance()->getReplacementString("%beschlussTypSchriftlich%");

          cursor.insertHtml("<b>Beschlussart:</b><br>");
          cursor.insertText(voteTypeString);

          cursor.movePosition(QTextCursor::NextRow);

          cursor.insertHtml ("<b>Gerichtsentscheidung:</b>\n");
          cursor.insertHtml(gerichtsentscheidung);

          cursor.movePosition(QTextCursor::End);

          cursor.insertTable(1, 2, tableFormat);

          //get date of etv
          QSqlQuery query2 (*Database::getInstance()->getDatabase());
          query2.prepare("SELECT obj_id, wi_jahr, etv_nr, etv_datum, etv_uhrzeit, etv_ort FROM Eigentuemerversammlungen WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvNum");
          query2.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
          query2.bindValue(":year", Database::getInstance()->getCurrentYear());
          query2.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
          query2.exec();

          QString etvDate ("");

          if (query2.next())
          {
            /*dump.append("\n_______________________");
            dump.append("\nDaten gewählte ETV");
            dump.append("\nWirtschaftsjahr: ");
            dump.append(query.value(1).toString());
            dump.append("\nETV #: ");
            dump.append(query.value(2).toString());
            dump.append("\nETV Datum: ");*/
            etvDate = query2.value(3).toString();
            /*dump.append("\nETV Uhrzeit: ");
            dump.append(query.value(4).toString());
            dump.append("\nETV Ort: ");
            dump.append(query.value(5).toString());*/
          }

          cursor.insertText("Datum: ");
          cursor.insertText(etvDate);

          cursor.movePosition (QTextCursor::NextCell);
          cursor.insertText(StringReplacer::getInstance()->getReplacementString("%Versammlungsleiter%"));

          cursor.movePosition (QTextCursor::End);

          cursor.insertHtml("<br>");
          startNumber++;
        }
      }

      //save file
      QPrinter printer(QPrinter::HighResolution);
      printer.setPageSize(QPrinter::A4);
      printer.setOutputFormat(QPrinter::PdfFormat);

      QFileDialog fileDialog;
      fileDialog.setDirectory(rFilePath);
      QString file = fileDialog.getSaveFileName(this, tr("Speichern als PDF"), "/tmp/Beschlusssammlung.pdf", tr("PDF Datei (*.pdf)"));

      printer.setOutputFileName(file);
      doc->print(&printer);
      delete doc;

      return QFileInfo(file).path();
    }
  }
  return "";
}

QString GeneratorTab::replaceCoverPageTagsByUnderscore (QString in)
{
    //check if the string may at all contain any wildcards
    if (in.contains ("%"))
    {
        QSqlQuery query (*Database::getInstance()->getDatabase());

        //get years
        query.prepare("SELECT obj_mea FROM Objekt WHERE obj_id = :id");
        query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
        query.exec();

        while (query.next())
          in.replace("%MEAgesamt%", query.value(0).toString());

        in.replace("%DatumEinladungsschreiben%", "_________");
        in.replace("%UhrzeitStartVersammlung%", "_________");
        in.replace("%UhrzeitEndeVersammlung%", "_________");
        in.replace("%BeschlussfaehigMarker%", "  ");
        in.replace("%MEAAnwesend%", "_________");
        in.replace("%Versammlungsleiter%", "__________________");
        in.replace("%VersammlungsleiterFktVerwalterMarker%", "  ");
        in.replace("%VersammlungsleiterFktMitarbeiterMarker%", "  ");
        in.replace("%VersammlungsleiterFktWEGMarker%", "  ");
        in.replace("%Protokollfuehrung%", "__________________");
        in.replace("%ProtokollfuehrerFktChef%", "  ");
        in.replace("%ProtokollfuehrerFktMitarbeiter%", "  ");
        in.replace("%ProtokollfuehrerFktWEG%", "  ");
        in.replace("%Abstimmungsregelung%", "__________________");
        in.replace("%Einladungsfrist%", "_________");
    }
    return in;
}

void GeneratorTab::on_doPdf_clicked()
{
  QString storedFilePath (QDir::home().path());

  //tagesordnung generieren
  if (Qt::Checked == ui->agenda->checkState())
  {
    storedFilePath = generateAgendaAsPdf (storedFilePath);
  }

  if (Qt::Checked == ui->protokollvorlage->checkState())
  {
    storedFilePath = generateReportTemplateAsPdf (storedFilePath);
  }

  if (Qt::Checked == ui->procuration->checkState())
  {
    storedFilePath = generateProcurationAsPdf(storedFilePath);
  }

  if (Qt::Checked == ui->protokollabschrift->checkState())
  {
    storedFilePath = generateReportTranscriptAsPdf(storedFilePath);
  }

  if (Qt::Checked == ui->beschlusssammlung->checkState())
  {
    storedFilePath = generateDecissionLibraryAsPdf (storedFilePath);
  }
}
