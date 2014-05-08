#ifndef REPORTGENERATORTAB_H
#define REPORTGENERATORTAB_H

#include <QWidget>
#include <QPair>
#include <QList>
#include <QString>
#include <QStringList>
#include "wizarddialogbox.h"

class QSqlQueryModelRichtext;

namespace Ui {
  class ReportGeneratorTab;
}

class ReportGeneratorTab : public QWidget
{
  Q_OBJECT

public:
  explicit ReportGeneratorTab(QWidget *parent = 0);
  void refreshOnSelected ();
  ~ReportGeneratorTab();

private slots:
  void on_startWizard_clicked();
  void on_startAgendaWizard_clicked();
  void on_tableAgenda_doubleClicked(const QModelIndex &index);
  void on_tableReportTemplate_doubleClicked(const QModelIndex &index);
  void on_addEntry_clicked();
  void on_editEntry_clicked();
  void on_moveAgendaItemUp_clicked();
  void on_moveAgendaItemDown_clicked();
  void on_deleteEntry_clicked();
  void on_addDecission_clicked();

private:
  void startCoverpageWizard ();
  void startAgendaWizard ();
  void updateAgendaTable ();
  void updateReportTemplateTable ();
  void changeAgendaItemSettings (int aId);
  int processWizardDialog (QString aDialogText, QString aWildcard, eDlgType aType = eSimpleDialog, bool aMultipleChoice = false, QStringList aList = QStringList(""));

  Ui::ReportGeneratorTab *ui;
  bool mMarkerVersammlungsleiterFktSet;
  bool mMarkerProtokollfuehrerFktSet;
  QSqlQueryModelRichtext* mQueryModel;
  QSqlQueryModelRichtext* mQueryModelReportTemplate;
};

#endif // REPORTGENERATORTAB_H
