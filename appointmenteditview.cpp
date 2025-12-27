#include "appointmenteditview.h"
#include "ui_appointmenteditview.h"

AppointmentEditView::AppointmentEditView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppointmentEditView)
{
    ui->setupUi(this);
}

AppointmentEditView::~AppointmentEditView()
{
    delete ui;
}
