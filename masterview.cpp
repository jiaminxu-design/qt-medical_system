#include "masterview.h"
#include "ui_masterview.h"
#include<QDebug>
#include"idatabase.h"
#include<QMessageBox>

MasterView::MasterView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MasterView)
{
    ui->setupUi(this);

    this->setWindowFlag(Qt::FramelessWindowHint);

    goLoginView();

    IDatabase::getInstance();
}

MasterView::~MasterView()
{
    delete ui;
}

void MasterView::goLoginView()
{
    qDebug() << "goLoginView";
    loginView = new LoginView(this);
    pushWidgetToStackView(loginView);

    connect(loginView, SIGNAL(loginSuccess()), this, SLOT(goWelcomeView()));
}

void MasterView::goWelcomeView()
{
    qDebug() << "goWelcomeView";
    welcomeView = new WelcomeView(this);
    pushWidgetToStackView(welcomeView);

//    connect(welcomeView, SIGNAL(goDoctorView()), this, SLOT(goDoctorView()));
//    connect(welcomeView, SIGNAL(goPatientView()), this, SLOT(goPatientView()));
//    connect(welcomeView, SIGNAL(goRecordView()), this, SLOT(goRecordView()));
//    connect(welcomeView, SIGNAL(goMedicineView()), this, SLOT(goMedicineView()));
//    connect(welcomeView, SIGNAL(goAppointmentView()), this, SLOT(goAppointmentView()));
    // 使用Qt::UniqueConnection确保信号只连接一次
    connect(welcomeView, SIGNAL(goDoctorView()),
            this, SLOT(goDoctorView()), Qt::UniqueConnection);
    connect(welcomeView, SIGNAL(goPatientView()),
            this, SLOT(goPatientView()), Qt::UniqueConnection);
    connect(welcomeView, SIGNAL(goRecordView()),
            this, SLOT(goRecordView()), Qt::UniqueConnection);
    connect(welcomeView, SIGNAL(goMedicineView()),
            this, SLOT(goMedicineView()), Qt::UniqueConnection);
    connect(welcomeView, SIGNAL(goAppointmentView()),
            this, SLOT(goAppointmentView()), Qt::UniqueConnection);

}



void MasterView::goDoctorView()
{
    qDebug() << "goDoctorView";
    doctorView = new DoctorView(this);
    pushWidgetToStackView(doctorView);

    // 关联“跳转到编辑界面”信号
    connect(doctorView, SIGNAL(goDoctorEditView(int)), this, SLOT(goDoctorEditView(int)));
}



void MasterView::goDoctorEditView(int rowNo)
{
    qDebug() << "goDoctorEditView";
    doctorEditView = new DoctorEditView(this, rowNo);
    pushWidgetToStackView(doctorEditView);

    // 关联“返回上一级”信号
    connect(doctorEditView, SIGNAL(goPreviousView()), this, SLOT(goPreviousView()));
}




void MasterView::goPatientEditView(int rowNo)
{
    qDebug() << "goPatientEditView";
    patientEditView = new PatientEditView(this, rowNo);
    pushWidgetToStackView(patientEditView);
    connect(patientEditView, SIGNAL(goPreviousView()), this, SLOT(goPreviousView()));


}

//void MasterView::goPatientView()
//{
//    // 如果patientView已存在，先检查是否需要销毁
//    if (patientView != nullptr) {
//        // 先从stackedWidget中移除
//        int index = ui->stackedWidget->indexOf(patientView);
//        if (index >= 0) {
//            ui->stackedWidget->removeWidget(patientView);
//        }
//        // 安全删除
//        delete patientView;
//        patientView = nullptr;
//    }

//    // 创建新的PatientView
//    patientView = new PatientView(this);

//    // 重新连接信号
//    connect(patientView, SIGNAL(goPatientEditView(int)),
//            this, SLOT(goPatientEditView(int)));

//    // 添加到stackedWidget
//    pushWidgetToStackView(patientView);
//}

//void MasterView::goPatientView()
//{
//    qDebug() << "goPatientView";

//    // 如果页面已存在，直接显示；否则新建
//    if (patientView == nullptr) {
//        patientView = new PatientView(this);
//        connect(patientView, SIGNAL(goPatientEditView(int)),
//                this, SLOT(goPatientEditView(int)));
//    } else {
//        // 刷新现有视图的数据
//        if (IDatabase::getInstance().patientTabModel) {
//            IDatabase::getInstance().patientTabModel->select();
//        }
//    }

//    pushWidgetToStackView(patientView);
//}

void MasterView::goPatientView()
{
    qDebug() << "goPatientView";

    // 如果patientView已存在，先清理
    if (patientView != nullptr) {
        // 从stackedWidget中移除
        int index = ui->stackedWidget->indexOf(patientView);
        if (index >= 0) {
            ui->stackedWidget->removeWidget(patientView);
        }
        // 断开所有连接
        patientView->disconnect();
        delete patientView;
        patientView = nullptr;
    }

    // 总是创建新的PatientView
    patientView = new PatientView(this);

    // 连接信号
    connect(patientView, SIGNAL(goPatientEditView(int)),
            this, SLOT(goPatientEditView(int)));

    pushWidgetToStackView(patientView);
}

