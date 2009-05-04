#include "xlsreader.h"

#include <stdio.h>
#include <QProcess>

xlsreader::xlsreader(QString file) :
        progName("xls2csv"),
        fileName(file)
{
}

#ifdef Q_OS_LINUX
void xlsreader::convertToCSV(QString outFileName) {

    FILE *pFile;
    pFile = fopen(outFileName.toStdString().c_str(), "w");
    fclose(pFile);

    QProcess proc;
    QStringList args;
    args << "-s 1251" << "-d utf8" << "-c ';'" << fileName;

    proc.setStandardOutputFile(outFileName);
    proc.start(progName, args);
    proc.waitForFinished();

}
#endif

#ifdef Q_OS_WIN32
void xlsreader::convertToCSV() {

}
#endif
