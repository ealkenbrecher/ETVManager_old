#ifndef STRINGREPLACER_H
#define STRINGREPLACER_H

#include <QString>
#include <QPair>
#include <QList>

class QSqlDatabase;

class StringReplacer
{
  public:
    static StringReplacer *getInstance();
    void addPair (const QString &rWildcard, const QString &rReplacementString);
    const QString getReplacementString (const QString &rWildcard);
    void removeEntry (const QString &rWildcard);
    const QString findAndReplaceWildcards (QString in);
    void findAndReplaceWildcards(QString *in);
    void clear ();
  private:
    StringReplacer(void);
    ~StringReplacer(void);
    bool getFromDb (QString *pString, const QString &rWildcard);
    bool getFromCommon (QString *pString, const QString &rWildcard);

    QSqlDatabase* mDb;

    typedef QPair<QString, QString> QStringPair;
    typedef QList<QStringPair> QStringPairList;
};

#endif // STRINGREPLACER_H
