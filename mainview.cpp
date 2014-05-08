#include "mainview.h"
#include "ui_mainview.h"
#include "propertysettings.h"
#include "agendatab.h"
#include "propertytab.h"
#include "database.h"
#include <QSqlTableModel>
#include <QtWidgets>
#include <QtSql>
#include "ordertab.h"
#include "generatortab.h"
#include "patterneditor.h"
#include "patterneditorreport.h"
#include "systemsettings.h"
#include "reportgeneratortab.h"
#include "stringreplacer.h"
#include "decissionlibrarytab.h"

mainView::mainView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mainView)
{
    ui->setupUi(this);

    mAgendaTab = 0;
    mPropertyTab = 0;
    mOrderTab = 0;
    mGeneratorTab = 0;
    mReportGeneratorTab = 0;

    //generate tab classes
    mAgendaTab = new AgendaTab (this);
    mPropertyTab = new PropertyTab (this);
    mOrderTab = new OrderTab (this);
    mGeneratorTab = new GeneratorTab (this);
    mReportGeneratorTab = new ReportGeneratorTab (this);
    mDecissionLibraryTab = new DecissionLibraryTab (this);

    if (0 != mPropertyTab)
      ui->tabs->addTab(mPropertyTab, "ETV planen");
    if (0 != mAgendaTab)
      ui->tabs->addTab(mAgendaTab, "Tagesordnung vorbereiten");
    if (0 != mOrderTab)
      ui->tabs->addTab(mOrderTab, "Protokollvorlage vorbereiten");
    if (0 != mReportGeneratorTab)
      ui->tabs->addTab(mReportGeneratorTab, "Protokollabschrift vorbereiten");
    if (0 != mDecissionLibraryTab)
      ui->tabs->addTab(mDecissionLibraryTab, "Beschlusssammlung vorbereiten");
    if (0 != mGeneratorTab)
      ui->tabs->addTab(mGeneratorTab, "Dokumente ausgeben");

    ui->tabs->setCurrentWidget(mPropertyTab);

    connect (ui->tabs, SIGNAL(currentChanged(int)), this, SLOT(slotRefreshOnChangedTab (int)));

    mCurrentEstateId = 0;
    enableTabsOnValidData(false);
}

mainView::~mainView()
{
  if (Database::getInstance()->dbIsOk())
      db.close();

  /*delete mAgendaTab;
  delete mPropertyTab;
  delete mOrderTab;
  delete mReportGeneratorTab;
  delete mGeneratorTab;
  delete mDecissionLibraryTab;*/
  delete ui;
}

void mainView::etvSelectionValid (bool aValid)
{
  enableTabsOnValidData(aValid);
}

void mainView::enableTabsOnValidData (bool aEnable)
{
  if (0 != mAgendaTab)
    mAgendaTab->setEnabled(aEnable);

  if (0 != mOrderTab)
    mOrderTab->setEnabled(aEnable);

  if (0 != mGeneratorTab)
    mGeneratorTab->setEnabled(aEnable);

  if (0 != mReportGeneratorTab)
    mReportGeneratorTab->setEnabled(aEnable);

  if (0 != mDecissionLibraryTab)
    mDecissionLibraryTab->setEnabled(aEnable);
}

void mainView::slotRefreshOnChangedTab (int aIndex)
{
    switch (aIndex)
    {
      case ePropertySelectionTab:
        if (0 != mPropertyTab)
          mPropertyTab->refreshOnSelected ();
        break;
      case eAgendaTab:
        if (0 != mAgendaTab)
            mAgendaTab->refreshOnSelected();
        break;
      case eOrderTab:
        if (0 != mOrderTab)
            mOrderTab->refreshOnSelected();
        break;
      case eReportGeneratorTab:
        if (0 != mReportGeneratorTab)
          mReportGeneratorTab->refreshOnSelected();
        break;
      case eGeneratorTab:
        if (0 != mGeneratorTab)
          mGeneratorTab->refreshOnSelected();
        break;
      case eDecissionLibraryTab:
        if (0 != mDecissionLibraryTab)
          mDecissionLibraryTab->refreshOnSelected();
      default:
            break;
    }
}

/*bool mainView::setProperty (const char *name, const QVariant &value)
{
    if (name == "tab")
        setCurrentTab ((int)value.data());
}*/

void mainView::setCurrentTab (int aTab)
{
    switch (aTab)
    {
        case ePropertySelectionTab:
            if (0 != mPropertyTab)
                ui->tabs->setCurrentIndex(0);
            break;
        default:
            break;
    }
}

QSqlError mainView::addConnection(const QString &driver, const QString &dbName, const QString &host,
                                  const QString &user, const QString &passwd, int port)
{
    static int cCount = 0;

    QSqlError err;
    db = QSqlDatabase::addDatabase(driver, QString("Browser%1").arg(++cCount));
    db.setDatabaseName(dbName);
    db.setHostName(host);
    db.setPort(port);
    if (!db.open(user, passwd))
    {
        err = db.lastError();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(QString("Browser%1").arg(cCount));
    }
    //connectionWidget->refresh();

    Database::getInstance()->setDatabase(&db);

    return err;
}

void mainView::addConnection()
{
    QSqlConnectionDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    QSqlError err = addConnection(dialog.driverName(), dialog.databaseName(), dialog.hostName(),
                       dialog.userName(), dialog.password(), dialog.port());
    if (err.type() != QSqlError::NoError)
        QMessageBox::warning(this, tr("Unable to open database"), tr("An error occurred while "
                                   "opening the connection: ") + err.text());
}

void  mainView::patternSettings ()
{
    PatternEditor editor (this);
    editor.exec();
}

void mainView::reportSettings ()
{
    patternEditorReport editor (this);
    editor.exec();
}

void mainView::systemSettings()
{
    SystemSettings settings (this);
    settings.exec();
}

void mainView::addProperty()
{
  PropertySettings dialog (this);
  dialog.setVotingRule(StringReplacer::getInstance()->getReplacementString("%stimmrechtVorlageWEG%"));
  dialog.setInvitationDeadline(StringReplacer::getInstance()->getReplacementString("%einladungsfristVorlageWEG%"));
  dialog.setpropertyId(-99);

  //abort -> do not save settings
  if (dialog.exec() != QDialog::Accepted)
  {
      return;
  }
  else
  {
    if (Database::getInstance()->dbIsOk())
    {
      QSqlQuery query (*Database::getInstance()->getDatabase());
      //set values
      query.prepare("INSERT INTO Objekt (obj_name, obj_mea, obj_stimmrecht, obj_anz_et, obj_inv_deadline) VALUES (:name, :mea, :stimmrecht, :anz_et, :deadline)");
      query.bindValue(":name", dialog.propertyName());
      query.bindValue(":mea", QString::number(dialog.mea()));
      query.bindValue(":stimmrecht", dialog.votingRule());
      query.bindValue(":anz_et", QString::number(dialog.ownerQuantity()));
      query.bindValue(":deadline", dialog.getInvitationDeadline());
      query.exec();

      if (0 != mPropertyTab)
      {
        mPropertyTab->refreshView();
        mPropertyTab->refreshRealEstateSelector(false);
      }
    }
  }
}

void mainView::refreshView()
{
    mPropertyTab->refreshView();
    mPropertyTab->refreshRealEstateSelector(false);
}
