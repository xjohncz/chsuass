#ifndef XLSREADER_H
#define XLSREADER_H

#include <QString>
#include <QMap>

class xlsreader
{
public:
    xlsreader();

    QString getXLSFileName() { return fileXLS; }
    void setXLSFileName(const QString &fileName) { fileXLS = fileName; }

    void convertToCSV(const QString &outFileName);

    QStringList readSubjectsFromCSV(const QString &fileName);
    QMap<QString, int> readStudentMarksFromCSV(const QString &fileName);
    QMap<int, QString> readGroupFromCSV(const QString &fileName);

    QStringList readSubjectsFromXLSStudentCard();
    QMap<QString, int> readStudentMarksFromXLSStudentCard();
    QMap<int, QString> readGroupXLS();

protected:
    QString getSectionFromCSV(const QString &record, int beg, int end);
    QString prepareTempFile();
    void cleanTempFile();

private:
    QString fileXLS;
};

#endif // XLSREADER_H
