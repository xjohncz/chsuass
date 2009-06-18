#include "reportcreator.h"

#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QDir>

reportcreator::reportcreator(const QString &rep)
{
    report = rep;
}

reportcreator::~reportcreator() {
}

void reportcreator::createCardReport(const QMap<int, QString> &map) {

    QString templatePath = QDir::currentPath() + "/templates/card/";

    QTextCodec *cp1251_codec = QTextCodec::codecForName("cp1251");

    QFile file;
    QTextStream textStream(&file);
    textStream.setCodec(cp1251_codec);
    QString resultReport;

    file.setFileName(templatePath + "card_begin.htm");
    file.open(QIODevice::ReadOnly);
    resultReport = textStream.readAll();
    file.close();

    QString cardHeader;
    QString cardCont;
    QString cardFooter;

    file.setFileName(templatePath + "card_header.htm");
    file.open(QIODevice::ReadOnly);
    cardHeader = textStream.readAll();
    file.close();

    file.setFileName(templatePath + "card_cont.htm");
    file.open(QIODevice::ReadOnly);
    cardCont = textStream.readAll();
    file.close();

    file.setFileName(templatePath + "card_footer.htm");
    file.open(QIODevice::ReadOnly);
    cardFooter = textStream.readAll();
    file.close();

    QMapIterator<int, QString> i(map);
    int count = 1;
    while(i.hasNext()) {
        i.next();

        QString head = cardHeader;
        head.replace(QString("%cnum%"), QString::number(i.key()));

        QString cont = cardCont;
        QString questions = i.value();
        questions.replace(QString("\n"), QString("<br>"));

        cont.replace(QString("%card_contents%"), questions);

        resultReport = resultReport + head + cont + cardFooter;

        if(count % 2 != 0)
            resultReport = resultReport + "\n<hr>\n";

        count++;
    }

    file.setFileName(templatePath + "card_end.htm");
    file.open(QIODevice::ReadOnly);
    resultReport = resultReport + textStream.readAll();
    file.close();

    report = resultReport;

}

void reportcreator::insertList(const QStringList &list, const QString &replaceString, const QString &htmlString, const QString &spanString,
                               QString &resReport) {

    if(list.isEmpty())
        resReport.replace(replaceString, "");
    else {
        resReport.replace(replaceString, list.at(0));
        for(int i = 1; i < list.count(); i++) {
            int idx = resReport.indexOf(spanString);
            QString replStr = htmlString;
            replStr.replace(replaceString, list.at(i));
            resReport.insert(idx, replStr);
        }
    }

}

