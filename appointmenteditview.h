#ifndef APPOINTMENTEDITVIEW_H
#define APPOINTMENTEDITVIEW_H

#include <QWidget>

namespace Ui {
class AppointmentEditView;
}

class AppointmentEditView : public QWidget
{
    Q_OBJECT

public:
    explicit AppointmentEditView(QWidget *parent = nullptr);
    ~AppointmentEditView();

private:
    Ui::AppointmentEditView *ui;
};

#endif // APPOINTMENTEDITVIEW_H
