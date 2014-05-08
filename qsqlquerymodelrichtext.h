#ifndef QSQLQUERYMODELRICHTEXT_H
#define QSQLQUERYMODELRICHTEXT_H

#include <QSqlQueryModel>

class QSqlQueryModelRichtext : public QSqlQueryModel
{
public:
  QSqlQueryModelRichtext();
  QVariant data(const QModelIndex &index, int role) const;
};

#endif // QSQLQUERYMODELRICHTEXT_H
