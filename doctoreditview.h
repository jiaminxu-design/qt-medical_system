#ifndef DOCTOREDITVIEW_H
#define DOCTOREDITVIEW_H

#include <QWidget>

namespace Ui {
class doctoreditview;
}

class doctoreditview : public QWidget
{
    Q_OBJECT

public:
    explicit doctoreditview(QWidget *parent = nullptr);
    ~doctoreditview();

private:
    Ui::doctoreditview *ui;
};

#endif // DOCTOREDITVIEW_H
