#include "stringreplacer.h"
#include <QtCore>
#include "database.h"
#include <QSqlQuery>


StringReplacer::StringReplacer()
{
  mDb = 0;

  if (Database::getInstance()->dbIsOk())
    mDb = Database::getInstance()->getDatabase();

  clear ();
}

StringReplacer::~StringReplacer()
{
  clear ();
}

StringReplacer* StringReplacer::getInstance()
{
  static StringReplacer theInstance;
  return &theInstance;
}

void StringReplacer::addPair (const QString &rWildcard, const QString &rReplacementString)
{
  if (mDb)
  {
    if (!mDb->isOpen())
    {
      qDebug () << "StringReplacer::addPair: Db not open. This is a helper method. Open db in caller before using.";
      return;
    }

    QSqlQuery query (*mDb);

    //get values
    query.prepare("SELECT * FROM Wildcards WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvNum AND wildcard = :wildcard");
    query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
    query.bindValue(":year", Database::getInstance()->getCurrentYear());
    query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
    query.bindValue(":wildcard", rWildcard);
    query.exec();

    //record found
    if (query.next ())
    {
      //set wildcard
      query.prepare("UPDATE Wildcards SET replacement = :replacement WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvNum AND wildcard = :wildcard");
      query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
      query.bindValue(":year", Database::getInstance()->getCurrentYear());
      query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
      query.bindValue(":wildcard", rWildcard);
      query.bindValue(":replacement", rReplacementString);

      if (false == query.exec())
        qDebug () << "StringReplacer::addPair -> update Query failed.";
    }
    //no record found
    else
    {
      //set wildcard
      query.clear();
      query.prepare("INSERT INTO Wildcards (obj_id, wi_jahr, etv_nr, wildcard, replacement) VALUES (:id, :year, :etvNum, :newWildcard, :newReplacement)");
      query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
      query.bindValue(":year", Database::getInstance()->getCurrentYear());
      query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
      query.bindValue(":newWildcard", rWildcard);
      query.bindValue(":newReplacement", rReplacementString);

      if (false == query.exec())
        qDebug () << "StringReplacer::addPair -> insert Query failed.";
    }
  }
}

void StringReplacer::removeEntry (const QString &rWildcard)
{
  //todo
}

const QString StringReplacer::getReplacementString (const QString &rWildcard)
{
  QString retVal ("");

  if (getFromDb(&retVal, rWildcard))
    return retVal;
  else if (getFromCommon(&retVal, rWildcard))
    return retVal;

  return retVal;
}

bool StringReplacer::getFromDb (QString *pRetVal, const QString &rWildcard)
{
  if (0 != pRetVal)
  {
    if (mDb)
    {
      if (!mDb->isOpen())
      {
        qDebug () << "StringReplacer::addPair: Db not open. This is a helper method. Open db in caller before using.";
        return false;
      }

      QSqlQuery query (*mDb);

      //get values
      query.prepare("SELECT replacement FROM Wildcards WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvNum AND wildcard = :wildcard");
      query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
      query.bindValue(":year", Database::getInstance()->getCurrentYear());
      query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
      query.bindValue(":wildcard", rWildcard);
      query.exec();

      if (query.next())
      {
        *pRetVal = query.value(0).toString();
        return true;
      }
    }
  }
  return false;
}

bool StringReplacer::getFromCommon (QString *pRetVal, const QString &rWildcard)
{
  if (0 != pRetVal)
  {
    if (mDb)
    {
      if (!mDb->isOpen())
      {
        qDebug () << "StringReplacer::addPair: Db not open. This is a helper method. Open db in caller before using.";
        return false;
      }

      QSqlQuery query (*mDb);

      //get values
      query.prepare("SELECT Text FROM CommonTextPatterns WHERE wildcard = :wildcard");
      query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
      query.bindValue(":year", Database::getInstance()->getCurrentYear());
      query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
      query.bindValue(":wildcard", rWildcard);
      query.exec();

      if (query.next())
      {
        *pRetVal = query.value(0).toString();
        return true;
      }
    }
  }
  return false;
}

void StringReplacer::findAndReplaceWildcards(QString *in)
{
  if (in)
  {
    if (mDb)
    {
      if (!mDb->isOpen())
      {
        qDebug () << "StringReplacer::addPair: Db not open. This is a helper method. Open db in caller before using.";
        return;
      }

      QSqlQuery query (*mDb);

      //get values
      query.prepare("SELECT wildcard, replacement FROM Wildcards WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvNum");
      query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
      query.bindValue(":year", Database::getInstance()->getCurrentYear());
      query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
      query.exec();

      QStringPair stringPair;
      QStringPairList stringPairList;

      while (query.next())
      {
        stringPair.first = query.value(0).toString();
        stringPair.second = query.value(1).toString();

        stringPairList.append(stringPair);
      }

      for (int i = 0; i < stringPairList.count(); ++i)
      {
        in->replace (stringPairList[i].first, stringPairList[i].second);
      }
    }
  }
}

const QString StringReplacer::findAndReplaceWildcards (QString in)
{
  QString retVal = in;

  if (mDb)
  {
    if (!mDb->isOpen())
    {
      qDebug () << "StringReplacer::addPair: Db not open. This is a helper method. Open db in caller before using.";
      return in;
    }

    QSqlQuery query (*mDb);

    //get values
    query.prepare("SELECT wildcard, replacement FROM Wildcards WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvNum");
    query.bindValue(":id", Database::getInstance()->getCurrentPropertyId());
    query.bindValue(":year", Database::getInstance()->getCurrentYear());
    query.bindValue(":etvNum", Database::getInstance()->getCurrentEtvNumber());
    query.exec();

    QStringPair stringPair;
    QStringPairList stringPairList;

    while (query.next())
    {
      stringPair.first = query.value(0).toString();
      stringPair.second = query.value(1).toString();

      stringPairList.append(stringPair);
    }

    for (int i = 0; i < stringPairList.count(); ++i)
    {
      retVal.replace (stringPairList[i].first, stringPairList[i].second);
    }
  }
  return retVal;
}

void StringReplacer::clear ()
{

}
