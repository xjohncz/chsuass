#include "xlsreader.h"

#include <stdio.h>
#include <QProcess>
#include <QDir>

xlsreader::xlsreader(QString file) :
        fileName(file)
{
}

/*FIXME: check if outFileName is absolute path name*/
void xlsreader::convertToCSV(QString inpFileName, QString outFileName) {

    FILE *pFile;
    pFile = fopen(outFileName.toStdString().c_str(), "w");
    fclose(pFile);

    QProcess proc;
    proc.setStandardOutputFile(outFileName);

#ifdef Q_OS_LINUX
    proc.start("xls2csv", QStringList() << "-s" << "1251" << "-d" << "utf8" << "-c" << ";" << inpFileName);
#endif

#ifdef Q_OS_WIN32
    QString xls2csv = QDir::currentPath() + "/libs/xls2csv/xls2csv.exe";
    proc.start(xls2csv, QStringList() << "-s" << "1251" << "-d" << "utf8" << "-c" << ";" << inpFileName);
#endif

    proc.waitForFinished();

}

void xlsreader::convertToCSV(QString outFileName) {

    return convertToCSV(fileName, outFileName);

}

QString xlsreader::getSectionFromCSV(QString record, int beg, int end) {

    QString str = record.section(";", beg, end);
    return str.mid(1, str.length() - 2);

}

QStringList xlsreader::readSubjectsFromStudentCard(QString fileName) {

    QStringList resList;

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);

    while(!file.atEnd()) {

        QString str = file.readLine();
        QString examType = getSectionFromCSV(str, 33, 33);

        if(examType != "Экзамен")
            continue;

        QString subject = getSectionFromCSV(str, 30, 30);
        resList.append(subject);
    }

    file.close();

    return resList;
}

QMap<QString, int> xlsreader::readStudentMarksFromStudentCard(QString fileName) {

    QMap<QString, int> resMarks;

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);

    while(!file.atEnd()) {

        QString str = file.readLine();
        QString examType = getSectionFromCSV(str, 33, 33);

        if(examType != "Экзамен")
            continue;

        QString subject = getSectionFromCSV(str, 30, 30);
        QString strMark = getSectionFromCSV(str, 34, 34);

        int mark = 0;
        if(strMark == "удовлетворительно")
            mark = 3;
        else if(strMark == "хорошо")
            mark = 4;
        else if(strMark == "отлично")
            mark = 5;

        resMarks.insert(subject, mark);
    }

    file.close();

    return resMarks;
}

QMap<int, QString> xlsreader::readGroup(QString fileName) {

    QMap<int, QString> resGroup;

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);

    while(!file.atEnd()) {

        QString str = file.readLine();
        QString recType = getSectionFromCSV(str, 5, 5);

        if(examType != "Студент")
            continue;

        QString strStudentNum = getSectionFromCSV(str, 7, 7);
        QString student = getSectionFromCSV(str, 9, 9);

        int studentNum = strStudentNum.toInt();

        resMarks.insert(studentNum, student);
    }

    file.close();

    return resGroup;
}
