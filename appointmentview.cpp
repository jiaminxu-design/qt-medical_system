#include "appointmentview.h"
#include "ui_appointmentview.h"

AppointmentView::AppointmentView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppointmentView)
{
    ui->setupUi(this);
}

AppointmentView::~AppointmentView()
{
    delete ui;
}
