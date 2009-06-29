#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtSql>
#include <QtGui>
#include <QCloseEvent>
#include <QStandardItemModel>
#include <QtWebKit>

#include "serverdaemon.h"
#include "dbservice.h"
#include "xlsreader.h"

enum IdType {
    Student,
    Consultant,
    Instructor
};

namespace Ui
{
    class MainWindowClass;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *, QEvent *);

    void initEventFilterInstalls();
    void showTableViewTooltip(QTableView *object, QEvent *event, const QString &name, int column);

    int getSelectedRowFromTableView(QTableView *view);
    void showRowInTableView(QTableView *view, int column, int id);
    void filterNewExamStudentsTableView(int column);
    void addPresidentOrSecretary(QTableWidget *tableWidget);
    void removePresidentOrSecretary(QTableWidget *tableWidget);

    void createStudentReport(int studentId, int examId, const QString &fileName);

private:
    void readConfig();

    void initSignalConnections();

    void initGroups();
    void initSubjects();
    void initCards();
    void initThemes();
    void initInstructors();
    void initMembers();
    void initExamTypes();
    void initExams();
    void initNewExam();
    void initCurrentExam();

    void showSelectDialog(const QString &tableName, IdType type);

    void startPrint();

    Ui::MainWindowClass *ui;

    dbservice *dbServ;
    xlsreader *xlsRead;

    ServerDaemon *daemon;
    int selectedGroupID;

    IdType currentIdType;

    //THEMES
    int themeStudentId;
    int themeConsultantId;
    int themeInstructorId;
    //THEMES

    //CURRENT EXAM
    int currentExamID;
    int currentExamTypeID;
    int currentExamSelectedStudentID;
    int currentExamSentStudentId;
    //CURRENT EXAM

    //PRINTER SETTINGS
    QPrinter *printer;
    QStringList printList;
    int printIndex;
    //PRINTER

    QHttp *http;

private slots:
    void on_printCardsButton_clicked();
    void on_btnSaveSettings_clicked();
    void printWebView(bool);
    void on_btnCreateReports_clicked();
    void on_removeQuestionButton_clicked();
    void on_btnRemoveExam_clicked();
    void on_saveThemeButton_clicked();
    void on_themesTableView_pressed(QModelIndex index);
    void on_applyInstructorsButton_clicked();
    void on_cancelInstructorsButton_clicked();
    void on_addInstructorButton_clicked();
    void on_deleteInstructorButton_clicked();
    //void onRequestFinished(int, bool);
    void on_saveQuestionButton_clicked();
    void on_btnCurrentExamSaveCharacteristic_clicked();
    void on_btnCurrentExamSaveResultMark_clicked();
    void on_btnCurrentExamSaveWrcCount_clicked();
    void on_btnSaveCurrentExamTime_clicked();
    void on_btnExamPrintStudentReport_clicked();
    void on_btnExamSaveStudentReport_clicked();
    void on_btnMakeExamCurrent_clicked();
    void on_tvExamStudents_pressed(QModelIndex index);
    void on_tvExamList_pressed(QModelIndex index);
    void on_addSecretaryToExamButton_clicked();
    void on_addPresidentToExamButton_clicked();
    void on_removeSecretaryButton_clicked();
    void on_removePresidentButton_clicked();
    void on_studentsTableView_pressed(QModelIndex index);
    void on_showStudentMarksButton_clicked();
    void on_saveNewExam_clicked();
    void on_deleteAllMemberFromExamButton_clicked();
    void on_addAllMemberToExamButton_clicked();
    void on_deleteMemberFromExamButton_clicked();
    void on_addMemberToExamButton_clicked();
    void on_deleteAllStudentFromExamButton_clicked();
    void on_addAllStudentsToExamButton_clicked();
    void on_deleteStudentFromExamButton_clicked();
    void on_addStudentToExamButton_clicked();
    void on_applyMembersButton_clicked();
    void on_cancelMembersButton_clicked();
    void on_addMemberButton_clicked();
    void on_deleteMemberButton_clicked();
    //void on_applyThemesButton_clicked();
    //void on_cancelThemesButton_clicked();
    //void on_addThemeButton_clicked();
    void on_deleteThemeButton_clicked();
    void on_applySubjectsButton_clicked();
    void on_cancelSubkectsButton_clicked();
    void on_addSubjectButton_clicked();
    void on_deleteSubjectButton_clicked();
    void on_importSubjectsButton_clicked();
    void on_exportCardsButton_clicked();
    void slotClientAuthentication(QString username, int client);
    void slotRemoveUserSlot(QString username);
    void slotExportStudents(int client);
    void slotExportCards(int client);
    void slotSaveResults(QString xmlDoc);

private slots:
    void on_recvResultsButton_clicked();
    void on_showStudentInfoButton_clicked();
    void on_applyStudentsButton_clicked();
    void on_cancelStudentsButton_clicked();
    void on_addStudentButton_clicked();
    void on_deleteStudentButton_clicked();
    void on_applyGroupsButton_clicked();
    void on_cancelGroupsButton_clicked();
    void on_addGroupButton_clicked();
    void on_deleteGroupButton_clicked();
    void on_currentExamSaveCardNumberButton_clicked();
    void on_sendStudentInfoButton_clicked();
    void on_currentExamStudentListTableView_clicked(QModelIndex index);
    void on_serverButton_clicked();
    void on_fillCurrentExamButton_clicked();
    void on_filterButton_clicked();
    void on_refreshFilterListsButton_clicked();
    void on_browseInstructorButton_clicked();
    void on_browseConsultantButton_clicked();
    void onIdSelected(int id);

    void on_browseStudentButton_clicked();
    void on_applyCardsButton_clicked();
    void on_cancelCardsButton_clicked();
    void on_addCardButton_clicked();
    void on_deleteCardButton_clicked();
    void on_groupsTableView_pressed(QModelIndex index);
};

#endif // MAINWINDOW_H
