#ifndef DATABASEDECISSIONTABLEIMPL_H
#define DATABASEDECISSIONTABLEIMPL_H

#include <QSqlQuery>

class Database;

class DatabaseDecissionTableImpl
{
  public:
    DatabaseDecissionTableImpl();
    QSqlQuery getSelectQuery (QString aArg);
  private:
    Database* mDatabaseclass;

};

#endif // DATABASEDECISSIONTABLEIMPL_H
