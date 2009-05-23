#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtSql>
#include <QtGui>
#include <QCloseEvent>
#include <QStandardItemModel>

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

    int getSelectedRowFromTableView(QTableView *view);

private:
    void initGroups();
    void initSubjects();
    void initCards();
    void initThemes();
    void initMembers();
    void initExamTypes();
    
    //void fillDBConnection(QString dbuser, QString dbpass, QString dbname=QString("assistant_schema"), QString dbhost=QString("localhost"), int dbport=3306);

    void showSelectDialog(const QString &tableName, IdType type);

    Ui::MainWindowClass *ui;

    dbservice *dbServ;
    xlsreader *xlsRead;

    //QSqlDatabase db;
    //QSqlTableModel *groupsTableModel;
    //QSqlTableModel *studentsTableModel;
    //QSqlTableModel *cardsTableModel;
    //QSqlTableModel *themesTableModel;
    //QSqlRelationalTableModel *membersTableModel;

    ServerDaemon *daemon;
    int selectedGroupID;

    IdType currentIdType;

    //CURRENT EXAM
    int currentExamID;
    int currentExamTypeID;
    int currentExamSelectedStudentID;
    QStandardItemModel *memberListModel;
    QSqlQueryModel *currentExamStudentListModel;
    QSqlQueryModel *currentExamStudenMarksModel;
    //CURRENT EXAM

private slots:
    void authenticationClientSlot(QString username, int client);
    void removeUserSlot(QString username);
    void studentRequestGrantedSlot();
    void saveStudentResultsSlot(int, QString, int, int, int, int, int, int);
    void slotExportStudents(int client);
    void slotExportCards(int client);

private slots:
    void on_showStudentInfoButton_clicked();
    void on_marksImportButton_clicked();
    void on_saveExamTimeButton_clicked();
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
