#ifndef EDITADVISERSDIALOG_H
#define EDITADVISERSDIALOG_H

#include <QDialog>

namespace Ui {
  class EditAdvisersDialog;
}

class EditAdvisersDialog : public QDialog
{
  Q_OBJECT

public:
  explicit EditAdvisersDialog(QWidget *parent = 0);
  ~EditAdvisersDialog();
  void setAdviserPresident (const QString &rValue);
  void setAdviser1 (const QString &rValue);
  void setAdviser2 (const QString &rValue);
  QString getAdviserPresident ();
  QString getAdviser1 ();
  QString getAdviser2 ();

private:
  Ui::EditAdvisersDialog *ui;
};

#endif // EDITADVISERSDIALOG_H
