#ifndef PATTERNEDITOR_H
#define PATTERNEDITOR_H

#include <QDialog>

class QSqlQueryModelRichtext;

namespace Ui {
  class PatternEditor;
}

class PatternEditor : public QDialog
{
  Q_OBJECT

public:
  explicit PatternEditor(QWidget *parent = 0);
  ~PatternEditor();
  void updatePatternTable ();

private slots:
  void on_editEntry_clicked();
  void on_tablePatterns_doubleClicked(const QModelIndex &index);
  void on_addEntry_clicked();
  void on_deleteEntry_clicked();

private:
  void changePatternItemSettings (int aId);
  Ui::PatternEditor *ui;
  QSqlQueryModelRichtext* mQueryModel;
};

#endif // PATTERNEDITOR_H
