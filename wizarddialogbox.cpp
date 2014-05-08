#include "wizarddialogbox.h"
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QtCore>
#include <QMessageBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QDoubleSpinBox>
#include "database.h"
#include "stringreplacer.h"

WizardDialogBox::WizardDialogBox(QWidget *parent) :
  QDialog(parent)
{
  initDialog(eSimpleDialog);
}

WizardDialogBox::WizardDialogBox(QWidget *parent, const char *pDialogText, eDlgType aDlgType)
  : QDialog(parent)
{
  initDialog(aDlgType);

  if (aDlgType == eComplexDialog)
    addVotingOptions ();

  if (0 != pDialogText)
  {
      QString text (pDialogText);
      setDialogText(text);
  }
}

WizardDialogBox::~WizardDialogBox()
{
  //if (0 != mBaselayout)
  //  delete mBaselayout;
  if (0 != mComplexDlgSuggestionsGrid)
    delete mComplexDlgSuggestionsGrid;
  if (0 != mHLayout)
    delete mHLayout;
  if (0 != mButtonLayout)
    delete mButtonLayout;
}

float WizardDialogBox::getVotingsYes ()
{
  if (mJaStimmen)
    return mJaStimmen->value();
  return -99;
}

float WizardDialogBox::getVotingsNo ()
{
  if (mNeinStimmen)
    return mNeinStimmen->value();
  return -99;
}

float WizardDialogBox::getVotingsConcordant ()
{
  if (mEnthaltungStimmen)
    return mEnthaltungStimmen->value();
  return -99;
}

void WizardDialogBox::setVotePreview (QString aValue)
{
  if (mBeschlussVorschau)
    mBeschlussVorschau->setText(aValue);
}

void WizardDialogBox::setVotingsYes (float aValue)
{
  if (mJaStimmen)
    mJaStimmen->setValue(aValue);
}

void WizardDialogBox::setVotingsNo (float aValue)
{
  if (mNeinStimmen)
    mNeinStimmen->setValue(aValue);
}

void WizardDialogBox::setVotingsConcordant (float aValue)
{
  if (mEnthaltungStimmen)
    mEnthaltungStimmen->setValue(aValue);
}

QString WizardDialogBox::getVoteText ()
{
  if (mBeschlussVorschau)
    return mBeschlussVorschau->toHtml();
  return "";
}

