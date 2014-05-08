#ifndef ORDERTAB_H
#define ORDERTAB_H

#include <QWidget>

class QSqlDatabase;
class QSqlQueryModelRichtext;

namespace Ui {
class OrderTab;
}

class OrderTab : public QWidget
{
    Q_OBJECT

public:
    OrderTab(QWidget *parent = 0);
    void refreshOnSelected ();
    void refresh ();
    virtual ~OrderTab();

signals:
    void selectTab (int);

private slots:
    void on_tableAgenda_doubleClicked(const QModelIndex &index);

    void on_tableReportTemplate_doubleClicked(const QModelIndex &index);

private:
    //void initReportPattern ();
    void updateAgendaTable ();
    void updateReportTemplateTable ();
    void changeAgendaItemSettings (int aId);
    Ui::OrderTab *ui;
    QSqlQueryModelRichtext* mQueryModel;
    QSqlQueryModelRichtext* mQueryModelReportTemplate;
};

#endif // ORDERTAB_H
