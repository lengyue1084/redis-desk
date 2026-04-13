#include "delegates/connectionpanel/connectionpaneldelegate.h"

#include <QLineEdit>
#include <QLinearGradient>
#include <QPainter>

#include "utils/fontmanager.h"

ConnectionPanelDelegrate::ConnectionPanelDelegrate(QObject *parent)
    : BaseDelegate(parent)
{
}

void ConnectionPanelDelegrate::paint(QPainter *painter,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const
{
    const bool isActive = index.data(Qt::UserRole + 11).toBool();
    const bool isConnecting = index.data(Qt::UserRole + 12).toBool();
    const bool isHovered = option.state & QStyle::State_MouseOver;
    const bool isSelected = option.state & QStyle::State_Selected;

    QIcon icon(QStringLiteral(":/images/icons/icon-client.png"));
    if (isActive || isConnecting || isHovered || isSelected)
        icon = QIcon(QStringLiteral(":/images/icons/icon-client-active.png"));

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    const QRect backgroundRect = option.rect.adjusted(2, 1, -2, -1);
    if (isActive) {
        QLinearGradient gradient(backgroundRect.topLeft(), backgroundRect.topRight());
        gradient.setColorAt(0.0, QColor(147, 51, 234, 40));
        gradient.setColorAt(1.0, QColor(147, 51, 234, 18));
        painter->setBrush(gradient);
        painter->setPen(QPen(QColor(168, 85, 247, 140), 1));
        painter->drawRoundedRect(backgroundRect, 6, 6);

        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(168, 85, 247));
        painter->drawRoundedRect(
            QRect(option.rect.left(), option.rect.top() + 6, 3, option.rect.height() - 12), 1, 1);
    } else if (isConnecting) {
        QLinearGradient gradient(backgroundRect.topLeft(), backgroundRect.bottomRight());
        gradient.setColorAt(0.0, QColor(245, 158, 11, 34));
        gradient.setColorAt(1.0, QColor(217, 119, 6, 16));
        painter->setBrush(gradient);
        painter->setPen(QPen(QColor(251, 191, 36, 120), 1));
        painter->drawRoundedRect(backgroundRect, 6, 6);
    } else if (isHovered || isSelected) {
        painter->setBrush(QColor(55, 65, 81, 80));
        painter->setPen(Qt::NoPen);
        painter->drawRoundedRect(backgroundRect, 6, 6);
    } else {
        painter->fillRect(option.rect, option.palette.base());
    }
    painter->restore();

    const double scale = FontManager::getFontScale(option.widget);
    const int leftMargin = static_cast<int>(10 * scale);
    const int iconWidth = static_cast<int>(15 * scale);
    const int indicatorWidth = isConnecting ? static_cast<int>(30 * scale) : 0;
    const int rightPadding = isConnecting ? static_cast<int>(12 * scale) : static_cast<int>(8 * scale);

    painter->save();
    QRect iconRect = option.rect.adjusted(leftMargin, 0, 0, 0);
    iconRect.setWidth(iconWidth);
    icon.paint(painter, iconRect, Qt::AlignCenter);
    painter->restore();

    if (isConnecting) {
        painter->save();
        const int indicatorHeight = static_cast<int>(18 * scale);
        const QRect indicatorRect(option.rect.right() - rightPadding - indicatorWidth,
                                  option.rect.center().y() - indicatorHeight / 2,
                                  indicatorWidth,
                                  indicatorHeight);
        drawConnectingIndicator(painter, indicatorRect);
        painter->restore();
    }

    painter->save();
    QFont font = FontManager::getTitleMenuFont(option.widget);
    painter->setFont(font);

    QRect textRect = option.rect.adjusted(static_cast<int>(30 * scale),
                                          0,
                                          -(indicatorWidth + rightPadding),
                                          0);

    QColor textColor(156, 163, 175);
    if (isActive) {
        textColor = QColor(243, 232, 255);
    } else if (isConnecting) {
        textColor = QColor(255, 247, 237);
    } else if (isHovered || isSelected) {
        textColor = Qt::white;
    }

    painter->setPen(textColor);
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, index.data(Qt::DisplayRole).toString());
    painter->restore();
}

QWidget *ConnectionPanelDelegrate::createEditor(QWidget *parent,
                                                const QStyleOptionViewItem &option,
                                                const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    return new QLineEdit(parent);
}

void ConnectionPanelDelegrate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    auto *lineEdit = static_cast<QLineEdit *>(editor);
    lineEdit->setText(index.data(Qt::DisplayRole).toString());
}

void ConnectionPanelDelegrate::setModelData(QWidget *editor,
                                            QAbstractItemModel *model,
                                            const QModelIndex &index) const
{
    auto *lineEdit = static_cast<QLineEdit *>(editor);
    model->setData(index, lineEdit->text(), Qt::EditRole);
}

QSize ConnectionPanelDelegrate::sizeHint(const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    const double scale = FontManager::getFontScale(option.widget);
    const int minHeight = static_cast<int>(30 * scale);
    size.setHeight(qMax(size.height(), minHeight));
    return size;
}

void ConnectionPanelDelegrate::drawConnectingIndicator(QPainter *painter, const QRect &rect) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(255, 255, 255, 18));
    painter->drawRoundedRect(rect, rect.height() / 2.0, rect.height() / 2.0);

    const int dotSize = qMax(3, rect.height() / 4);
    const int spacing = dotSize + 2;
    const int totalWidth = dotSize * 3 + 2 * 2;
    const int startX = rect.center().x() - totalWidth / 2;
    const int y = rect.center().y() - dotSize / 2;

    const QColor dotColors[] = {
        QColor(253, 224, 71, 255),
        QColor(251, 191, 36, 220),
        QColor(245, 158, 11, 180),
    };

    for (int i = 0; i < 3; ++i) {
        painter->setBrush(dotColors[i]);
        painter->drawEllipse(QRect(startX + i * spacing, y, dotSize, dotSize));
    }

    painter->restore();
}
