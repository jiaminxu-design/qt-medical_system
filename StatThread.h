#ifndef STATTHREAD_H
#define STATTHREAD_H

#include <QThread>
#include <QVariantMap>
#include "idatabase.h"

class StatThread : public QThread
{
    Q_OBJECT
public:
    explicit StatThread(QObject *parent = nullptr);

protected:
    void run() override;

signals:
    void statFinished(const QVariantMap &statData);

private:
    IDatabase &db = IDatabase::getInstance();
};

#endif // STATTHREAD_H
