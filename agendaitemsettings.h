#ifndef AGENDAITEMSETTINGS_H
#define AGENDAITEMSETTINGS_H

#include <QDialog>

namespace Ui {
class AgendaItemSettings;
}

class QSqlQueryModelRichtext;

class AgendaItemSettings : public QDialog
{
    Q_OBJECT

public:
    explicit AgendaItemSettings(QWidget *parent = 0);
    ~AgendaItemSettings();
    QString getHeader ();
    QString getDescription ();
    QString getSuggestion ();
    QString getSuggestion2 ();
    QString getSuggestion3 ();
    int getItemType ();

    void refresh ();
    void setHeader (QString rValue);
    void setDescription (QString rValue);
    void setSuggestion (QString rValue);
    void setSuggestion2 (QString rValue);
    void setSuggestion3 (QString rValue);
    void setItemType (int aType);

private slots:
    void on_insertPattern_clicked();

private:

    QString replaceWildcards (QString in);
    Ui::AgendaItemSettings *ui;
    QSqlQueryModelRichtext* m_model;
};

#endif // AGENDAITEMSETTINGS_H
