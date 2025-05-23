/*
    SPDX-FileCopyrightText: 2009, 2010, 2012 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KDecoration3/Decoration>
#include <KDecoration3/DecorationThemeProvider>
#include <KPluginMetaData>
#include <QElapsedTimer>
#include <QVariant>

class QQmlComponent;
class QQmlContext;
class QQmlEngine;
class QQuickItem;

class KConfigLoader;

namespace Aurorae
{

class Borders;
class Renderer;

class Decoration : public KDecoration3::Decoration
{
    Q_OBJECT
    // TODO Plasma 7, maybe? Rename this to "window"
    Q_PROPERTY(KDecoration3::DecoratedWindow *client READ window CONSTANT)
    Q_PROPERTY(QQuickItem *item READ item)
public:
    explicit Decoration(QObject *parent = nullptr, const QVariantList &args = QVariantList());
    ~Decoration() override;

    void paint(QPainter *painter, const QRectF &repaintRegion) override;

    Q_INVOKABLE QVariant readConfig(const QString &key, const QVariant &defaultValue = QVariant());

    QQuickItem *item() const;

public Q_SLOTS:
    bool init() override;
    void installTitleItem(QQuickItem *item);

    void updateShadow();
    void updateBlur();

Q_SIGNALS:
    void configChanged();

protected:
    void hoverEnterEvent(QHoverEvent *event) override;
    void hoverLeaveEvent(QHoverEvent *event) override;
    void hoverMoveEvent(QHoverEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void setupBorders(QQuickItem *item);
    void updateBorders();
    void updateBuffer();
    void updateExtendedBorders();

    bool m_supportsMask{false};

    QRectF m_contentRect; // the geometry of the part of the buffer that is not a shadow when buffer was created.
    std::unique_ptr<QQuickItem> m_item;
    std::unique_ptr<QQmlContext> m_qmlContext;
    Borders *m_borders;
    Borders *m_maximizedBorders;
    Borders *m_extendedBorders;
    Borders *m_padding;
    QString m_themeName;

    std::unique_ptr<Renderer> m_view;
};

class ThemeProvider : public KDecoration3::DecorationThemeProvider
{
    Q_OBJECT
public:
    explicit ThemeProvider(QObject *parent, const KPluginMetaData &data);

    QList<KDecoration3::DecorationThemeMetaData> themes() const override
    {
        return m_themes;
    }

private:
    void init();
    void findAllQmlThemes();
    void findAllSvgThemes();
    bool hasConfiguration(const QString &theme);
    QList<KDecoration3::DecorationThemeMetaData> m_themes;
    const KPluginMetaData m_data;
};
}
