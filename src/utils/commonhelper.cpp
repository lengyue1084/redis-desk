#include "utils/commonhelper.h"
#include <QWidget>
#include <QStyle>
#include <QVariantAnimation>
#include <QPixmap>
#include <QPainter>
#include <QIcon>

void CommonHelper::refreshStyle(QWidget* widget)
{
    if(!widget) return;
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
    widget->update();
}

void CommonHelper::refreshListStyle(QList<QWidget *> list)
{
    foreach (QWidget * widget, list) {
        refreshStyle(widget);
    }
}

void CommonHelper::spinRefreshIcon(QPushButton *btn, int durationMs)
{
    if (!btn) return;

    QIcon originalIcon = btn->icon();
    QSize sz = btn->iconSize();
    QPixmap srcPix = originalIcon.pixmap(sz);

    auto *anim = new QVariantAnimation(btn);
    anim->setStartValue(0.0);
    anim->setEndValue(360.0);
    anim->setDuration(durationMs);
    anim->setEasingCurve(QEasingCurve::InOutCubic);

    QObject::connect(anim, &QVariantAnimation::valueChanged, btn, [btn, srcPix, sz](const QVariant &val) {
        qreal angle = val.toReal();
        QPixmap rotated(sz);
        rotated.fill(Qt::transparent);
        QPainter p(&rotated);
        p.setRenderHint(QPainter::SmoothPixmapTransform);
        p.translate(sz.width() / 2.0, sz.height() / 2.0);
        p.rotate(angle);
        p.drawPixmap(-srcPix.width() / 2.0, -srcPix.height() / 2.0, srcPix);
        p.end();
        btn->setIcon(QIcon(rotated));
    });

    QObject::connect(anim, &QVariantAnimation::finished, btn, [btn, originalIcon, anim]() {
        btn->setIcon(originalIcon);
        anim->deleteLater();
    });

    anim->start();
}

bool CommonHelper::confirm(QWidget *parent, const QString &title, const QString &text,
                           QMessageBox::Icon icon)
{
    QMessageBox box(icon, title, text, QMessageBox::NoButton, parent);
    auto *yesBtn = box.addButton(QStringLiteral("是"), QMessageBox::YesRole);
    box.addButton(QStringLiteral("否"), QMessageBox::NoRole);
    box.setDefaultButton(qobject_cast<QPushButton*>(yesBtn));
    box.exec();
    return box.clickedButton() == yesBtn;
}

