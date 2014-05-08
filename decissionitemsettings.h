#ifndef DECISSIONITEMSETTINGS_H
#define DECISSIONITEMSETTINGS_H

#include <QDialog>

namespace Ui {
  class DecissionItemSettings;
}

class DecissionItemSettings : public QDialog
{
  Q_OBJECT

public:
  explicit DecissionItemSettings(QWidget *parent = 0);
  ~DecissionItemSettings();
  void setHeader (const QString aValue);
  void setDescription (const QString aValue);
  void setDecissionText (const QString aValue);
  void setDecissionProclamation (const QString aValue);
  void setYesVotes (float aValue);
  void setNoVotes (float aValue);
  void setAbsentionVotes (float aValue);
  void setType (int aType);

  const QString getHeader (void);
  const QString getDescription (void);
  const QString getDecissionText (void);
  const QString getDecissionProclamation (void);
  float getYesVotes (void);
  float getNoVotes (void);
  float getAbsentionVotes (void);
  int getType ();

private:
  Ui::DecissionItemSettings *ui;
};

#endif // DECISSIONITEMSETTINGS_H