//void MasterView::goPreviousView()
//{
//    int count = ui->stackedWidget->count();
//    if (count > 1) {
//        ui->stackedWidget->setCurrentIndex((count - 2));
//        ui->labelTitle->setText(ui->stackedWidget->currentWidget()->windowTitle());

//        QWidget *widget = ui->stackedWidget->widget(count - 1);
//        ui->stackedWidget->removeWidget(widget);
//        delete widget;
//    }
//}

void MasterView::goPreviousView()
{
    int count = ui->stackedWidget->count();
    if (count > 1) {
        // 获取要删除的widget
        QWidget *widget = ui->stackedWidget->widget(count - 1);

        // 检查widget类型并清理对应的指针
        if (widget == loginView) {
            loginView = nullptr;
        } else if (widget == welcomeView) {
            welcomeView = nullptr;
        } else if (widget == patientView) {
            patientView = nullptr;
        } else if (widget == doctorView) {
            doctorView = nullptr;
        } else if (widget == recordView) {
            recordView = nullptr;
        } else if (widget == medicineView) {
            medicineView = nullptr;
        } else if (widget == appointmentView) {
            appointmentView = nullptr;
        } else if (widget == patientEditView) {
            patientEditView = nullptr;
        } else if (widget == doctorEditView) {
            doctorEditView = nullptr;
        } else if (widget == medicineEditView) {
            medicineEditView = nullptr;
        }

        // 切换视图
        ui->stackedWidget->setCurrentIndex(count - 2);
        ui->labelTitle->setText(ui->stackedWidget->currentWidget()->windowTitle());

        // 移除并删除widget
        ui->stackedWidget->removeWidget(widget);
        delete widget;
    }
}



//void MasterView::goRecordView()
//{
//    qDebug() << "goRecordView";
//    recordView = new RecordView(this);
//    pushWidgetToStackView(recordView);
//}


// 修改 goRecordView 绑定跳转信号
void MasterView::goRecordView()
{
    qDebug() << "goRecordView";
    recordView = new RecordView(this);
    pushWidgetToStackView(recordView);
    // 绑定跳转到编辑页面的信号（和药品模块一致）
    connect(recordView, SIGNAL(goRecordEditView(int)), this, SLOT(goRecordEditView(int)));
}

// 完善 goRecordEditView 方法
//void MasterView::goRecordEditView(int rowNo)
//{
//    qDebug() << "goRecordEditView";
//    recordEditView = new RecordEditView(this,
//                                        rowNo); // 注意变量名统一（建议改为recordEditView）
//    pushWidgetToStackView(recordEditView);
//    connect(recordEditView, SIGNAL(goPreviousView()), this, SLOT(goPreviousView()));
//}

void MasterView::goRecordEditView(int rowNo)
{
    qDebug() << "goRecordEditView";
    // 确保创建的是独立页面，且父对象是MasterView（不是RecordView）
    RecordEditView *editView = new RecordEditView(this, rowNo);
    pushWidgetToStackView(editView);
    connect(editView, &RecordEditView::goPreviousView, this, &MasterView::goPreviousView);
}



void MasterView::goMedicineView()
{
    qDebug() << "goMedicineView";
    medicineView = new MedicineView(this);
    pushWidgetToStackView(medicineView);

    // 关联“跳转到药品编辑页面”的信号
    connect(medicineView, &MedicineView::goMedicineEditView, this, &MasterView::goMedicineEditView);
}


void MasterView::goMedicineEditView(int rowNo)
{
    qDebug() << "goMedicineEditView";
    medicineEditView = new MedicineEditView(this, rowNo);
    pushWidgetToStackView(medicineEditView);
    connect(medicineEditView, SIGNAL(goPreviousView()), this, SLOT(goPreviousView()));


}

void MasterView::goAppointmentView()
{
    qDebug() << "goAppointmentView";
    appointmentView = new AppointmentView(this);
    pushWidgetToStackView(appointmentView);
}

void MasterView::pushWidgetToStackView(QWidget *widget)
{
    ui->stackedWidget->addWidget(widget);
    int count = ui->stackedWidget->count();
    ui->stackedWidget->setCurrentIndex(count - 1);
    ui->labelTitle->setText(widget->windowTitle());
}


void MasterView::on_btBack_clicked()
{
    goPreviousView();
}


void MasterView::on_stackedWidget_currentChanged(int arg1)
{
    int count = ui->stackedWidget->count();
    if (count > 1)
        ui->btBack->setEnabled(true);
    else
        ui->btBack->setEnabled(false);

    QString title = ui->stackedWidget->currentWidget()->windowTitle();

    if (title == "欢迎") {
        ui->btLogout->setEnabled(true);
        ui->btBack->setEnabled(false);

    } else {
        ui->btLogout->setEnabled(false);
    }
}


void MasterView::on_btLogout_clicked()
{
    goPreviousView();
}

