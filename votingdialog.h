#ifndef VOTINGDIALOG_H
#define VOTINGDIALOG_H

#include <QDialog>

class QSpinBox;
class QDoubleSpinBox;

namespace Ui {
  class VotingDialog;
}

enum ResultId
{
  eUndef = 0,
  eMehrheitlichAngenommen = 1,
  eAllstimmigAngenommen = 2,
  eEinstimmigAngenommen = 3,
  eMehrheitlichAbgelehnt = 4,
  eKeinBeschluss = 5,
  eManualEdit = 6
};

class VotingDialog : public QDialog
{
  Q_OBJECT

public:
  explicit VotingDialog(QWidget *parent = 0, int aType = 0);

  ResultId getResult ();
  QString getResultManualEdit ();

  int getVotingsYes ();
  float getVotingsYesMEA ();
  int getVotingsNo ();
  float getVotingsNoMEA ();
  int getVotingsConcordant ();
  float getVotingsConcordantMEA ();

  void setManualResult (QString aValue);
  void setResult (ResultId aId);
  void setVotingsYes (int aValue);
  void setVotingsYesMEA (float aValue);
  void setVotingsNo (int aValue);
  void setVotingsNoMEA (float aValue);
  void setVotingsConcordant (int aValue);
  void setVotingsConcordantMEA (float aValue);

  ~VotingDialog();

private slots:
  void on_voteManual_toggled(bool checked);

private:
  void setupView (int aType);
  Ui::VotingDialog *ui;
  int mType;

  QDoubleSpinBox* mSpinBoxDoubleYes;
  QDoubleSpinBox* mSpinBoxDoubleNo;
  QDoubleSpinBox* mSpinBoxDoubleConcordant;

  QSpinBox* mSpinBoxYes;
  QSpinBox* mSpinBoxNo;
  QSpinBox* mSpinBoxConcordant;
};

#endif // VOTINGDIALOG_H
