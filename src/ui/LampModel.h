#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QSet>
#include "../core/Lamp.h"

class LampModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QList<int> previewLampIds READ previewLampIds WRITE setPreviewLampIds NOTIFY previewLampIdsChanged)

public:
    enum LampRoles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        XPosRole,
        YPosRole,
        IsPreviewRole // Role to check if lamp is in preview
    };

    explicit LampModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void addLamp(const Lamp &lamp);

    Q_INVOKABLE void updateLampPosition(int id, double x, double y);

    QList<int> previewLampIds() const;
    void setPreviewLampIds(const QList<int>& ids);

signals:
    void previewLampIdsChanged();

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    QList<Lamp> m_lamps;
    QSet<int> m_previewLampIdSet; // Use a QSet for efficient lookups
    QList<int> m_previewLampIdList;
};
