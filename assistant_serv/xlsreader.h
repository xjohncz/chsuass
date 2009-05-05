#ifndef XLSREADER_H
#define XLSREADER_H

#include <QString>

class xlsreader
{
public:
    xlsreader(QString file);
    void convertToCSV(QString outFileName);

private:
    QString progName;
    QString fileName;
};

#endif // XLSREADER_H
