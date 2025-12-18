#include "idatabase.h"
#include<QUuid>

void IDatabase::initDatabase()
{
    database = QSqlDatabase::addDatabase("QSQLITE");
    QString aFile = "E:/qt_code/lab4.db";
    database.setDatabaseName(aFile);

    if (!database.open()) {
        qDebug() << "failed to open database.";
    } else
        qDebug() << "open database is ok";
}


bool IDatabase::initPatientModel()
{
    // 如果模型已经存在且有效，直接刷新数据并返回
    if (patientTabModel != nullptr) {
        if (patientTabModel->database().isOpen()) {
            // 刷新数据但不删除模型
            patientTabModel->select();
            return true;
        }
        // 如果数据库关闭了，需要清理
        if (thePatientSelection) {
            delete thePatientSelection;
            thePatientSelection = nullptr;
        }
        if (patientTabModel) {
            delete patientTabModel;
            patientTabModel = nullptr;
        }
    }

    // 校验数据库是否打开
    if (!database.isOpen()) {
        return false;
    }

    patientTabModel = new QSqlTableModel(this, database);
    patientTabModel->setTable("patient");
    patientTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    int nameField = patientTabModel->fieldIndex("NAME");
    if (nameField >= 0)
        patientTabModel->setSort(nameField, Qt::AscendingOrder);

    if (!patientTabModel->select()) {
        delete patientTabModel;
        patientTabModel = nullptr;
        return false;
    }

    // 每次都创建新的选择模型
    if (thePatientSelection) {
        delete thePatientSelection;
    }
    thePatientSelection = new QItemSelectionModel(patientTabModel);

    return true;
}

int IDatabase::addNewPatient()
{
    if (!patientTabModel) return -1;

    int newRow = patientTabModel->rowCount();

    // 1. 先插入空行
    if (!patientTabModel->insertRow(newRow)) {
        qDebug() << "插入行失败：" << patientTabModel->lastError().text();
        return -1;
    }

    // 2. 生成UUID
    QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
    qDebug() << "新患者 ID:" << uuid;

    // 3. 直接通过setData设置ID字段
    int idColumn = patientTabModel->fieldIndex("ID");
    if (idColumn >= 0) {
        QModelIndex idIndex = patientTabModel->index(newRow, idColumn);
        patientTabModel->setData(idIndex, uuid);
    } else {
        qDebug() << "❌ 找不到ID字段！";
        patientTabModel->removeRow(newRow);
        return -1;
    }

    // 4. 设置其他默认值
    int nameColumn = patientTabModel->fieldIndex("NAME");
    if (nameColumn >= 0) {
        QModelIndex nameIndex = patientTabModel->index(newRow, nameColumn);
        patientTabModel->setData(nameIndex, "新患者");
    }

    // 5. 立即提交这一行到数据库
    if (!patientTabModel->submitAll()) {
        qDebug() << "❌ 提交新患者失败：" << patientTabModel->lastError().text();
        patientTabModel->revertAll();
        return -1;
    }

    // 6. 刷新模型（关键！）
    patientTabModel->select();

    // 7. 验证ID是否已设置
    QSqlRecord rec = patientTabModel->record(newRow);
    QString savedID = rec.value("ID").toString();

    return newRow;
}

bool IDatabase::searchPatient(QString filter)
{
    if (!patientTabModel) return false;
    patientTabModel->setFilter(filter);
    bool ok = patientTabModel->select();
    if (!ok) {
        qDebug() << "患者搜索失败：" << patientTabModel->lastError().text();
    }
    return ok;
}

bool IDatabase::deleteCurrentPatient()
{
    if (!thePatientSelection || !patientTabModel) return false;

    QModelIndex curIndex = thePatientSelection->currentIndex();
    if (!curIndex.isValid()) return false;

    if (!patientTabModel->removeRow(curIndex.row())) {
        qDebug() << "删除患者失败：" << patientTabModel->lastError().text();
        return false;
    }

    bool submitted = patientTabModel->submitAll();
    patientTabModel->select(); // 刷新
    return submitted;
}

bool IDatabase::submitPatientEdit()
{
    if (!patientTabModel) {
        return false;
    }

    bool success = patientTabModel->submitAll();

    // 无论成功与否都刷新
    patientTabModel->select();

    return success;
}

void IDatabase::revertPatientEdit()
{
    if (patientTabModel) {
        patientTabModel->revertAll();
    }
}



QString IDatabase::userLogin(QString userName, QString password)
{
    // return "loginOK";
    QSqlQuery query;
    query.prepare("select username,password from user where userName=:USER");
    query.bindValue(":USER", userName);
    query.exec();
    if (query.first() && query.value("username").isValid()) {
        QString passwd = query.value("password").toString();
        if (passwd == password) {
            qDebug() << "login Ok";
            return "loginOk";
        } else {
            qDebug() << "wrong Password";
            return "wrongPassword";
        }
    } else {
        qDebug() << "no such user";
        return "wrongUsername";
    }

}

IDatabase::IDatabase(QObject *parent) : QObject(parent)
{
    initDatabase();
}


// ========== Department表实现（仅用于读取科室数据，供下拉框选择） ==========
bool IDatabase::initDepartmentModel()
{
    departmentTabModel = new QSqlTableModel(this, database);
    departmentTabModel->setTable("Department"); // 关联Navicat中的Department表
    departmentTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    departmentTabModel->setSort(departmentTabModel->fieldIndex("NAME"), Qt::AscendingOrder);

    if (!departmentTabModel->select()) {
        qDebug() << "科室模型初始化失败：" << departmentTabModel->lastError().text();
        return false;
    }
    theDepartmentSelection = new QItemSelectionModel(departmentTabModel);
    return true;
}

