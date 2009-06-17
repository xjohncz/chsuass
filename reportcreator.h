#ifndef REPORTCREATOR_H
#define REPORTCREATOR_H

#include <QMap>
#include <QStringList>
#include "dbservice.h"

class reportcreator
{
public:
    reportcreator(/*dbservice *dbs*/);
    ~reportcreator();
    static QString createCardReport(const QMap<int, QString> &map);
    static QString createGEKReport(const QString &student, const QString &president, const QStringList &members,
                                   const QDate &date, const QTime &begin_time, const QTime &end_time,
                                   int cardNumber, const QString &question1, const QString &question2, const QString &question3,
                                   const QString &characteristic, const QString &notes, const QString &opinions,
                                   int resultMark);
    static QString createGAKReport(const QString &student, const QString &president, const QString &secretary,
                                   const QString &instructor, const QString &consultant, const QStringList &members,
                                   const QDate &date, const QTime &begin_time, const QTime &end_time,
                                   const QString &theme, int wrccount, int postercount, int questions_time,
                                   const QStringList &addQuestions, const QString &characteristic,
                                   const QString &notes, const QString &opinions, int resultMark);
    static void writeReport(const QString &fileName, const QString &report);

protected:
    static void insertList(const QStringList &list, const QString &replaceString, const QString &htmlString,
                           const QString &spanString, QString &report);

private:
    //dbservice *dbServ;

};

#endif // REPORTCREATOR_H
