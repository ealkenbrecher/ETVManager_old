#include "qsqlquerymodelrichtext.h"
#include <QTextDocument>

QSqlQueryModelRichtext::QSqlQueryModelRichtext()
{
}

QVariant QSqlQueryModelRichtext::data(const QModelIndex &index, int role) const
{
  //remove html syntax from database entry
  //return as plain string
  // - just for tableview purpose

  QVariant value = QSqlQueryModel::data(index, role);

  if (value.isValid() && role == Qt::DisplayRole)
  {
      QTextDocument doc;
      doc.setHtml(value.toString());

      return doc.toPlainText();
  }
  return value;
}
