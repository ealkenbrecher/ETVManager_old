#ifndef ORDERITEMSETTINGS_H
#define ORDERITEMSETTINGS_H

#include <QDialog>

namespace Ui {
  class OrderItemSettings;
}

class OrderItemSettings : public QDialog
{
  Q_OBJECT

public:
  explicit OrderItemSettings(QWidget *parent = 0);
  ~OrderItemSettings();
  void setValue (int aValue);
  int getValue ();

private:
  Ui::OrderItemSettings *ui;
};

#endif // ORDERITEMSETTINGS_H
