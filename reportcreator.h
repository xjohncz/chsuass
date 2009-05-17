#ifndef REPORTCREATOR_H
#define REPORTCREATOR_H

#include <QMap>

class reportcreator
{
public:
    reportcreator();
    ~reportcreator();
    static QString createCardReport(const QMap<int, QString> &map);
    static void writeReport(const QString &fileName, const QString &report);

};

#endif // REPORTCREATOR_H
