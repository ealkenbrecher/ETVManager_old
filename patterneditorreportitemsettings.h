#ifndef PATTERNEDITORREPORTITEMSETTINGS_H
#define PATTERNEDITORREPORTITEMSETTINGS_H

#include <QDialog>

namespace Ui {
  class PatternEditorReportItemSettings;
}

class PatternEditorReportItemSettings : public QDialog
{
  Q_OBJECT

public:
  explicit PatternEditorReportItemSettings(QWidget *parent = 0);
  ~PatternEditorReportItemSettings();
  void updatePatternTable();
  void setBodyText (QString aText);
  void setPatternName (QString aText);
  QString getBodyText ();
  QString getPatternName ();

private slots:
  void on_insertWildcard_clicked();

private:
  void refresh();
  Ui::PatternEditorReportItemSettings *ui;
};

#endif // PATTERNEDITORREPORTITEMSETTINGS_H
