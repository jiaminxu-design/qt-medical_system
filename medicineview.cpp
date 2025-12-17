#include "medicineview.h"
#include "ui_medicineview.h"

MedicineView::MedicineView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MedicineView)
{
    ui->setupUi(this);
}

MedicineView::~MedicineView()
{
    delete ui;
}
