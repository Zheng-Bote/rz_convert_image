#include "includes/rz_convert_image.hpp"
#include "includes/rz_config.h"

#include <QDir>

const QString PLUGIN_SHORTNAME = PROG_EXEC_NAME;
const QString PLUGIN_NAME = PROG_NAME;
const QString PLUGIN_VERSION = PROG_VERSION;
const QString PLUGIN_DESCRIPTION = PROG_DESCRIPTION;

Rz_convertImage::Rz_convertImage(QObject *parent)
{
    Q_UNUSED(parent);
}

Rz_convertImage::~Rz_convertImage() {}

QString Rz_convertImage::getPluginNameShort()
{
    return PLUGIN_SHORTNAME;
}

QString Rz_convertImage::getPluginNameLong()
{
    return PLUGIN_NAME;
}

QString Rz_convertImage::getPluginVersion()
{
    return PLUGIN_NAME + "-v" + PLUGIN_VERSION;
}

QString Rz_convertImage::getPluginDescription()
{
    return PLUGIN_DESCRIPTION;
}

std::tuple<bool, std::string> Rz_convertImage::parseFile(QMap<QString, QString> &empty,
                                                         QString pathToFile)
{
    return std::make_tuple(true, "");
}

std::tuple<bool, std::string> Rz_convertImage::writeFile(QMap<QString, QString> mapParseKeys,
                                                         QMap<QString, QString> mapFileAttribs,
                                                         QString pathToFile)
{
    return std::make_tuple(true, "");
}

void Rz_convertImage::setHashMap(const QHash<QString, QString> defaultMetaKeys, QString type) {}

QHash<QString, QString> Rz_convertImage::getHashMap(QString type)
{
    QHash<QString, QString> defaultMetaKeys;
    return defaultMetaKeys;
}

std::tuple<bool, std::string> Rz_convertImage::isTargetExist(const QFile &pathToTarget,
                                                             const QString type)
{
    QFileInfo fInfo(pathToTarget);

    if (!pathToTarget.exists()) {
        return std::make_tuple(false, "Rz_convertImage::isTargetExist:: no");
    }
    if (type.contains("dir") && fInfo.isDir()) {
        return std::make_tuple(false, "Rz_convertImage::isTargetExist:: yes");
    }
    if (type.contains("file") && fInfo.isFile()) {
        return std::make_tuple(false, "Rz_convertImage::isTargetExist:: yes");
    }
    return std::make_tuple(false, "Rz_convertImage::isTargetExist:: no");
}

bool Rz_convertImage::createWebpPath()
{
    QDir path = imgStruct.fileAbolutePath;
    QString newPath = imgStruct.fileAbolutePath.append(imgStruct.newFolder);
    if (path.mkpath(newPath)) {
        // qDebug() << "createPath OK: " << newPath.toStdString();
        return true;
    } else {
        qCritical() << "createPath NOK: " << newPath.toStdString();
        return false;
    }
}
