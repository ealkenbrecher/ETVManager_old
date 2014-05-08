#ifndef SYSTEMSETTINGS_H
#define SYSTEMSETTINGS_H

#include <QDialog>

namespace Ui {
  class SystemSettings;
}

class SystemSettings : public QDialog
{
  Q_OBJECT

public:
  explicit SystemSettings(QWidget *parent = 0);
  ~SystemSettings();

private:
  Ui::SystemSettings *ui;
};

#endif // SYSTEMSETTINGS_H