void reportcreator::createGEKReport(const QString &student, const QString &president, const QStringList &members,
                                   const QDate &date, const QTime &begin_time, const QTime &end_time,
                                   int cardNumber, const QString &question1, const QString &question2, const QString &question3,
                                   const QString &characteristic, const QString &notes, const QString &opinions,
                                   int resultMark) {

    QString templatePath = QCoreApplication::applicationDirPath() + "/templates/gek_protocol/";

    QTextCodec *cp1251_codec = QTextCodec::codecForName("cp1251");

    QFile file;
    QTextStream textStream(&file);
    textStream.setCodec(cp1251_codec);
    QString resultReport;

    file.setFileName(templatePath + "gek_protocol_cont.htm");
    file.open(QIODevice::ReadOnly);
    resultReport = textStream.readAll();
    file.close();

    QString reportCharacter;
    QString reportMember;
    QString reportNote;
    QString reportOpinion;
    QString reportSignature;    

    file.setFileName(templatePath + "gek_protocol_character.htm");
    file.open(QIODevice::ReadOnly);
    reportCharacter = textStream.readAll();
    file.close();

    file.setFileName(templatePath + "gek_protocol_member.htm");
    file.open(QIODevice::ReadOnly);
    reportMember = textStream.readAll();
    file.close();

    file.setFileName(templatePath + "gek_protocol_note.htm");
    file.open(QIODevice::ReadOnly);
    reportNote = textStream.readAll();
    file.close();

    file.setFileName(templatePath + "gek_protocol_opinion.htm");
    file.open(QIODevice::ReadOnly);
    reportOpinion = textStream.readAll();
    file.close();

    file.setFileName(templatePath + "gek_protocol_signature.htm");
    file.open(QIODevice::ReadOnly);
    reportSignature = textStream.readAll();
    file.close();

    resultReport.replace(QString("%day%"), QString::number(date.day()));
    resultReport.replace(QString("%month%"), QDate::longMonthName(date.month()));
    resultReport.replace(QString("%year%"), QString::number(date.year()));

    resultReport.replace(QString("%beg_hour%"), QString::number(begin_time.hour()));
    resultReport.replace(QString("%beg_min%"), QString::number(begin_time.minute()));
    resultReport.replace(QString("%end_hour%"), QString::number(end_time.hour()));
    resultReport.replace(QString("%end_min%"), QString::number(end_time.minute()));

    resultReport.replace(QString("%student%"), student);
    resultReport.replace(QString("%cardnumber%"), QString::number(cardNumber));

    resultReport.replace(QString("%question1%"), question1);
    resultReport.replace(QString("%question2%"), question2);
    resultReport.replace(QString("%question3%"), question3);

    if(resultMark == 2)
        resultReport.replace(QString("%resultmark%"), QObject::tr("Плохо"));
    else if (resultMark == 3)
        resultReport.replace(QString("%resultmark%"), QObject::tr("Удовлетворительно"));
    else if (resultMark == 4)
        resultReport.replace(QString("%resultmark%"), QObject::tr("Хорошо"));
    else if (resultMark == 5)
        resultReport.replace(QString("%resultmark%"), QObject::tr("Отлично"));
    else
        resultReport.replace(QString("%resultmark%"), QObject::tr(""));

    resultReport.replace(QString("%president%"), president);
    insertList(members, "%member%", reportMember, "<span id=\"member_row\"></span>", resultReport);

    for(int i = 1; i < members.count(); i++) {
        int idx = resultReport.indexOf(QString("<span id=\"signature\"></span>"));
        resultReport.insert(idx, reportSignature);
    }

    QStringList characters = characteristic.split("\n", QString::SkipEmptyParts, Qt::CaseInsensitive);
    insertList(characters, "%characteristic%", reportCharacter, "<span id=\"answer_characteristic\"></span>", resultReport);

    QStringList notesList = notes.split("\n", QString::SkipEmptyParts, Qt::CaseInsensitive);
    insertList(notesList, "%note%", reportNote, "<span id=\"note\"></span>", resultReport);

    QStringList opinionsList = opinions.split("\n", QString::SkipEmptyParts, Qt::CaseInsensitive);
    insertList(opinionsList, "%opinion%", reportOpinion, "<span id=\"special_opinion\"></span>", resultReport);

    report = resultReport;

}

