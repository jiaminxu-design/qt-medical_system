//#ifndef IDATABASE_H
//#define IDATABASE_H

//#include <QObject>
//#include<QtSql>
//#include<QSqlDatabase>
//#include<QDataWidgetMapper>

//class IDatabase : public QObject
//{
//    Q_OBJECT
//public:

////    static IDatabase &getInstance()
////    {
////        static IDatabase instance;
////        return instance;
////    }
//    static IDatabase &getInstance();
//    bool submitAppointmentEdit()
//    {
//        return appointmentTabModel->submitAll();
//    }
//    QSqlTableModel *appointmentTabModel = nullptr; // 预约表模型

//    QString userLogin(QString userName, QString password);
//    QSqlQuery querySql(const QString &sql);

//private:

//    explicit IDatabase(QObject *parent = nullptr);
//    IDatabase(IDatabase const &) = delete;
//    void operator=(IDatabase const &)  = delete;

//    QSqlDatabase database;

//    void initDatabase();


//signals:

//public:
//    // ---------- Patient表 ----------
//    bool initPatientModel();
//    int addNewPatient();
//    bool searchPatient(QString filter);
//    bool deleteCurrentPatient();
//    bool submitPatientEdit();
//    void revertPatientEdit();

//    QSqlTableModel *patientTabModel;
//    QItemSelectionModel *thePatientSelection;


//    // ---------- Department表 ----------
//    bool initDepartmentModel();
//    int addNewDepartment();
//    bool searchDepartment(QString filter);
//    bool deleteCurrentDepartment();
//    bool submitDepartmentEdit();
//    void revertDepartmentEdit();
//    QSqlTableModel *departmentTabModel;
//    QItemSelectionModel *theDepartmentSelection;

//    // ---------- Doctor表 ----------
//    bool initDoctorModel();
//    int addNewDoctor();
//    bool searchDoctor(QString filter);
//    bool deleteCurrentDoctor();
//    bool submitDoctorEdit();
//    void revertDoctorEdit();
//    QSqlTableModel *doctorTabModel;
//    QItemSelectionModel *theDoctorSelection;

//    // ---------- Medicine表 ----------

//    bool initMedicineModel();
//    int addNewMedicine();
//    bool searchMedicine(QString filter);
//    bool deleteCurrentMedicine();
//    bool submitMedicineEdit();
//    void revertMedicineEdit();
//    QSqlTableModel *medicineTabModel = nullptr;
//    QItemSelectionModel *theMedicineSelection = nullptr;

//    // ---------- Record表（就诊记录） ----------
//    bool initRecordModel();          // 初始化就诊记录模型
//    int addNewRecord();              // 新增就诊记录
//    bool searchRecord(QString filter);// 搜索就诊记录
//    bool deleteCurrentRecord();      // 删除当前就诊记录
//    bool submitRecordEdit();         // 提交就诊记录修改
//    void revertRecordEdit();         // 撤销就诊记录修改
//    QSqlTableModel *recordTabModel = nullptr;      // 就诊记录模型
//    QItemSelectionModel *theRecordSelection = nullptr; // 就诊记录选择模型

//    // ========== 预约表（Appointment）核心操作 ==========
//    bool initAppointmentModel();

//    QItemSelectionModel *theAppointmentSelection = nullptr;
//    bool submitAppointmentEdit()
//    {
//        return appointmentTabModel ? appointmentTabModel->submitAll() : false;
//    }
//    void revertAppointmentEdit()
//    {
//        if (appointmentTabModel) appointmentTabModel->revertAll();
//    }
//    void searchAppointment(const QString &filter)
//    {
//        if (appointmentTabModel) appointmentTabModel->setFilter(filter);
//    }
//    bool deleteCurrentAppointment();
//    int addNewAppointment();

//};

//#endif // IDATABASE_H

#ifndef IDATABASE_H
#define IDATABASE_H

#include <QObject>
#include <QtSql>
#include <QSqlDatabase>
#include <QDataWidgetMapper>

class IDatabase : public QObject
{
    Q_OBJECT
public:
    // 单例模式获取实例
    static IDatabase &getInstance();

    // 预约表核心操作（仅保留一份，增加空指针保护）
    QSqlTableModel *appointmentTabModel = nullptr; // 预约表模型
    bool submitAppointmentEdit()
    {
        return appointmentTabModel ? appointmentTabModel->submitAll() : false;
    }

    // 通用方法
    QString userLogin(QString userName, QString password);
    QSqlQuery querySql(const QString &sql);

private:
    // 单例模式：私有构造+禁用拷贝
    explicit IDatabase(QObject *parent = nullptr);
    IDatabase(IDatabase const &) = delete;
    void operator=(IDatabase const &) = delete;

    // 数据库核心对象
    QSqlDatabase database;
    void initDatabase(); // 初始化数据库连接

signals:

public:
    // ---------- Patient表（患者） ----------
    bool initPatientModel();
    int addNewPatient();
    bool searchPatient(QString filter);
    bool deleteCurrentPatient();
    bool submitPatientEdit();
    void revertPatientEdit();
    QSqlTableModel *patientTabModel = nullptr;
    QItemSelectionModel *thePatientSelection = nullptr;

    // ---------- Department表（科室） ----------
    bool initDepartmentModel();
    int addNewDepartment();
    bool searchDepartment(QString filter);
    bool deleteCurrentDepartment();
    bool submitDepartmentEdit();
    void revertDepartmentEdit();
    QSqlTableModel *departmentTabModel = nullptr;
    QItemSelectionModel *theDepartmentSelection = nullptr;

    // ---------- Doctor表（医生） ----------
    bool initDoctorModel();
    int addNewDoctor();
    bool searchDoctor(QString filter);
    bool deleteCurrentDoctor();
    bool submitDoctorEdit();
    void revertDoctorEdit();
    QSqlTableModel *doctorTabModel = nullptr;
    QItemSelectionModel *theDoctorSelection = nullptr;

    // ---------- Medicine表（药品） ----------
    bool initMedicineModel();
    int addNewMedicine();
    bool searchMedicine(QString filter);
    bool deleteCurrentMedicine();
    bool submitMedicineEdit();
    void revertMedicineEdit();
    QSqlTableModel *medicineTabModel = nullptr;
    QItemSelectionModel *theMedicineSelection = nullptr;

    // ---------- Record表（就诊记录） ----------
    bool initRecordModel();          // 初始化就诊记录模型
    int addNewRecord();              // 新增就诊记录
    bool searchRecord(QString filter);// 搜索就诊记录
    bool deleteCurrentRecord();      // 删除当前就诊记录
    bool submitRecordEdit();         // 提交就诊记录修改
    void revertRecordEdit();         // 撤销就诊记录修改
    QSqlTableModel *recordTabModel = nullptr;      // 就诊记录模型
    QItemSelectionModel *theRecordSelection = nullptr; // 就诊记录选择模型

    // ========== 预约表（Appointment）扩展操作 ==========
    bool initAppointmentModel();                     // 初始化预约模型
    QItemSelectionModel *theAppointmentSelection = nullptr; // 预约选择模型
    void revertAppointmentEdit()                     // 撤销预约修改
    {
        if (appointmentTabModel) appointmentTabModel->revertAll();
    }
    void searchAppointment(const QString &filter)    // 筛选预约
    {
        if (appointmentTabModel) appointmentTabModel->setFilter(filter);
    }
    bool deleteCurrentAppointment();                 // 删除当前预约
    int addNewAppointment();                         // 新增预约行
};

#endif // IDATABASE_H

