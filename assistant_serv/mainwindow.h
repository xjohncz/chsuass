#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "serverdaemon.h"

#include <QtSql>
#include <QtGui>
#include <QCloseEvent>
#include <QStandardItemModel>

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

private:
    void initGroups();
    void initCards();
    void initThemes();
    void initMembers();
    void initUsers();
    void initExamTypes();
    
    void fillDBConnection(QString dbuser, dbpass, dbname=QString("assistant_schema"), dbhost=QString("localhost"), dbport=QString("3306"));

    void deleteRowFromTableModel(QSqlTableModel *, QTableView *);
    void addRowToTableModel(QSqlTableModel *);
    void revertChanges(QSqlTableModel *);
    void submitChanges(QSqlTableModel *);
    void showSelectDialog(const QString &tableName, IdType type);

    Ui::MainWindowClass *ui;

    QSqlDatabase db;
    QSqlTableModel *groupsTableModel;
    QSqlTableModel *studentsTableModel;
    QSqlTableModel *cardsTableModel;
    QSqlTableModel *themesTableModel;
    QSqlRelationalTableModel *membersTableModel;
    QSqlTableModel *usersTableModel;

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

public slots:
    void authenticationClient(QString username, QString passHash, int client);
    void removeUserSlot(QString username);
    void studentRequestGrantedSlot();
    void saveStudentResultsSlot(int, QString, int, int, int, int, int, int);

private slots:
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
