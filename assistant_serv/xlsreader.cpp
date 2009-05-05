#include "xlsreader.h"

#include <stdio.h>
#include <QProcess>
#include <QDir>

xlsreader::xlsreader(QString file) :
        progName("xls2csv"),
        fileName(file)
{
}

void xlsreader::convertToCSV(QString outFileName) {

#ifdef Q_OS_LINUX

    FILE *pFile;
    pFile = fopen(outFileName.toStdString().c_str(), "w");
    fclose(pFile);

    QProcess proc;
    QStringList args;
    args << "-s 1251" << "-d utf8" << "-c ';'" << fileName;

    proc.setStandardOutputFile(outFileName);
    proc.start(progName, args);
    proc.waitForFinished();

#endif

#ifdef Q_OS_WIN32

    QDir tempDir(QDir::currentPath());
    tempDir.mkdir("TempCSVDir");
    tempDir.cd("TempCSVDir");

    QString csvDir = QDir::currentPath() + "/TempCSVDir";
    QString xls2csv = QDir::currentPath() + "/libs/xls2csv.exe";
    QString tempFile = csvDir + "/tempFile.xls";
    QFile::copy(fileName, tempFile);

    QProcess proc;
    QStringList args;
    args << tempFile << "cp1251";

    proc.start(xls2csv, args);
    proc.waitForFinished();

    QStringList nameFilters;
    nameFilters << "*.csv";
    QStringList fileList = tempDir.entryList(nameFilters, QDir::Files);
    QString csvFile = tempDir.absoluteFilePath(fileList.at(0));
    QFile::copy(csvFile, outFileName);

    fileList = tempDir.entryList(QDir::Files);
    for(int i = 0; i < fileList.count(); i++)
        tempDir.remove(fileList.at(i));
    tempDir.cdUp();
    tempDir.rmdir("TempCSVDir");

#endif

}
