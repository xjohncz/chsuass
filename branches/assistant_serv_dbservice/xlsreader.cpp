#include "xlsreader.h"

#include <stdio.h>
#include <QProcess>
#include <QDir>
#include <QTextStream>
#include <QTextCodec>
#include <QtGui>

xlsreader::xlsreader()
{
}

void xlsreader::convertToCSV(const QString &outFileName) {

    if(fileXLS.isEmpty())
        return;

    FILE *pFile;
    pFile = fopen(outFileName.toStdString().c_str(), "w");
    fclose(pFile);

    QProcess proc;
    proc.setStandardOutputFile(outFileName);

#ifdef Q_OS_LINUX
    proc.start("xls2csv", QStringList() << "-s" << "cp1251" << "-d" << "utf-8" << "-c" << ";" << fileXLS);
#endif

#ifdef Q_OS_WIN32
    QString xls2csv = QDir::currentPath() + "/libs/xls2csv/xls2csv.exe";
    proc.start(xls2csv, QStringList() << "-s" << "cp1251" << "-d" << "utf-8" << "-c" << ";" << fileXLS);
#endif

    proc.waitForFinished();

}

QString xlsreader::getSectionFromCSV(const QString &record, int beg, int end) {

    QString str = record.section(";", beg, end);
    return str.mid(1, str.length() - 2);

}

QString xlsreader::prepareTempFile() {

    QDir tempDir = QDir::homePath();
    tempDir.mkdir("TempCSV");
    tempDir.cd("TempCSV");

    QString tempCSV = tempDir.absolutePath() + "/temp.csv";
    convertToCSV(tempCSV);

    return tempCSV;

}

void xlsreader::cleanTempFile() {

    QDir tempDir = QDir::homePath();
    tempDir.cd("TempCSV");
    tempDir.remove("temp.csv");
    tempDir.cdUp();
    tempDir.rmdir("TempCSV");

}

QStringList xlsreader::readSubjectsFromCSV(const QString &fileName) {

    QStringList resList;

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QTextStream textStream(&file);

    QTextCodec *utf8_codec = QTextCodec::codecForName("utf-8");
    textStream.setCodec(utf8_codec);

    while(!textStream.atEnd()) {

        QString str = textStream.readLine();
        QString examType = getSectionFromCSV(str, 33, 33);

        if(examType != QObject::trUtf8("Экзамен"))
            continue;

        QString subject = getSectionFromCSV(str, 30, 30);
        resList.append(subject);
    }

    file.close();

    return resList;
}

QStringList xlsreader::readSubjectsFromXLSStudentCard() {

    QString tempCSV = prepareTempFile();
    QStringList subjList = readSubjectsFromCSV(tempCSV);
    cleanTempFile();

    return subjList;

}

QMap<QString, int> xlsreader::readStudentMarksFromCSV(const QString &fileName) {

    QMap<QString, int> resMarks;

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QTextStream textStream(&file);

    QTextCodec *utf8_codec = QTextCodec::codecForName("utf-8");
    textStream.setCodec(utf8_codec);

    while(!textStream.atEnd()) {

        QString str = textStream.readLine();
        QString examType = getSectionFromCSV(str, 33, 33);

        if(examType != QObject::trUtf8("Экзамен"))
            continue;

        QString subject = getSectionFromCSV(str, 30, 30);
        QString strMark = getSectionFromCSV(str, 34, 34);

        int mark = 0;
        if(strMark == QObject::trUtf8("удовлетворительно"))
            mark = 3;
        else if(strMark == QObject::trUtf8("хорошо"))
            mark = 4;
        else if(strMark == QObject::trUtf8("отлично"))
            mark = 5;

        resMarks.insert(subject, mark);
    }

    file.close();

    return resMarks;
}

QMap<QString, int> xlsreader::readStudentMarksFromXLSStudentCard() {

    QString tempCSV = prepareTempFile();
    QMap<QString, int> studentMarks = readStudentMarksFromCSV(tempCSV);
    cleanTempFile();

    return studentMarks;

}

QMap<int, QString> xlsreader::readGroupFromCSV(const QString &fileName) {

    QMap<int, QString> resGroup;

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QTextStream textStream(&file);

    QTextCodec *utf8_codec = QTextCodec::codecForName("utf-8");
    textStream.setCodec(utf8_codec);

    while(!textStream.atEnd()) {

        QString str = textStream.readLine();
        QString recType = getSectionFromCSV(str, 5, 5);

        if(recType != QObject::trUtf8("Студент"))
            continue;

        QString strStudentNum = getSectionFromCSV(str, 7, 7);
        QString student = getSectionFromCSV(str, 9, 9);

        int studentNum = strStudentNum.toInt();

        resGroup.insert(studentNum, student);
    }

    file.close();

    return resGroup;
}

QMap<int, QString> xlsreader::readGroupXLS() {

    QString tempCSV = prepareTempFile();
    QMap<int, QString> group = readGroupFromCSV(tempCSV);
    cleanTempFile();

    return group;

}