int IDatabase::addNewDepartment()
{
    return 0;    // 无需新增科室（Navicat手动加）
}
bool IDatabase::searchDepartment(QString filter)
{
    return true;    // 无需搜索科室
}
bool IDatabase::deleteCurrentDepartment()
{
    return false;    // 无需删除科室
}
bool IDatabase::submitDepartmentEdit()
{
    return false;    // 无需编辑科室
}
void IDatabase::revertDepartmentEdit() {} // 无需撤销科室修改

// ========== Doctor表实现（核心：医生的增删改查） ==========
bool IDatabase::initDoctorModel()
{
    doctorTabModel = new QSqlTableModel(this, database);
    doctorTabModel->setTable("Doctor"); // 关联Navicat中的Doctor表
    doctorTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    doctorTabModel->setSort(doctorTabModel->fieldIndex("NAME"), Qt::AscendingOrder);

    if (!doctorTabModel->select()) {
        qDebug() << "医生模型初始化失败：" << doctorTabModel->lastError().text();
        return false;
    }
    theDoctorSelection = new QItemSelectionModel(doctorTabModel);
    return true;
}

int IDatabase::addNewDoctor()
{
    // 插入空行
    doctorTabModel->insertRow(doctorTabModel->rowCount());
    int curRecNo = doctorTabModel->rowCount() - 1;

    // 初始化医生ID和工号（EMPLOYEENO）
    QSqlRecord curRec = doctorTabModel->record(curRecNo);
    curRec.setValue("ID", QUuid::createUuid().toString(QUuid::WithoutBraces)); // 唯一ID
    curRec.setValue("EMPLOYEENO", QString("DOC%1").arg(
                        QDateTime::currentDateTime().toTime_t())); // 工号
    doctorTabModel->setRecord(curRecNo, curRec);

    return curRecNo;
}

bool IDatabase::searchDoctor(QString filter)
{
    doctorTabModel->setFilter(filter);
    bool ret = doctorTabModel->select();
    if (!ret) qDebug() << "医生搜索失败：" << doctorTabModel->lastError().text();
    return ret;
}

bool IDatabase::deleteCurrentDoctor()
{
    QModelIndex curIndex = theDoctorSelection->currentIndex();
    if (!curIndex.isValid()) return false;

    doctorTabModel->removeRow(curIndex.row());
    bool ret = doctorTabModel->submitAll();
    doctorTabModel->select(); // 刷新表格
    return ret;
}

bool IDatabase::submitDoctorEdit()
{
    bool ret = doctorTabModel->submitAll();
    if (!ret) qDebug() << "医生信息保存失败：" << doctorTabModel->lastError().text();
    return ret;
}

void IDatabase::revertDoctorEdit()
{
    doctorTabModel->revertAll();
}


// ========== Medicine 表实现 ==========

bool IDatabase::initMedicineModel()

{
    medicineTabModel = new QSqlTableModel(this, database);
    medicineTabModel->setTable("Medicine");
    medicineTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    medicineTabModel->setSort(medicineTabModel->fieldIndex("NAME"), Qt::AscendingOrder);

    if (!medicineTabModel->select()) {
        qDebug() << "药品模型初始化失败：" << medicineTabModel->lastError().text();
        return false;

    }
    theMedicineSelection = new QItemSelectionModel(medicineTabModel);
    return true;

}


int IDatabase::addNewMedicine()
{
    // 1. 先校验模型是否初始化
    if (!medicineTabModel) {
        qDebug() << "药品模型未初始化！";
        return -1;
    }

    // 2. 插入新行（安全写法）
    int newRow = medicineTabModel->rowCount();
    if (!medicineTabModel->insertRow(newRow)) {
        qDebug() << "插入新行失败：" << medicineTabModel->lastError().text();
        return -1;
    }

    // 3. 初始化字段（只初始化存在的字段，避免字段名错误）
    QSqlRecord rec = medicineTabModel->record(newRow);
    // 仅初始化数据库中存在的字段
    rec.setValue("ID", QUuid::createUuid().toString(QUuid::WithoutBraces));
    rec.setValue("STOCK", 0);
    rec.setValue("STOCK_STATUS", "缺货");

    // 4. 设置默认值（兼容空字段）
    rec.setValue("UNIT", "盒");
    rec.setValue("PRICE", 0.00);
    rec.setValue("EXPIRY_DATE", QDate::currentDate().addYears(1).toString("yyyy-MM-dd"));

    // 5. 提交记录
    if (!medicineTabModel->setRecord(newRow, rec)) {
        qDebug() << "设置记录失败：" << medicineTabModel->lastError().text();
        medicineTabModel->removeRow(newRow); // 回滚
        return -1;
    }
    return newRow;
}



bool IDatabase::searchMedicine(QString filter)
{
    medicineTabModel->setFilter(filter);
    return medicineTabModel->select();
}


bool IDatabase::deleteCurrentMedicine()
{
    QModelIndex curIndex = theMedicineSelection->currentIndex();
    if (!curIndex.isValid()) return false;
    medicineTabModel->removeRow(curIndex.row());
    bool ret = medicineTabModel->submitAll();
    medicineTabModel->select();
    return ret;

}


bool IDatabase::submitMedicineEdit()
{
    return medicineTabModel->submitAll();
}

void IDatabase::revertMedicineEdit()
{
    medicineTabModel->revertAll();
}

