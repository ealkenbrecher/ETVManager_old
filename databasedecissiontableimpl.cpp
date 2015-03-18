#include "databasedecissiontableimpl.h"
#include "database.h"

#include <QSqlQuery>

DatabaseDecissionTableImpl::DatabaseDecissionTableImpl()
{
  mDatabaseclass = Database::getInstance();
}

QSqlQuery DatabaseDecissionTableImpl::getSelectQuery (QString aArg)
{
  if (0 != mDatabaseclass)
  {
    if (0 != mDatabaseclass->getDatabase())
    {
      QString request ("SELECT ");
      request.append(aArg);
      request.append(" FROM Beschluesse WHERE obj_id = :id AND wi_jahr = :year AND etv_nr = :etvnr");

      Database::getInstance()->getDatabase()->open();
      QSqlQuery query (*mDatabaseclass->getDatabase());

      query.prepare(request);
      query.bindValue(":id", mDatabaseclass->getCurrentPropertyId());
      query.bindValue(":year", mDatabaseclass->getCurrentYear());
      query.bindValue(":etvnr", mDatabaseclass->getCurrentEtvNumber());
      query.exec();
      Database::getInstance()->getDatabase()->close();
    }
  }
  QSqlQuery noResult;
  return noResult;
}
