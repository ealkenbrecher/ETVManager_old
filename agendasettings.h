#ifndef AGENDASETTINGS_H
#define AGENDASETTINGS_H

#include <QDialog>

namespace Ui {
class AgendaSettings;
}

class QAbstractButton;

class AgendaSettings : public QDialog
{
    Q_OBJECT

public:
    explicit AgendaSettings(QWidget *parent = 0);
    int getYear () const;
    int getEtvNum () const;
    QDate getEtvDate () const;
    QTime getEtvTime () const;
    QString getEtvLocation () const;
    void setYear (int aYear);
    void setEtvNum (int aNum);
    void setEtvDate (QDate aDate);
    void setEtvTime (QTime aTime);
    void setEtvLocation (QString aLocation);
    void disableYearSetting (bool aDisable);
    void disableEtvNumSetting (bool aDisable);
    ~AgendaSettings();

private slots:
    void on_year_valueChanged(int arg1);

private:
    Ui::AgendaSettings *ui;
};

#endif // AGENDASETTINGS_H
