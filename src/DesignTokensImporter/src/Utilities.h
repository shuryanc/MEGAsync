#ifndef UTILITIES
#define UTILITIES

#include "Types.h"

#include <QString>
#include <QMap>
#include <QJsonObject>
#include <QVector>

namespace DTI
{
    class Utilities
    {

    public:
         Utilities() = delete;

        enum class Theme
        {
            LIGHT,
            DARK
        };

        static bool createDirectory(const QString& path);
        static QStringList findFilesInDir(const QString& dirPath,
                                          const QString& nameFilter = QString::fromLatin1("*.*"),
                                          bool findInSubfolders = false);
        static bool isFileValid(const QString& path);
        static QMap<QString, QString> parseColorTheme(const QJsonObject& jsonThemeObject, const ColorData& colorData);
        static void traverseDirectory(const QString& directoryPath,
                                      const QStringList& filters,
                                      QStringList& filePaths);
        static bool addToResources(const QString& filePath, const QString& qrcPath);
        static QString extractFileName(const QString& filePath);
        static QString extractFileNameNoExtension(const QString& filePath);
        static QString getFileHash(const QString& filePath);
        static QMap<QString, QMap<QString, QString>> readHashesJSONFile(const QString& filePath);
        static bool writeHashesJsonFile(const QList<QStringList> &filePaths,
                                       const QStringList &jsonObjectNames,
                                       const QString &outputFilePath);
        static QJsonObject createWidgetStyleSheet(const QString& objectName,
                                                  const QMap<QString, QString>& properties);
        static bool writeJSONToFile(const QJsonDocument& jsonDoc,
                                    const QString& filePath);
        static QString getSubStringBetweenMarkers(const QString& str,
                                                  const QString& startMarker,
                                                  const QString& stopMarker);
        static bool areAllStringsPresent(const QStringList& list1, const QStringList& list2);
        static bool writeStyleSheetToFile(const QString& css, const QString& filePath);
        static QString themeToString(Utilities::Theme theme);
        static QString resolvePath(const QString& basePath, const QString& relativePath);
        //!
        //! \brief Utilities::findValueByKey
        //! \param myMap Qmap with key/value pairs
        //! \param key key value that needs to match a key in @myMap
        //! \returns the value if found, or default-constructed value if not found
        //!
        template <typename Key, typename Value>
        static Value findValueByKey(const QMap<Key, Value>& map, const Key& key)
        {
            auto it = map.find(key);
            return (it != map.end()) ? it.value() : Value();
        }
    };
}


#endif // UTILITIES
