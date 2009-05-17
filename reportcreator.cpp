#include "reportcreator.h"

#include <QTextDocumentWriter>
#include <QTextDocumentFragment>
#include <QFile>
#include <QTextStream>
//#include <QImage>
//#include <QVector>

reportcreator::reportcreator()
        : document(new QTextDocument()),
        cursor(document)
{
}

reportcreator::~reportcreator() {

    delete document;

}

QTextDocument *reportcreator::createCardsReport() {

    QImage logo(":/images/chsu_logo.png");

    QTextTableFormat tableFormat;
    QVector<QTextLength> tableWidths;
    tableWidths << QTextLength(QTextLength::PercentageLength, 10)
            << QTextLength(QTextLength::PercentageLength, 90);
    tableFormat.setColumnWidthConstraints(tableWidths);
    cursor.insertTable(1, 2, tableFormat);

    QTextBlockFormat blockFormat;
    blockFormat.setAlignment(Qt::AlignHCenter);

    cursor.setBlockFormat(blockFormat);
    cursor.insertImage(logo);

    cursor.movePosition(QTextCursor::NextCell);
    cursor.insertText(QObject::tr("МИНИСТЕРСТВО ОБРАЗОВАНИЯ И НАУКИ РОССИЙСКОЙ ФЕДЕРАЦИИ"));
    cursor.insertBlock();
    cursor.insertText(QObject::tr("ФЕДЕРАЛЬНОЕ АГЕНСТВО ПО ОБРАЗОВАНИЮ"));
    cursor.insertBlock();
    cursor.insertText(QObject::tr("Государственное образовательное учреждение"));
    cursor.insertBlock();
    cursor.insertText(QObject::tr("высшего профессионального образования"));
    cursor.insertBlock();
    cursor.insertText(QObject::tr("\"ЧЕРЕПОВЕЦКИЙ ГОСУДАРСТВЕННЫЙ УНИВЕРСИТЕТ\""));

    cursor.movePosition(QTextCursor::NextBlock);
    QTextFrameFormat frameFormat;
    frameFormat.setBorder(1);
    frameFormat.setHeight(150);
    cursor.insertFrame(frameFormat);
    cursor.movePosition(QTextCursor::NextBlock);
    cursor.insertText("test");

//    QFile cardTemplate("templates/card.htm");
//    cardTemplate.open(QIODevice::ReadOnly);
//
//    QTextStream textStream(&cardTemplate);
//    QString htmlCard = textStream.readAll();
//
//    cardTemplate.close();
//
//    QTextDocumentFragment docFragment = QTextDocumentFragment::fromHtml(htmlCard);
//
//    cursor.insertFragment(docFragment);

    return document;

}

void reportcreator::writeReport(const QString &fileName) {

    QTextDocumentWriter writer(fileName);
    writer.setFormat("html");
    writer.write(document);

}
