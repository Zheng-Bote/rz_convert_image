#include "includes/rz_convert_image.hpp"
#include "includes/rz_config.h"

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
    setImageStruct(pathToFile);

    return std::make_tuple(true, "");
}

std::tuple<bool, std::string> Rz_convertImage::writeFile(QMap<QString, QString> mapParseKeys,
                                                         QMap<QString, QString> mapFileAttribs,
                                                         QString pathToFile)
{
    convertImages();
    return std::make_tuple(true, "");
}

void Rz_convertImage::setHashMap(const QHash<QString, QString> defaultMetaKeys, QString type) {}

QHash<QString, QString> Rz_convertImage::getHashMap(QString type)
{
    QHash<QString, QString> defaultMetaKeys;
    return defaultMetaKeys;
}

const bool Rz_convertImage::convertImage(const int &targetSize)
{
    bool oknok{false};
    std::string msg{"blank"};

    QString imgIn = imgStruct.fileAbolutePath
                    + (QDir::separator() + imgStruct.fileBasename + "." + imgStruct.fileSuffix);
    QString imgOutName = imgStruct.fileBasename + "__" + QString::number(targetSize) + "."
                         + newImageStruct.newSuffix;

    QString imgOutPath = imgStruct.fileAbolutePath + QDir::separator() + newImageStruct.newFolder
                         + QDir::separator() + imgOutName;

    std::tie(oknok, msg) = isTargetExist(QFile(imgOutPath), "Dir");
    if (!oknok) {
        createWebpPath();
    }

    QImage imageInput{imgIn};
    QImage imageOutput = imageInput.scaledToWidth(targetSize, Qt::SmoothTransformation);

    newImageStruct.doWatermarkPicture = true;
    if (newImageStruct.doWatermarkPicture) {
        QImage reducedCopy{"://resources/img/reduced_copy.png"};
        QPainter painter(&imageOutput);
        painter.drawImage(0, 0, reducedCopy);
    }

    if (!imageOutput.save(imgOutPath, "WEBP", newImageStruct.quality)) {
        qDebug() << "convertImage failed to save webp image: " << imgOutPath;
        //std::filesystem::remove(imgOut.toStdString());
        return false;
    }

    return true;
}

const bool Rz_convertImage::convertImages()
{
    bool ret{false};
    for (const auto &size : newImageStruct.webpSizes) {
        QFuture<bool> future = QtConcurrent::run(&Rz_convertImage::convertImage, this, size);
        ret = future.result();
    }
    return ret;
}

void Rz_convertImage::setImageStruct(QString &pathToImageInput)
{
    QFileInfo fileInfo(pathToImageInput);
    //imgStruct = *new class imageStruct;

    imgStruct.fileName = fileInfo.fileName();
    imgStruct.fileBasename = fileInfo.completeBaseName();
    imgStruct.fileSuffix = fileInfo.completeSuffix();
    imgStruct.fileAbolutePath = fileInfo.absolutePath();
}

std::tuple<bool, std::string> Rz_convertImage::isTargetExist(const QFile &pathToTarget,
                                                             const QString type)
{
    QFileInfo fInfo(pathToTarget);

    if (!pathToTarget.exists()) {
        return std::make_tuple(false, "Rz_convertImage::isTargetExist:: no");
    }
    if (type.contains("dir") && fInfo.isDir() && fInfo.isWritable()) {
        return std::make_tuple(true, "Rz_convertImage::isTargetExist::dir: yes");
    }
    if (type.contains("file") && fInfo.isFile() && fInfo.isWritable()) {
        return std::make_tuple(true, "Rz_convertImage::isTargetExist::file: yes");
    }
    return std::make_tuple(false, "Rz_convertImage::isTargetExist:: no");
}

bool Rz_convertImage::createWebpPath()
{
    QDir path = imgStruct.fileAbolutePath;
    QString newPath = imgStruct.fileAbolutePath + QDir::separator() + newImageStruct.newFolder;
    if (path.mkpath(newPath)) {
        // qDebug() << "createPath OK: " << newPath.toStdString();
        return true;
    } else {
        qCritical() << "createPath NOK: " << newPath.toStdString();
        return false;
    }
}

QString Rz_convertImage::getPhotoDateTimeHuman()
{
    std::string pathToFile = imgStruct.fileAbolutePath.append(imgStruct.fileName).toStdString();

    auto ftime = std::filesystem::last_write_time(pathToFile);
    auto datetime = std::format("{0:%Y-%m-%d}_{0:%H%M%S}", ftime);
    QString dt = datetime.c_str();
    dt = dt.remove(QRegularExpression("(\\.\\d+)$"));
    return dt;
}
