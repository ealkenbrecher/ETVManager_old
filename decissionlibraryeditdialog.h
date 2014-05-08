#ifndef DECISSIONLIBRARYEDITDIALOG_H
#define DECISSIONLIBRARYEDITDIALOG_H

#include <QDialog>

namespace Ui {
  class DecissionLibraryEditDialog;
}

class DecissionLibraryEditDialog : public QDialog
{
  Q_OBJECT

public:
  explicit DecissionLibraryEditDialog(QWidget *parent = 0);
  ~DecissionLibraryEditDialog();

  void setVoteType (int aValue);
  void setAnnotation (const QString &rValue);
  void setCourtOrder (const QString &rValue);
  void setAddDescriptionFlag (bool aValue);

  int getVoteType (void) const;
  QString getAnnotation (void) const;
  QString getCourtOrder (void) const;
  bool getAddDescriptionFlag ();

private:
  Ui::DecissionLibraryEditDialog *ui;
};

#endif // DECISSIONLIBRARYEDITDIALOG_H