void WizardDialogBox::initDialog (eDlgType aDlgType)
{
  mOkButton = 0;
  mJaStimmen = 0;
  mNeinStimmen = 0;
  mEnthaltungStimmen = 0;
  mBeschlussVorschau = 0;
  mBaselayout = 0;
  mDescriptionText = 0;
  mSuggestion1 = 0;
  mSuggestion2 = 0;
  mSuggestion3 = 0;
  mCheckBox1 = 0;
  mCheckBox2 = 0;
  mCheckBox3 = 0;
  mMultipleChoice = false;
  mSimpleDlgEditField = 0;
  mComplexDlgEditField = 0;
  mHLayout = 0;
  mButtonLayout = 0;
  mComplexDlgSuggestionsGrid = 0;

  mDescriptionText = new QLabel (this);

  mBaselayout = new QGridLayout ();
  setLayout(mBaselayout);

  mType = aDlgType;

  switch (mType)
  {
    case eSimpleDialog:
      setFixedSize(400, 200);
      mDescriptionText->setText("Bitte treffen Sie eine Eingabe.");
      mSimpleDlgEditField = new QLineEdit (this);
      mBaselayout->addWidget(mSimpleDlgEditField, 1, 0, 3, 2);
      break;
    case eComplexDialog:
      setFixedSize(850, 640);
      mDescriptionText->setText("Bitte wählen Sie einen Beschlusstext aus den Vorschlägen aus \noder treffen Sie eine manuelle Eingabe.");
      mComplexDlgEditField = new QTextEdit (this);
      mComplexDlgSuggestionsGrid = new QGridLayout ();
      mBaselayout->addWidget(mComplexDlgEditField, 1, 0, 3, 2);
      mBaselayout->addLayout(mComplexDlgSuggestionsGrid, 1, 2, 3, 2);
      break;
    case eOrderDialog:
      setFixedSize(400, 200);
      mDescriptionText->setText("Bitte fügen Sie weitere Infos hinzu.");
      mComplexDlgEditField = new QTextEdit (this);
      //mComplexDlgSuggestionsGrid = new QGridLayout (this);
      mBaselayout->addWidget(mComplexDlgEditField, 1, 0, 3, 4);
      //mBaselayout->addLayout(mComplexDlgSuggestionsGrid, 1, 2, 3, 2);
      break;
    case eCheckBoxDialog:
      setFixedSize(400, 200);
      mDescriptionText->setText("Bitte wählen Sie eine der Optionen.");
      break;
    default:
      break;
  }

  mBaselayout->addWidget(mDescriptionText, 0, 0, 1, 4);

  QPushButton* buttonNext = new QPushButton ("Weiter", this);
  connect (buttonNext, SIGNAL (clicked ()), this, SLOT (accept()));
  QPushButton* buttonCancel = new QPushButton ("Abbrechen", this);
  connect (buttonCancel, SIGNAL (clicked ()), this, SLOT (reject()));

  //buttonNext->setSizePolicy(QSizePolicy::Fixed);
  //buttonCancel->setSizePolicy(QSizePolicy::Fixed);

  mHLayout = new QHBoxLayout ();
  mHLayout->addWidget(buttonCancel, 0, Qt::AlignHCenter | Qt::AlignRight);
  mHLayout->addWidget(buttonNext, 0, Qt::AlignHCenter | Qt::AlignRight);

  mBaselayout->addLayout(mHLayout, 13, 3, 1, 1);

  this->setWindowTitle("Beschlussfassung editieren");
}

void WizardDialogBox::setTitle (QString aTitle)
{
  this->setWindowTitle (aTitle);
}

void WizardDialogBox::setMultipleChoice (bool aAllowed)
{
  mMultipleChoice = aAllowed;
}

void WizardDialogBox::setCheckBox (eCheckBoxIndex aIndex, QString aLabel)
{
  if (mType != eCheckBoxDialog)
    return;

  switch (aIndex)
  {
    case eCheckBox1:
      //already initialised -> change label
      if (0 != mCheckBox1)
        mCheckBox1->setText(aLabel);
      else
      {
        mCheckBox1 = new QCheckBox (aLabel, this);
        mBaselayout->addWidget(mCheckBox1, 1, 0, 3, 2);
        connect (mCheckBox1, SIGNAL (stateChanged (int)), this, SLOT (stateCheckBox1Changed (int)));
      }
      break;
    case eCheckBox2:
      //already initialised -> change label
      if (0 != mCheckBox2)
        mCheckBox2->setText(aLabel);
      else
      {
        mCheckBox2 = new QCheckBox (aLabel, this);
        mBaselayout->addWidget(mCheckBox2, 2, 0, 3, 2);
        connect (mCheckBox2, SIGNAL (stateChanged (int)), this, SLOT (stateCheckBox2Changed (int)));
      }
      break;
    case eCheckBox3:
      //already initialised -> change label
      if (0 != mCheckBox3)
        mCheckBox3->setText(aLabel);
      else
      {
        mCheckBox3 = new QCheckBox (aLabel, this);
        mBaselayout->addWidget(mCheckBox3, 3, 0, 3, 2);
        connect (mCheckBox3, SIGNAL (stateChanged (int)), this, SLOT (stateCheckBox3Changed (int)));
      }
      break;
    default:
      break;
  }
}

void WizardDialogBox::stateCheckBox1Changed (int aState)
{
  if (aState == Qt::Checked)
  {
    if (0 != mCheckBox2)
      mCheckBox2->setChecked(false);

    if (0 != mCheckBox3)
      mCheckBox3->setChecked(false);
  }
}

