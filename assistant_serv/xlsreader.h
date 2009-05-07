#ifndef XLSREADER_H
#define XLSREADER_H

#include <QString>
#include <QMap>

class xlsreader
{
public:
    xlsreader(QString file);

    QString getXLSFileName() { return fileXLS; }
    void setXLSFileName(QString fileName) { fileXLS = fileName; }

    void convertToCSV(QString outFileName);

    QStringList readSubjectsFromCSV(QString fileName);
    QMap<QString, int> readStudentMarksFromCSV(QString fileName);
    QMap<int, QString> readGroupFromCSV(QString fileName);

    QStringList readSubjectsFromXLSStudentCard(QString fileName);
    QMap<QString, int> readStudentMarksFromXLSStudentCard(QString fileName);
    QMap<int, QString> readGroupXLS(QString fileName);

protected:
    QString getSectionFromCSV(QString record, int beg, int end);
    QString prepareTempFile(QString fileName);
    void cleanTempFile(QString tempFile);

private:
    QString fileXLS;
};

#endif // XLSREADER_H
