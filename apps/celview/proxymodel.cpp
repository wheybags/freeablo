#include "proxymodel.h"

ProxyModel::ProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

void ProxyModel::find(const QString &value)
{
    mFind = value;
    invalidateFilter();
}

bool ProxyModel::filterAcceptsRow(int sourceRow,
        const QModelIndex &sourceParent) const
{
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);

    return sourceModel()->data(index0).toString().contains(mFind);
}
