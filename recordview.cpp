#include "recordview.h"
#include "ui_recordview.h"

RecordView::RecordView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecordView)
{
    ui->setupUi(this);
}

RecordView::~RecordView()
{
    delete ui;
}
