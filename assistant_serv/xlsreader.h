#ifndef XLSREADER_H
#define XLSREADER_H

#include <QString>
#include <QMap>

class xlsreader
{
public:
    xlsreader(QString file);
    void convertToCSV(QString outFileName);

    static void convertToCSV(QString inpFileName, QString outFileName);
    static QStringList readSubjectsFromStudentCard(QString fileName);
    static QMap<QString, int> readStudentMarksFromStudentCard(QString fileName);

protected:
    static QString getSectionFromCSV(QString record, int beg, int end);

private:
    QString fileName;
};

#endif // XLSREADER_H
