#ifndef XLSREADER_H
#define XLSREADER_H

#include <QString>

class xlsreader
{
public:
    xlsreader(QString file);

#ifdef Q_OS_LINUX
    void convertToCSV(QString outFileName);
#endif

#ifdef Q_OS_WIN32
    void convertToCSV();
#endif

private:
    QString progName;
    QString fileName;
};

#endif // XLSREADER_H
