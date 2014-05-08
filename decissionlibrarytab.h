#ifndef DECISSIONLIBRARYTAB_H
#define DECISSIONLIBRARYTAB_H

#include <QWidget>

class QSqlDatabase;
class QSqlQueryModelRichtext;

namespace Ui {
  class DecissionLibraryTab;
}

class DecissionLibraryTab : public QWidget
{
  Q_OBJECT

public:
  explicit DecissionLibraryTab(QWidget *parent = 0);
  void refreshOnSelected ();
  ~DecissionLibraryTab();

private slots:
  void on_tableAgenda_doubleClicked(const QModelIndex &index);
  void on_editEntry_clicked();

private:
  void updateAgendaTable ();
  void changeAgendaItemSettings (int aId);
  Ui::DecissionLibraryTab *ui;
  QSqlQueryModelRichtext* mQueryModel;
  QSqlQueryModelRichtext* mQueryModelReportTemplate;
};

#endif // DECISSIONLIBRARYTAB_H