void reportcreator::createGAKReport(const QString &student, const QString &president, const QString &secretary,
                                   const QString &instructor, const QString &consultant, const QStringList &members,
                                   const QDate &date, const QTime &begin_time, const QTime &end_time,
                                   const QString &theme, int wrccount, int postercount, int questions_time,
                                   const QStringList &addQuestions, const QString &characteristic,
                                   const QString &notes, const QString &opinions, int resultMark) {

    QString templatePath = QCoreApplication::applicationDirPath() + "/templates/gak_protocol/";

    QTextCodec *cp1251_codec = QTextCodec::codecForName("cp1251");

    QFile file;
    QTextStream textStream(&file);
    textStream.setCodec(cp1251_codec);
    QString resultReport;

    file.setFileName(templatePath + "gak_protocol_cont.htm");
    file.open(QIODevice::ReadOnly);
    resultReport = textStream.readAll();
    file.close();

    QString reportCharacter;
    QString reportMember;
    QString reportNote;
    QString reportOpinion;
    QString reportSignature;
    QString reportQuestion;

    file.setFileName(templatePath + "gak_protocol_character.htm");
    file.open(QIODevice::ReadOnly);
    reportCharacter = textStream.readAll();
    file.close();

    file.setFileName(templatePath + "gak_protocol_member.htm");
    file.open(QIODevice::ReadOnly);
    reportMember = textStream.readAll();
    file.close();

    file.setFileName(templatePath + "gak_protocol_note.htm");
    file.open(QIODevice::ReadOnly);
    reportNote = textStream.readAll();
    file.close();

    file.setFileName(templatePath + "gak_protocol_opinion.htm");
    file.open(QIODevice::ReadOnly);
    reportOpinion = textStream.readAll();
    file.close();

    file.setFileName(templatePath + "gak_protocol_addquestion.htm");
    file.open(QIODevice::ReadOnly);
    reportQuestion = textStream.readAll();
    file.close();

    file.setFileName(templatePath + "gak_protocol_signature.htm");
    file.open(QIODevice::ReadOnly);
    reportSignature = textStream.readAll();
    file.close();

    resultReport.replace(QString("%day%"), QString::number(date.day()));
    resultReport.replace(QString("%month%"), QDate::longMonthName(date.month()));
    resultReport.replace(QString("%year%"), QString::number(date.year()));

    resultReport.replace(QString("%beg_hour%"), QString::number(begin_time.hour()));
    resultReport.replace(QString("%beg_min%"), QString::number(begin_time.minute()));
    resultReport.replace(QString("%end_hour%"), QString::number(end_time.hour()));
    resultReport.replace(QString("%end_min%"), QString::number(end_time.minute()));

    resultReport.replace(QString("%student%"), student);
    resultReport.replace(QString("%student_vin%"), student);
    resultReport.replace(QString("%theme%"), theme);

    if(resultMark == 2)
        resultReport.replace(QString("%resultmark%"), QObject::tr("Плохо"));
    else if (resultMark == 3)
        resultReport.replace(QString("%resultmark%"), QObject::tr("Удовлетворительно"));
    else if (resultMark == 4)
        resultReport.replace(QString("%resultmark%"), QObject::tr("Хорошо"));
    else if (resultMark == 5)
        resultReport.replace(QString("%resultmark%"), QObject::tr("Отлично"));
    else
        resultReport.replace(QString("%resultmark%"), QObject::tr(""));

    resultReport.replace(QString("%president%"), president);
    resultReport.replace(QString("%instructor%"), instructor);
    resultReport.replace(QString("%consultant%"), consultant);
    insertList(members, "%member%", reportMember, "<span id=\"member_row\"></span>", resultReport);

    for(int i = 1; i < members.count(); i++) {
        int idx = resultReport.indexOf(QString("<span id=\"signature\"></span>"));
        resultReport.insert(idx, reportSignature);
    }

    resultReport.replace(QString("%wrccount%"), QString::number(wrccount));
    resultReport.replace(QString("%postercount%"), QString::number(postercount));
    resultReport.replace(QString("%minutecount%"), QString::number(questions_time));

    insertList(addQuestions, "%addquestion%", reportQuestion, "<span id=\"addquestion_row\"></span>", resultReport);

    QStringList characters = characteristic.split("\n", QString::SkipEmptyParts, Qt::CaseInsensitive);
    insertList(characters, "%characteristic%", reportCharacter, "<span id=\"answer_characteristic\"></span>", resultReport);

    QStringList notesList = notes.split("\n", QString::SkipEmptyParts, Qt::CaseInsensitive);
    insertList(notesList, "%note%", reportNote, "<span id=\"note\"></span>", resultReport);

    QStringList opinionsList = opinions.split("\n", QString::SkipEmptyParts, Qt::CaseInsensitive);
    insertList(opinionsList, "%opinion%", reportOpinion, "<span id=\"special_opinion\"></span>", resultReport);

    report = resultReport;
}

void reportcreator::writeReport(const QString &fileName) {

    QTextCodec *cp1251_codec = QTextCodec::codecForName("cp1251");
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    QTextStream textStream(&file);
    textStream.setCodec(cp1251_codec);

    textStream << report;
    textStream.flush();

    file.close();

    QString logo = QDir::currentPath() + "/templates/chsu_logo.jpg";
    QString path = fileName;
    path.truncate(path.lastIndexOf("/"));

    QFile::copy(logo, path + "/chsu_logo.jpg");

}
