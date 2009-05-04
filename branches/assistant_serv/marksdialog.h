#ifndef MARKSDIALOG_H
#define MARKSDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class marksdialog;
}

class marksdialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(marksdialog)
public:
    explicit marksdialog(QWidget *parent = 0);
    virtual ~marksdialog();

    void setId(int id) { currentId = id; }
    int getId() { return currentId; }

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::marksdialog *m_ui;

    int currentId;
};

#endif // MARKSDIALOG_H
