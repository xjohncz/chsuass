#ifndef REPORTCREATOR_H
#define REPORTCREATOR_H

#include <QTextDocument>
#include <QTextCursor>

class reportcreator
{
public:
    reportcreator();
    ~reportcreator();
    QTextDocument *createCardsReport();
    void writeReport(const QString &fileName);

    void clear() { document->clear(); }
    //QTextDocument *getDocument() { return document; }

private:
    QTextDocument *document;
    QTextCursor cursor;
};

#endif // REPORTCREATOR_H
