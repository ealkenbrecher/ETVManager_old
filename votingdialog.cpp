#include "votingdialog.h"
#include "ui_votingdialog.h"
#include <QSpinBox>
#include <QDoubleSpinBox>
#include "database.h"
#include "global.h"

VotingDialog::VotingDialog(QWidget *parent, int aType) :
  QDialog(parent),
  ui(new Ui::VotingDialog)
{
  ui->setupUi(this);
  mType = aType;

  mSpinBoxDoubleYes = 0;
  mSpinBoxDoubleNo = 0;
  mSpinBoxDoubleConcordant = 0;

  mSpinBoxYes = 0;
  mSpinBoxNo = 0;
  mSpinBoxConcordant = 0;

  setupView (mType);
}

VotingDialog::~VotingDialog()
{
  qDebug () << "destructor called";
  delete ui;
}

ResultId VotingDialog::getResult ()
{
  ResultId value;

  if (ui->voteManual->isChecked ())
    value = eManualEdit;
  else if (ui->voteMajorityAccepted->isChecked ())
    value = eMehrheitlichAngenommen;
  else if (ui->voteMajorityAll->isChecked ())
    value = eAllstimmigAngenommen;
  else if (ui->voteMajorityConcordant->isChecked ())
    value = eEinstimmigAngenommen;
  else if (ui->voteMajorityRejected->isChecked ())
    value = eMehrheitlichAbgelehnt;
  else if (ui->voteNoVote->isChecked ())
    value = eKeinBeschluss;
  else
    value = eUndef;

  return value;
}

QString VotingDialog::getResultManualEdit ()
{
  if (ui->voteManual->isChecked ())
    return ui->voteManualTextEdit->toHtml();
  else
    return "";
}

void VotingDialog::on_voteManual_toggled(bool checked)
{
  ui->voteManualTextEdit->setEnabled(checked);
}

int VotingDialog::getVotingsYes ()
{
  if (0 != mSpinBoxYes)
    return mSpinBoxYes->value ();
  else
  {
    qDebug () << "mSpinBoxYes NULL";
    return -1;
  }
}

float VotingDialog::getVotingsYesMEA ()
{
  if (0 != mSpinBoxDoubleYes)
    return mSpinBoxDoubleYes->value ();
  else
  {
    qDebug () << "mSpinBoxYesMEA NULL";
    return -1;
  }
}

int VotingDialog::getVotingsNo ()
{
  if (0 != mSpinBoxNo)
    return mSpinBoxNo->value ();
  else
  {
    qDebug () << "mSpinBoxNo NULL";
    return -1;
  }
}

float VotingDialog::getVotingsNoMEA ()
{
  if (0 != mSpinBoxDoubleNo)
    return mSpinBoxDoubleNo->value ();
  else
  {
    qDebug () << "mSpinBoxNoMEA NULL";
    return -1;
  }
}

int VotingDialog::getVotingsConcordant ()
{
  if (0 != mSpinBoxConcordant)
    return mSpinBoxConcordant->value ();
  else
    return -1;
}

float VotingDialog::getVotingsConcordantMEA ()
{
  if (0 != mSpinBoxDoubleConcordant)
    return mSpinBoxDoubleConcordant->value ();
  else
    return -1;
}

void VotingDialog::setupView(int aType)
{
  if (eVoice == aType)
  {
    mSpinBoxDoubleYes = new QDoubleSpinBox (this);
    ui->spinBoxLayout->addWidget (mSpinBoxDoubleYes);

    mSpinBoxDoubleNo = new QDoubleSpinBox (this);
    ui->spinBoxLayout->addWidget (mSpinBoxDoubleNo);

    mSpinBoxDoubleConcordant = new QDoubleSpinBox (this);
    ui->spinBoxLayout->addWidget (mSpinBoxDoubleConcordant);

    QLabel* label1 = new QLabel (this);
    label1->setText ("MEA");
    QLabel* label2 = new QLabel (this);
    label2->setText ("MEA");
    QLabel* label3 = new QLabel (this);
    label3->setText ("MEA");

    ui->spinBoxLabelLayout->addWidget (label1,0);
    ui->spinBoxLabelLayout->addWidget (label2,1);
    ui->spinBoxLabelLayout->addWidget (label3,2);
  }

  if (eHead == aType)
  {
    mSpinBoxYes = new QSpinBox (this);
    ui->spinBoxLayout->addWidget (mSpinBoxYes);

    mSpinBoxNo = new QSpinBox (this);
    ui->spinBoxLayout->addWidget (mSpinBoxNo);

    mSpinBoxConcordant = new QSpinBox (this);
    ui->spinBoxLayout->addWidget (mSpinBoxConcordant);

    QLabel* label1 = new QLabel (this);
    label1->setText ("Stimmen");
    QLabel* label2 = new QLabel (this);
    label2->setText ("Stimmen");
    QLabel* label3 = new QLabel (this);
    label3->setText ("Stimmen");

    ui->spinBoxLabelLayout->addWidget (label1,0);
    ui->spinBoxLabelLayout->addWidget (label2,1);
    ui->spinBoxLabelLayout->addWidget (label3,2);
  }
}

void VotingDialog::setManualResult (QString aValue)
{
  ui->voteManualTextEdit->setText (aValue);
}

void VotingDialog::setResult (ResultId aId)
{
  switch (aId)
    {
      case eManualEdit:
        ui->voteManual->setChecked(true);
        break;
      case eMehrheitlichAngenommen:
        ui->voteMajorityAccepted->setChecked(true);
        break;
      case eAllstimmigAngenommen:
        ui->voteMajorityAll->setChecked(true);
        break;
      case eEinstimmigAngenommen:
        ui->voteMajorityConcordant->setChecked(true);
        break;
      case eMehrheitlichAbgelehnt:
        ui->voteMajorityRejected->setChecked(true);
        break;
      case eKeinBeschluss:
        ui->voteNoVote->setChecked(true);
        break;
      default:
        break;
    }
}

void VotingDialog::setVotingsYes (int aValue)
{
  if (mSpinBoxYes)
    mSpinBoxYes->setValue(aValue);
}

void VotingDialog::setVotingsYesMEA (float aValue)
{
  if (mSpinBoxDoubleYes)
    mSpinBoxDoubleYes->setValue(aValue);
}

void VotingDialog::setVotingsNo (int aValue)
{
  if (mSpinBoxNo)
    mSpinBoxNo->setValue(aValue);
}

void VotingDialog::setVotingsNoMEA (float aValue)
{
  if (mSpinBoxDoubleNo)
    mSpinBoxDoubleNo->setValue(aValue);
}

void VotingDialog::setVotingsConcordant (int aValue)
{
  if (mSpinBoxConcordant)
    mSpinBoxConcordant->setValue(aValue);
}

void VotingDialog::setVotingsConcordantMEA (float aValue)
{
  if (mSpinBoxDoubleConcordant)
    mSpinBoxDoubleConcordant->setValue(aValue);
}
