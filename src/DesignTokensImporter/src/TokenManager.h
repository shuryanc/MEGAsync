#ifndef TOKENMANAGER_H
#define TOKENMANAGER_H

#include "Types.h"

#include <QMap>
#include <QFile>
#include <QStringList>
#include <QJsonArray>
#include <QJsonDocument>

namespace DTI
{
    class TokenManager
    {
    public:
        static TokenManager* instance();
        void run();

    private:
        TokenManager();
        ThemedColorData parseTheme(QFile& designTokensFile, const ColorData& coreData);
        ColorData parseCore(QFile& designTokensFile);
        void recurseCore(QString category, const QJsonObject& coreColors, ColorData& coreData);

        QString mCurrentDir;
    };
}

#endif



