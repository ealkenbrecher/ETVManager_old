#ifndef WIZARDDIALOGBOX_H
#define WIZARDDIALOGBOX_H

class QLabel;
class QTextEdit;
class QLineEdit;
class QGridLayout;
class QPushButton;
class QCheckBox;
class QRadioButton;
class QDoubleSpinBox;
class QHBoxLayout;

#include <QDialog>

enum eDlgType
{
  eSimpleDialog,
  eComplexDialog,
  eCheckBoxDialog,
  eOrderDialog
};

enum eCheckBoxIndex
{
  eCheckBox1 = 0,
  eCheckBox2 = 1,
  eCheckBox3 = 2
};

enum eReturnCode
{
  eAborted,
  eOk,
};

class WizardDialogBox : public QDialog
{
    Q_OBJECT
  public:
    explicit WizardDialogBox(QWidget *parent = 0);
    explicit WizardDialogBox(QWidget *parent = 0, const char *pDialogText = 0, eDlgType aDlgType = eSimpleDialog);
    ~WizardDialogBox();
    void setDialogText (QString &rValue);
    void setSuggestion1 (QString rValue);
    void setSuggestion2 (QString rValue);
    void setSuggestion3 (QString rValue);
    void setSavedSuggestion (QString aValue);
    void setCheckBox (eCheckBoxIndex aIndex, QString aLabel);
    void setMultipleChoice (bool aAllowed);
    void addVotingOptions ();
    void setTitle (QString aTitle);

    float getVotingsYes ();
    float getVotingsNo ();
    float getVotingsConcordant ();
    QString getVoteText ();

    void setVotePreview (QString aValue);
    void setVotingsYes (float aValue);
    void setVotingsNo (float aValue);
    void setVotingsConcordant (float aValue);

    QString getResult ();
    void setResult (QString aValue);

    Qt::CheckState getCheckState (eCheckBoxIndex aIndex);

  private slots:
    void mUseSuggestion1Clicked ();
    void mUseSuggestion2Clicked ();
    void mUseSuggestion3Clicked ();
    void stateCheckBox1Changed (int aState);
    void stateCheckBox2Changed (int aState);
    void stateCheckBox3Changed (int aState);
    void aboutToAbort ();
    void onZustimmungMehrheit_clicked ();
    void onZustimmungEinstimmig_clicked ();
    void onZustimmungAllstimmig_clicked ();
    void onAblehnungMehrheit_clicked ();
    void onAblehnungEinstimmig_clicked ();
    void onKeinBeschluss_clicked ();

  private:
    void initDialog (eDlgType aDlgType);

    QLabel* mDescriptionText;
    QTextEdit* mSuggestion1;
    QTextEdit* mSuggestion2;
    QTextEdit* mSuggestion3;
    QLineEdit* mSimpleDlgEditField;
    QTextEdit* mComplexDlgEditField;

    QGridLayout* mBaselayout;
    QGridLayout* mComplexDlgSuggestionsGrid;
    QHBoxLayout* mHLayout;
    QGridLayout* mButtonLayout;

    QPushButton* mUseSuggestion1;
    QPushButton* mUseSuggestion2;
    QPushButton* mUseSuggestion3;

    QCheckBox* mCheckBox1;
    QCheckBox* mCheckBox2;
    QCheckBox* mCheckBox3;

    //QRadioButton* mBeschlusseingabeManuell;
    QTextEdit* mBeschlussVorschau;

    QDoubleSpinBox* mJaStimmen;
    QDoubleSpinBox* mNeinStimmen;
    QDoubleSpinBox* mEnthaltungStimmen;

    QPushButton* mOkButton;

    eDlgType mType;

    bool mMultipleChoice;
};

#endif // WIZARDDIALOGBOX_H
