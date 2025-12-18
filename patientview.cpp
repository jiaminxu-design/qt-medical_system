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

    // 如果模型不存在，才初始化
    if (db.patientTabModel == nullptr) {
        if (!db.initPatientModel()) {
            qDebug() << "PatientView: 模型初始化失败";
            return;
        }
    } else {
        // 模型已存在，刷新数据
        db.patientTabModel->select();
    }

    // 设置模型到表格视图
    ui->tableView->setModel(db.patientTabModel);

    // 重要：每次都创建新的选择模型给这个视图
    if (db.thePatientSelection) {
        delete db.thePatientSelection;
    }
    db.thePatientSelection = new QItemSelectionModel(db.patientTabModel);
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

