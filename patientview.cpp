#include "patientview.h"
#include "ui_patientview.h"
#include"idatabase.h"
#include<QMessageBox>

PatientView::PatientView(QWidget *parent, int index) :
    QWidget(parent),
    ui(new Ui::PatientView)
{
    ui->setupUi(this);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setAlternatingRowColors(true);

    IDatabase &db = IDatabase::getInstance();

    // 永远尝试初始化模型，initPatientModel()会处理重复初始化
    if (!db.initPatientModel()) {
        // 可以显示错误信息，但不退出
        qDebug() << "PatientView: 模型初始化失败";
        return;
    }

    // 设置模型到表格视图
    ui->tableView->setModel(db.patientTabModel);
    ui->tableView->setSelectionModel(db.thePatientSelection);
}


PatientView::~PatientView()
{
    delete ui;
}

void PatientView::on_btAdd_clicked()
{
    int currow = IDatabase::getInstance().addNewPatient();
    emit goPatientEditView(currow);
}


void PatientView::on_btSearch_clicked()
{
    QString filter = QString("name like '%%1%'").arg(ui->txtSearch->text());
    IDatabase::getInstance().searchPatient(filter);
}



void PatientView::on_btEdit_clicked()
{
    QModelIndex curIndex = IDatabase::getInstance().thePatientSelection->currentIndex();
    emit goPatientEditView(curIndex.row());
}

void PatientView::on_btDelete_clicked()
{
    IDatabase::getInstance().deleteCurrentPatient();
}

