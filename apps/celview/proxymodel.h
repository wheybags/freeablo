#ifndef __CELVIEW_MODEL_H__
#define __CELVIEW_MODEL_H__

#include <QSortFilterProxyModel>
#include <QString>

class ProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    ProxyModel(QObject *parent = 0);
    void find(const QString& value);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const Q_DECL_OVERRIDE;
    QString mFind;
};

#endif