void WizardDialogBox::stateCheckBox2Changed (int aState)
{
  if (aState == Qt::Checked)
  {
    if (0 != mCheckBox1)
      mCheckBox1->setChecked(false);

    if (0 != mCheckBox3)
      mCheckBox3->setChecked(false);
  }
}

void WizardDialogBox::stateCheckBox3Changed (int aState)
{
  if (aState == Qt::Checked)
  {
    if (0 != mCheckBox1)
      mCheckBox1->setChecked(false);

    if (0 != mCheckBox2)
      mCheckBox2->setChecked(false);
  }
}

void WizardDialogBox::setDialogText (QString &rValue)
{
  if (0 != mDescriptionText)
  {
    mDescriptionText->setText(rValue);
  }
}

QString WizardDialogBox::getResult ()
{
  if (mType == eCheckBoxDialog)
  {
    qDebug () << "WizardDialogBox::getResult (): mType == eCheckBoxDialog";
    return "";
  }

  if (mType == eSimpleDialog)
  {
    if (0 != mSimpleDlgEditField)
    {
      return mSimpleDlgEditField->text();
    }
  }
  else if (mType == eComplexDialog)
  {
    if (0 != mComplexDlgEditField)
    {
      return mComplexDlgEditField->toHtml();
    }
  }
  else if (mType == eOrderDialog)
  {
    if (0 != mComplexDlgEditField)
    {
      return mComplexDlgEditField->toHtml();
    }
  }

  return "";
}

void WizardDialogBox::setResult (QString aValue)
{
  if (0 != mSimpleDlgEditField)
  {
    mSimpleDlgEditField->setText(aValue);
  }
  else if (0 != mComplexDlgEditField)
  {
    mComplexDlgEditField->setText(aValue);
  }
}

Qt::CheckState WizardDialogBox::getCheckState (eCheckBoxIndex aIndex)
{
  switch (aIndex)
  {
    case eCheckBox1:
      if (0 != mCheckBox1)
        return mCheckBox1->checkState();
      break;
    case eCheckBox2:
      if (0 != mCheckBox2)
        return mCheckBox2->checkState();
      break;
    case eCheckBox3:
      if (0 != mCheckBox3)
        return mCheckBox3->checkState();
      break;
    default:
      break;
  }
  return Qt::Unchecked;
}

void WizardDialogBox::setSuggestion1 (QString aValue)
{
  if (mType != eComplexDialog)
    return;

  if (0 == mSuggestion1)
  {
    mSuggestion1 = new QTextEdit (aValue, this);

    mUseSuggestion1 = new QPushButton ("3", this);
    mUseSuggestion1->setMaximumSize(25,25);
    mUseSuggestion1->setFont(QFont ("WebDings",12,12,false));

    mComplexDlgSuggestionsGrid->addWidget(mUseSuggestion1, 0, 0, 1, 1);
    mComplexDlgSuggestionsGrid->addWidget(mSuggestion1, 0, 1, 1, 1);
    connect (this->mUseSuggestion1, SIGNAL (clicked ()), SLOT (mUseSuggestion1Clicked ()));
  }
  else
    mSuggestion1->setText(aValue);
}

void WizardDialogBox::setSuggestion2 (QString aValue)
{
  if (mType != eComplexDialog)
    return;

  if (0 == mSuggestion2)
  {
    mSuggestion2 = new QTextEdit (aValue, this);

    mUseSuggestion2 = new QPushButton ("3", this);
    mUseSuggestion2->setMaximumSize(25,25);
    mUseSuggestion2->setFont(QFont ("WebDings",12,12,false));

    mComplexDlgSuggestionsGrid->addWidget(mUseSuggestion2, 1, 0, 1, 1);
    mComplexDlgSuggestionsGrid->addWidget(mSuggestion2, 1, 1, 1, 1);
    connect (this->mUseSuggestion2, SIGNAL (clicked ()), SLOT (mUseSuggestion2Clicked ()));
  }
  else
    mSuggestion2->setText(aValue);
}

