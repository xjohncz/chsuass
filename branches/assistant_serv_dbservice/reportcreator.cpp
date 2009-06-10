#include "reportcreator.h"

#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QDir>

reportcreator::reportcreator()
{
}

reportcreator::~reportcreator() {
}

QString reportcreator::createCardReport(const QMap<int, QString> &map) {

    QString templatePath = QDir::currentPath() + "/templates/card/";

    QTextCodec *cp1251_codec = QTextCodec::codecForName("cp1251");

    QFile file;
    QTextStream textStream(&file);
    textStream.setCodec(cp1251_codec);
    QString resultReport;

    file.setFileName(templatePath + "card_begin.htm");
    file.open(QIODevice::ReadOnly);
    resultReport = textStream.readAll();
    file.close();

    QString cardHeader;
    QString cardCont;
    QString cardFooter;

    file.setFileName(templatePath + "card_header.htm");
    file.open(QIODevice::ReadOnly);
    cardHeader = textStream.readAll();
    file.close();

    file.setFileName(templatePath + "card_cont.htm");
    file.open(QIODevice::ReadOnly);
    cardCont = textStream.readAll();
    file.close();

    file.setFileName(templatePath + "card_footer.htm");
    file.open(QIODevice::ReadOnly);
    cardFooter = textStream.readAll();
    file.close();

    QMapIterator<int, QString> i(map);
    int count = 1;
    while(i.hasNext()) {
        i.next();

        QString head = cardHeader;
        head.replace(QString("%cnum%"), QString::number(i.key()));

        QString cont = cardCont;
        QString questions = i.value();
        questions.replace(QString("\n"), QString("<br>"));

        cont.replace(QString("%card_contents%"), questions);

        resultReport = resultReport + head + cont + cardFooter;

        if(count % 2 != 0)
            resultReport = resultReport + "\n<hr>\n";

        count++;
    }

    file.setFileName(templatePath + "card_end.htm");
    file.open(QIODevice::ReadOnly);
    resultReport = resultReport + textStream.readAll();
    file.close();

    return resultReport;

}

void reportcreator::writeReport(const QString &fileName, const QString &report) {

    QTextCodec *cp1251_codec = QTextCodec::codecForName("cp1251");
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    QTextStream textStream(&file);
    textStream.setCodec(cp1251_codec);

    textStream << report;
    textStream.flush();

    file.close();

    QString logo = QDir::currentPath() + "/templates/chsu_logo.jpg";
    QString path = fileName;
    path.truncate(path.lastIndexOf("/"));

    QFile::copy(logo, path + "/chsu_logo.jpg");

}