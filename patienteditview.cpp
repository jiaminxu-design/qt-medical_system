#include "patienteditview.h"
#include "ui_patienteditview.h"
#include "idatabase.h"
#include <QSqlTableModel>
#include <QDebug>
#include <QMessageBox>
#include <QUuid>

PatientEditView::PatientEditView(QWidget *parent, int index) :
    QWidget(parent),
    ui(new Ui::PatientEditView),
    dataMapper(nullptr)
{
    ui->setupUi(this);

    // 获取模型
    QSqlTableModel *tabModel = IDatabase::getInstance().patientTabModel;
    if (!tabModel) {
        return;
    }

    // 验证行号是否有效
    if (index < 0 || index >= tabModel->rowCount()) {
        return;
    }

    // 检查该行的ID是否为空
    QModelIndex idIndex = tabModel->index(index, tabModel->fieldIndex("ID"));
    QString currentID = tabModel->data(idIndex).toString();

    if (currentID.isEmpty()) {
        // 如果是新行且ID为空，立即设置ID
        QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
        tabModel->setData(idIndex, uuid);
        tabModel->submitAll();
        tabModel->select(); // 重新加载数据
    }

    // 创建数据映射器
    dataMapper = new QDataWidgetMapper(this);
    dataMapper->setModel(tabModel);
    dataMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit); // 改为手动提交

    // 添加字段映射
    dataMapper->addMapping(ui->dbEditID, tabModel->fieldIndex("ID"));
    dataMapper->addMapping(ui->dbEditName, tabModel->fieldIndex("NAME"));
    dataMapper->addMapping(ui->dbEditIDCard, tabModel->fieldIndex("ID_CARD"));
    dataMapper->addMapping(ui->dbSpinHeight, tabModel->fieldIndex("HEIGHT"));
    dataMapper->addMapping(ui->dbSpinWeight, tabModel->fieldIndex("WEIGHT"));
    dataMapper->addMapping(ui->dbEditMobile, tabModel->fieldIndex("MOBILEPHOME"));
    dataMapper->addMapping(ui->dbDateEditDOB, tabModel->fieldIndex("DOB"));
    dataMapper->addMapping(ui->dbComboSex, tabModel->fieldIndex("SEX"));
    dataMapper->addMapping(ui->dbCreatedTimeStamp, tabModel->fieldIndex("CREATEDTIMESTAMP"));

    // 设置当前索引
    dataMapper->setCurrentIndex(index);
    ui->dbEditID->setEnabled(false);
}

PatientEditView::~PatientEditView()
{
    delete dataMapper;
    delete ui;
}

void PatientEditView::on_pushButton_clicked()  // 保存按钮
{
    // 1. 先通过dataMapper提交到模型
    if (dataMapper) {
        dataMapper->submit();
    }
    // 2. 获取当前行的ID
    QSqlTableModel *tabModel = IDatabase::getInstance().patientTabModel;
    if (!tabModel) {
        QMessageBox::warning(this, "错误", "数据模型未初始化！");
        return;
    }
    int currentRow = dataMapper ? dataMapper->currentIndex() : 0;
    QModelIndex idIndex = tabModel->index(currentRow, tabModel->fieldIndex("ID"));
    QString patientID = tabModel->data(idIndex).toString();
    if (patientID.isEmpty()) {
        QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
        tabModel->setData(idIndex, uuid);
        tabModel->submitAll(); // 立即提交ID到数据库
    }
    // 3. 验证所有必填字段
    bool hasEmptyField = false;
    QStringList emptyFields;
    QSqlRecord rec = tabModel->record(currentRow);
    for (int i = 0; i < rec.count(); i++) {
        QString fieldName = rec.fieldName(i);
        QVariant value = rec.value(i);
        if (fieldName == "ID" && value.toString().isEmpty()) {
            hasEmptyField = true;
            emptyFields << "ID";
        }
        if (fieldName == "NAME" && value.toString().isEmpty()) {
            hasEmptyField = true;
            emptyFields << "姓名";
        }
    }
    if (hasEmptyField) {
        QMessageBox::warning(this, "保存失败",

                             "以下必填字段不能为空：\n" + emptyFields.join("、"));
        return;
    }
    // 4. 确保姓名字段有值
    QModelIndex nameIndex = tabModel->index(currentRow, tabModel->fieldIndex("NAME"));
    if (tabModel->data(nameIndex).toString().isEmpty()) {
        tabModel->setData(nameIndex, "新患者");
    }
    // 5. 提交到数据库
    bool success = IDatabase::getInstance().submitPatientEdit();
    if (success) {
        QMessageBox::information(this, "成功", "患者信息保存成功！");
        emit goPreviousView();
    } else {
        QMessageBox::warning(this, "保存失败",

                             "保存失败：\n" + tabModel->lastError().text());
    }
}

void PatientEditView::on_pushButton_2_clicked()  // 取消按钮
{

    // 回滚数据
    IDatabase::getInstance().revertPatientEdit();

    // 刷新模型
    QSqlTableModel *tabModel = IDatabase::getInstance().patientTabModel;
    if (tabModel) {
        tabModel->select();
    }
    emit goPreviousView();
}
