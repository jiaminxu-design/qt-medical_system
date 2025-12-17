#ifndef APPOINTMENTVIEW_H
#define APPOINTMENTVIEW_H

#include <QWidget>

namespace Ui {
class AppointmentView;
}

class AppointmentView : public QWidget
{
    Q_OBJECT

public:
    explicit AppointmentView(QWidget *parent = nullptr);
    ~AppointmentView();

private:
    Ui::AppointmentView *ui;
};

#endif // APPOINTMENTVIEW_H
