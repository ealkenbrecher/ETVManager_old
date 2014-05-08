#ifndef PROPERTYTAB_H
#define PROPERTYTAB_H

#include <QWidget>

class QSqlDatabase;

namespace Ui {
class PropertyTab;
}

class PropertyTab : public QWidget
{
    Q_OBJECT

public:
    PropertyTab(QWidget *parent = 0);
    virtual ~PropertyTab();
    void refreshRealEstateSelector (bool aKeepSelectedIndex);
    void refreshView ();
    void refreshOnSelected ();

signals:
    void validEtvSelected (bool);

private slots:
    void on_editPropertySettings_clicked();
    void on_newAgenda_clicked();
    void on_liegenschaftSelector_currentTextChanged(const QString &arg1);
    void on_etvs_currentTextChanged(const QString &arg1);
    void on_changeAgendaSettings_clicked();
    void on_deleteAgenda_clicked();
    void on_liegenschaftSelector_currentIndexChanged(int index);
    void on_years_currentTextChanged(const QString &arg1);
    void on_etvs_activated(int index);
    void on_editAdvisers_clicked();
    void on_etvs_currentIndexChanged(const QString &arg1);

private:
    void disableUi (bool aValue);
    void updateInfoBox();
    void updatePropertyNameBox ();
    void updateYearBox ();
    void updateEtvBox();
    void setCurrentPropertyId ();
    void copyReportPattern ();
    void checkAdvisers (int aPropertyId, int aYear, int aEtvNum);

    Ui::PropertyTab *ui;
};

#endif // PROPERTYTAB_H
