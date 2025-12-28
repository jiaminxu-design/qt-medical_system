#include "StatThread.h"
#include <QSqlQuery>
#include <QDebug>

StatThread::StatThread(QObject *parent) : QThread(parent)
{
}

void StatThread::run()
{
    QVariantMap statData;

    if (!db.isDatabaseOpen()) { // 替换为公共方法
        qDebug() << "数据库未打开，药品统计失败！";
        emit statFinished(statData);
        return;
    }

    // ========== 药品核心统计逻辑 ==========
    // 1. 总药品数
    QSqlQuery totalQuery = db.querySql("SELECT COUNT(*) FROM Medicine");
    if (totalQuery.first()) {
        statData["总药品数"] = totalQuery.value(0).toInt();
    }

    // 2. 库存不足药品数（STOCK < 10 为缺货阈值）
    QSqlQuery lackQuery = db.querySql("SELECT COUNT(*) FROM Medicine WHERE STOCK < 10");
    if (lackQuery.first()) {
        statData["库存不足药品数"] = lackQuery.value(0).toInt();
    }

    // 3. 各分类药品数量（抗生素/镇痛药/感冒药）
    QSqlQuery cateQuery = db.querySql("SELECT CATEGORY, COUNT(*) FROM Medicine GROUP BY CATEGORY");
    while (cateQuery.next()) {
        QString cate = cateQuery.value(0).toString();
        int count = cateQuery.value(1).toInt();
        statData["分类_" + cate] = count;
    }

    // 4. 库存状态分布（正常库存/缺货）
    QSqlQuery statusQuery =
        db.querySql("SELECT STOCK_STATUS, COUNT(*) FROM Medicine GROUP BY STOCK_STATUS");
    while (statusQuery.next()) {
        QString status = statusQuery.value(0).toString();
        int count = statusQuery.value(1).toInt();
        statData["库存状态_" + status] = count;
    }

    // 5. 总库存价值（单价*库存求和）
    QSqlQuery valueQuery = db.querySql("SELECT SUM(PRICE * STOCK) FROM Medicine");
    if (valueQuery.first()) {
        double totalValue = valueQuery.value(0).toDouble();
        statData["总库存价值(元)"] = QString::number(totalValue, 'f', 2);
    }

    // 发送统计结果
    emit statFinished(statData);
}