void WizardDialogBox::setSuggestion3 (QString aValue)
{
  if (mType != eComplexDialog)
    return;

  if (0 == mSuggestion3)
  {
    mSuggestion3 = new QTextEdit (aValue, this);

    mUseSuggestion3 = new QPushButton ("3", this);
    mUseSuggestion3->setMaximumSize(25,25);
    mUseSuggestion3->setFont(QFont ("WebDings",12,12,false));

    mComplexDlgSuggestionsGrid->addWidget(mUseSuggestion3, 2, 0, 1, 1);
    mComplexDlgSuggestionsGrid->addWidget(mSuggestion3, 2, 1, 1, 1);
    connect (this->mUseSuggestion3, SIGNAL (clicked ()), SLOT (mUseSuggestion3Clicked ()));
  }
  else
    mSuggestion3->setText(aValue);
}

void WizardDialogBox::setSavedSuggestion (QString aValue)
{
  if (mComplexDlgEditField)
    mComplexDlgEditField->setText(aValue);
}

void WizardDialogBox::mUseSuggestion1Clicked ()
{
  mComplexDlgEditField->setText(mSuggestion1->toHtml());
}

void WizardDialogBox::mUseSuggestion2Clicked ()
{
  mComplexDlgEditField->setText(mSuggestion2->toHtml());
}

void WizardDialogBox::mUseSuggestion3Clicked ()
{
  mComplexDlgEditField->setText(mSuggestion3->toHtml());
}

void WizardDialogBox::aboutToAbort ()
{
  QMessageBox::StandardButton reply = QMessageBox::question(this, "Achtung", "Wizard abbrechen?", QMessageBox::Yes|QMessageBox::No);
  if (reply == QMessageBox::Yes)
    emit reject ();
}

void WizardDialogBox::onZustimmungMehrheit_clicked ()
{
  QString pattern ("");
  pattern = StringReplacer::getInstance()->getReplacementString("%verkuendungMehrheitlichAngenommen%");
  StringReplacer::getInstance()->findAndReplaceWildcards(&pattern);
  mBeschlussVorschau->setText(pattern);
}

void WizardDialogBox::onZustimmungEinstimmig_clicked ()
{
  QString pattern ("");
  pattern = StringReplacer::getInstance()->getReplacementString("%verkuendungEinstimmigAngenommen%");
  StringReplacer::getInstance()->findAndReplaceWildcards(&pattern);
  mBeschlussVorschau->setText(pattern);
}

void WizardDialogBox::onZustimmungAllstimmig_clicked ()
{
  QString pattern ("");
  pattern = StringReplacer::getInstance()->getReplacementString("%verkuendungAllstimmigAngenommen%");
  StringReplacer::getInstance()->findAndReplaceWildcards(&pattern);
  mBeschlussVorschau->setText(pattern);
}

void WizardDialogBox::onAblehnungMehrheit_clicked ()
{
  QString pattern ("");
  pattern = StringReplacer::getInstance()->getReplacementString("%verkuendungMehrheitlichAbgelehnt%");
  StringReplacer::getInstance()->findAndReplaceWildcards(&pattern);
  mBeschlussVorschau->setText(pattern);
}

void WizardDialogBox::onAblehnungEinstimmig_clicked ()
{
  QString pattern ("");
  pattern = StringReplacer::getInstance()->getReplacementString("%verkuendungEinstimmigAbgelehnt%");
  StringReplacer::getInstance()->findAndReplaceWildcards(&pattern);
  mBeschlussVorschau->setText(pattern);
}

void WizardDialogBox::onKeinBeschluss_clicked ()
{
  QString pattern ("");
  pattern = StringReplacer::getInstance()->getReplacementString("%verkuendungKeinBeschluss%");
  StringReplacer::getInstance()->findAndReplaceWildcards(&pattern);
  mBeschlussVorschau->setText(pattern);
}

