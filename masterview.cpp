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
        } else if (widget == recordEditView) {
            recordEditView = nullptr;
        }

        // 切换视图
        ui->stackedWidget->setCurrentIndex(count - 2);
        ui->labelTitle->setText(ui->stackedWidget->currentWidget()->windowTitle());

        // 移除并删除widget
        ui->stackedWidget->removeWidget(widget);
        delete widget;
    }
}

void MasterView::goRecordView()
{
    qDebug() << "goRecordView";
    RecordView *recordView = new RecordView(this);
    pushWidgetToStackView(recordView);
    // 绑定“跳转到编辑页”的信号（由RecordView发送，MasterView接收）
    connect(recordView, &RecordView::goRecordEditView, this, &MasterView::goRecordEditView);
}

void MasterView::goRecordEditView(int row)
{
    qDebug() << "goRecordEditView";
    RecordEditView *editView = new RecordEditView(this, row);
    pushWidgetToStackView(editView);
    // 绑定“返回上一页”的信号
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
void MasterView::goAppointmentEditView(int rowNo)
{
    qDebug() << "goAppointmentEditView";

    // 1. 确保预约列表页已创建并在栈中
    if (!appointmentView) {
        appointmentView = new AppointmentView(this);
        // 提前初始化列表页，避免空指针
        appointmentView->initUI();
    }
    int appointmentIndex = ui->stackedWidget->indexOf(appointmentView);
    if (appointmentIndex == -1) {
        pushWidgetToStackView(appointmentView);
    }

    // 2. 创建编辑页并压入栈
    AppointmentEditView *editView = new AppointmentEditView(this, rowNo);
    pushWidgetToStackView(editView);

    // 3. 绑定保存成功信号（强制用Qt5新语法，避免槽函数识别失败）
    bool connectOk = connect(editView, SIGNAL(appointmentSaved()),
                             appointmentView, SLOT(refreshTable()));
    if (!connectOk) {
        qDebug() << "保存信号绑定失败！检查refreshTable是否为槽函数";
    }

    // 4. 绑定返回信号
    connect(editView, &AppointmentEditView::goPreviousView, this, [this, editView]() {
        // 清理编辑页
        int editIdx = ui->stackedWidget->indexOf(editView);
        if (editIdx >= 0) {
            ui->stackedWidget->removeWidget(editView);
            editView->deleteLater();
        }
        // 切回列表页
        int listIdx = ui->stackedWidget->indexOf(appointmentView);
        if (listIdx >= 0) {
            ui->stackedWidget->setCurrentIndex(listIdx);
            ui->labelTitle->setText("预约排班管理");
        }
    });
}


//绑定编辑页跳转信号
void MasterView::goAppointmentView()
{
    qDebug() << "goAppointmentView";
    appointmentView = new AppointmentView(this);
    pushWidgetToStackView(appointmentView);

    // 绑定“跳转到编辑页”的信号
    connect(appointmentView, SIGNAL(goAppointmentEditView(int)), this,
            SLOT(goAppointmentEditView(int)));

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

