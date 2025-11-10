#pragma once

#include <QDebug>
#include <QMap>
#include <QString>
#include <string>
#include <tuple>

class Plugin
{
public:
    //virtual ~Plugin() = 0;

    virtual QString getPluginNameShort() = 0;
    virtual QString getPluginNameLong() = 0;
    virtual QString getPluginVersion() = 0;
    virtual QString getPluginDescription() = 0;

    virtual void setHashMap(const QHash<QString, QString> hashMap, QString type) = 0;
    virtual QHash<QString, QString> getHashMap(QString type) = 0;

    virtual std::tuple<bool, std::string> parseFile(QMap<QString, QString> &mapParseKeys,
                                                    QString pathToFile)
        = 0;
    virtual std::tuple<bool, std::string> writeFile(QMap<QString, QString> mapParseKeys,
                                                    QMap<QString, QString> mapFileAttribs,
                                                    QString pathToFile)
        = 0;
};

Q_DECLARE_INTERFACE(Plugin, "net.hase-zheng.photo_gallery_plugins");
