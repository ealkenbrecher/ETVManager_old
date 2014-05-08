#ifndef PATTERNEDITORREPORT_H
#define PATTERNEDITORREPORT_H

#include <QDialog>
#include "qsqlquerymodelrichtext.h"

namespace Ui {
  class patternEditorReport;
}

class patternEditorReport : public QDialog
{
  Q_OBJECT

public:
  explicit patternEditorReport(QWidget *parent = 0);
  void updatePatternTable();
  ~patternEditorReport();

private slots:
  void on_editEntry_clicked();

  void on_tablePatterns_doubleClicked(const QModelIndex &index);

private:
  void changePatternItemSettings (int aId);
  Ui::patternEditorReport *ui;
  QSqlQueryModelRichtext* mQueryModel;
};

#endif // PATTERNEDITORREPORT_H