void WizardDialogBox::addVotingOptions()
{
  if (0 != mBaselayout)
  {
    QPushButton* zustimmungMehrheit = new QPushButton ("Mehrheitlich Ja", this);
    QPushButton* zustimmungEinstimmig = new QPushButton ("Einstimmig Ja", this);
    QPushButton* zustimmungAllstimmig = new QPushButton ("Allstimmig Ja", this);
    QPushButton* ablehnungMehrheit = new QPushButton ("Mehrheitlich Nein", this);
    QPushButton* ablehnungEinstimmig = new QPushButton ("Einstimmig Nein", this);
    QPushButton* keinBeschluss = new QPushButton ("Kein Beschluss", this);

    connect (zustimmungMehrheit, SIGNAL (clicked ()), this, SLOT (onZustimmungMehrheit_clicked ()));
    connect (zustimmungEinstimmig, SIGNAL (clicked ()), this, SLOT (onZustimmungEinstimmig_clicked ()));
    connect (zustimmungAllstimmig, SIGNAL (clicked ()), this, SLOT (onZustimmungAllstimmig_clicked ()));
    connect (ablehnungMehrheit, SIGNAL (clicked ()), this, SLOT (onAblehnungMehrheit_clicked ()));
    connect (ablehnungEinstimmig, SIGNAL (clicked ()), this, SLOT (onAblehnungEinstimmig_clicked ()));
    connect (keinBeschluss, SIGNAL (clicked ()), this, SLOT (onKeinBeschluss_clicked ()));

    mBeschlussVorschau = new QTextEdit (this);

    QFrame* line = new QFrame(this);
    line->setObjectName(QString::fromUtf8("line"));
    line->setGeometry(QRect(320, 150, 118, 3));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    QLabel* stimmen = new QLabel ("Beschlussfassung Vorlagen:", this);

    mBaselayout->addWidget(line, 6, 0, 1, 4);
    mBaselayout->addWidget(stimmen, 7, 0, 1, 1);

    mButtonLayout = new QGridLayout();

    mButtonLayout->addWidget(zustimmungMehrheit, 8, 0, 1, 1);
    mButtonLayout->addWidget(zustimmungEinstimmig, 8, 1, 1, 1);
    mButtonLayout->addWidget(zustimmungAllstimmig, 8, 2, 1, 1);
    mButtonLayout->addWidget(ablehnungMehrheit, 9, 0, 1, 1);
    mButtonLayout->addWidget(ablehnungEinstimmig, 9, 1, 1, 1);
    mButtonLayout->addWidget(keinBeschluss, 9, 2, 1, 1);

    mBaselayout->addLayout(mButtonLayout, 8, 0, 1, 4);

    mBaselayout->addWidget(mBeschlussVorschau, 9, 0, 1, 4);

    QLabel* ja = new QLabel ("Ja:", this);
    QLabel* nein = new QLabel ("Nein:", this);
    QLabel* enthaltung = new QLabel ("Enthaltung:", this);

    mBaselayout->addWidget(ja, 10, 0, 1, 1);
    mBaselayout->addWidget(nein, 11, 0, 1, 1);
    mBaselayout->addWidget(enthaltung, 12, 0, 1, 1);

    mJaStimmen = new QDoubleSpinBox (this);
    mJaStimmen->setMaximum(100000);
    mNeinStimmen = new QDoubleSpinBox (this);
    mNeinStimmen->setMaximum(100000);
    mEnthaltungStimmen = new QDoubleSpinBox (this);
    mEnthaltungStimmen->setMaximum(100000);

    mBaselayout->addWidget(mJaStimmen, 10, 1, 1, 1);
    mBaselayout->addWidget(mNeinStimmen, 11, 1, 1, 1);
    mBaselayout->addWidget(mEnthaltungStimmen, 12, 1, 1, 1);
  }
}
