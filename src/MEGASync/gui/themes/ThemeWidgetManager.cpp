#include "ThemeWidgetManager.h"

#include "themes/ThemeManager.h"

#include <QDir>
#include <QWidget>
#include <QtConcurrent/QtConcurrent>

static const QMap<Preferences::ThemeType, QString> themeNames = {
    {Preferences::ThemeType::LIGHT_THEME,  QObject::tr("Light")},
    {Preferences::ThemeType::DARK_THEME,  QObject::tr("Dark")}
};

static QRegularExpression designTokensRE(QLatin1String("(#.*)\\/\\* *colorToken\\.(.*)\\*\\/"));
static const QString jsonThemedColorFile = QLatin1String(":/colors/ColorThemedTokens.json");

ThemeWidgetManager::ThemeWidgetManager(QObject *parent)
    : QObject{parent},
    mCurrentWidget{nullptr}
{
    connect(ThemeManager::instance(), &ThemeManager::themeChanged, this, &ThemeWidgetManager::onThemeChanged);

    designTokensRE.optimize();

    loadColorThemeJson();
}

void ThemeWidgetManager::loadColorThemeJson()
{
    mColorThemedTokens.clear();

    QFile file(jsonThemedColorFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << __func__ << " Error opening file : " << file.fileName();
        return;
    }

    QByteArray jsonData = file.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (!jsonDoc.isObject())
    {
        qDebug() << __func__ << " Error invalid json format on file : " << file.fileName();
        return;
    }

    QJsonObject rootObj = jsonDoc.object();
    for (auto it = rootObj.begin(); it != rootObj.end(); ++it)
    {
        QMap<QString, QString> tokens;

        QString theme = it.key();
        QJsonObject token = it.value().toObject();

        for (auto innerIt = token.begin(); innerIt != token.end(); ++innerIt)
        {
            tokens.insert(innerIt.key(), innerIt.value().toString());
        }

        mColorThemedTokens.insert(theme, tokens);
    }
}

void ThemeWidgetManager::applyCurrentTheme(QWidget* widget)
{
    if (widget == nullptr || widget == mCurrentWidget || widget->styleSheet().isEmpty())
    {
        return;
    }

    mCurrentWidget = widget;

    applyTheme(widget);
}

void ThemeWidgetManager::applyTheme(QWidget* widget)
{
    auto theme = ThemeManager::instance()->getSelectedTheme();
    auto currentTheme = themeToString(theme);

    if (!mColorThemedTokens.contains(currentTheme))
    {
        qDebug() << __func__ << " Error theme not found : " << currentTheme;
        return;
    }

    const auto& themedColorTokens = mColorThemedTokens.value(currentTheme);

    QString styleSheet = widget->styleSheet();

    bool updatedStyleSheet = false;
    QRegularExpressionMatchIterator matchIterator = designTokensRE.globalMatch(styleSheet);
    while (matchIterator.hasNext()) {
        QRegularExpressionMatch match = matchIterator.next();

        /*
         * Our regular expresion has three captures
         * 0 - The whole match
         * 1 - The current hex color value
         * 2 - The design token name
        */
        if (match.lastCapturedIndex() == 2)
        {
            const QString& tokenValue = themedColorTokens.value(match.captured(2));

            auto startIndex = match.capturedStart(1);
            auto endIndex = match.capturedEnd(1);
            styleSheet.replace(startIndex, endIndex-startIndex, tokenValue);
            updatedStyleSheet = true;
        }
    }

    if (updatedStyleSheet)
    {
        widget->setStyleSheet(styleSheet);
    }
}

std::shared_ptr<ThemeWidgetManager> ThemeWidgetManager::instance()
{
    static std::shared_ptr<ThemeWidgetManager> manager(new ThemeWidgetManager());
    return manager;
}

QString ThemeWidgetManager::themeToString(Preferences::ThemeType theme) const
{
    return themeNames.value(theme, QLatin1String("Light"));
}

void ThemeWidgetManager::onThemeChanged(Preferences::ThemeType theme)
{
    Q_UNUSED(theme)

    if (mCurrentWidget != nullptr)
    {
        applyTheme(mCurrentWidget);
    }
}



