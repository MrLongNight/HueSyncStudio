#include "LampModel.h"

LampModel::LampModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int LampModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_lamps.count();
}

QVariant LampModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_lamps.count()) return QVariant();

    const Lamp &lamp = m_lamps.at(index.row());

    switch (role) {
    case IdRole:
        return QVariant(lamp.id);
    case NameRole:
        return QVariant(lamp.name);
    case XPosRole:
        return QVariant(lamp.x);
    case YPosRole:
        return QVariant(lamp.y);
    case IsPreviewRole:
        return m_previewLampIdSet.contains(lamp.id);
    }

    return QVariant();
}

bool LampModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && (role == XPosRole || role == YPosRole)) {
        Lamp &lamp = m_lamps[index.row()];
        if (role == XPosRole) lamp.x = value.toDouble();
        if (role == YPosRole) lamp.y = value.toDouble();
        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}

Qt::ItemFlags LampModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) return Qt::NoItemFlags;
    return Qt::ItemIsEditable;
}

void LampModel::addLamp(const Lamp &lamp)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_lamps.append(lamp);
    endInsertRows();
}

void LampModel::updateLampPosition(int id, double x, double y)
{
    for (int i = 0; i < m_lamps.count(); ++i) {
        if (m_lamps[i].id == id) {
            m_lamps[i].x = x;
            m_lamps[i].y = y;
            QModelIndex idx = index(i, 0);
            emit dataChanged(idx, idx, {XPosRole, YPosRole});
            return;
        }
    }
}

QList<int> LampModel::previewLampIds() const
{
    return m_previewLampIdList;
}

void LampModel::setPreviewLampIds(const QList<int>& ids)
{
    m_previewLampIdList = ids;
    m_previewLampIdSet = QSet<int>(ids.begin(), ids.end());

    // Notify the view that the preview status has changed for all items
    emit dataChanged(index(0, 0), index(rowCount() - 1, 0), {IsPreviewRole});
    emit previewLampIdsChanged();
}

QHash<int, QByteArray> LampModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[NameRole] = "name";
    roles[XPosRole] = "x_pos";
    roles[YPosRole] = "y_pos";
    roles[IsPreviewRole] = "is_preview";
    return roles;
}
