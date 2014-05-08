#ifndef AGENDATAB_H
#define AGENDATAB_H

#include <QWidget>

class QSqlDatabase;
class QSqlQueryModel;

namespace Ui {
class AgendaTab;
}

class AgendaTab : public QWidget
{
    Q_OBJECT

public:
    AgendaTab(QWidget *parent = 0);
    void refreshOnSelected ();
    void refresh ();
    virtual ~AgendaTab();

signals:
    void selectTab (int);

private slots:
    void on_tableAgenda_doubleClicked(const QModelIndex &index);
    void on_addEntry_clicked();
    void on_editEntry_clicked();
    void on_deleteEntry_clicked();
    void on_moveAgendaItemUp_clicked();
    void on_moveAgendaItemDown_clicked();

private:
    void updateAgendaTable ();
    void changeAgendaItemSettings (int aTopId);
    Ui::AgendaTab *ui;
    QSqlQueryModel* mQueryModel;
};

#endif // AGENDATAB_H
