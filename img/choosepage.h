#ifndef CHOOSEPAGE_H
#define CHOOSEPAGE_H

#include <QDialog>

namespace Ui {
class ChoosePage;
}

class ChoosePage : public QDialog
{
    Q_OBJECT

public:
    explicit ChoosePage(QWidget *parent = nullptr);
    ~ChoosePage();

private:
    Ui::ChoosePage *ui;
};

#endif // CHOOSEPAGE_H
