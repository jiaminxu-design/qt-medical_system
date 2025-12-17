#include "doctoreditview.h"
#include "ui_doctoreditview.h"

doctoreditview::doctoreditview(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::doctoreditview)
{
    ui->setupUi(this);
}

doctoreditview::~doctoreditview()
{
    delete ui;
}
