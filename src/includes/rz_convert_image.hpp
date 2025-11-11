#pragma once

#include <QDir>
#include <QFile>
#include <QFuture>
#include <QFutureWatcher>
#include <QImage>
#include <QMatrix2x2>
#include <QObject>
#include <QPainter>
#include <QThread>
#include <QtConcurrent>
#include <QtPlugin>

#include "rz_photo-gallery_plugins.hpp"

class Rz_convertImage : public QObject, public Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "net.hase-zheng.photo_gallery_plugins");
    Q_INTERFACES(Plugin);

private:
    struct imageStruct
    {
        QString fileName{""};        // 2014-04-18_203353.jpg
        QString fileBasename{""};    // 2014-04-18_203353
        QString fileSuffix{""};      // jpg
        QString fileAbolutePath{""}; // /home/zb_bamboo/pictures/images
    };
    imageStruct imgStruct;
    void setImageStruct(QString &pathToImageInput);

    struct newImageStruct
    {
        QString newFolder{"WebP"};
        QString newSuffix{"webp"};
        int quality{75};
        QList<int> webpSizes = {480, 680, 800, 1024, 1280};
        bool doOversizeSmallerPicture{false};
        bool doWatermarkPicture{false};
        bool doRenameToDateTime{false};
        QString watermarkPicture{":/resources/img/reduced_copy.png"};
    };
    newImageStruct newImageStruct;

    const bool convertImage(const int &targetSize);
    const bool convertImages();

    std::tuple<bool, std::string> isTargetExist(const QFile &pathToTarget, const QString type);
    bool createWebpPath();
    QString getPhotoDateTimeHuman();

public:
    explicit Rz_convertImage(QObject *parent = nullptr);
    ~Rz_convertImage();

signals:
public slots:

    // Plugin interface
public:
    QString getPluginNameShort() Q_DECL_OVERRIDE;
    QString getPluginNameLong() Q_DECL_OVERRIDE;
    QString getPluginVersion() Q_DECL_OVERRIDE;
    QString getPluginDescription() Q_DECL_OVERRIDE;

    std::tuple<bool, std::string> parseFile(QMap<QString, QString> &empty,
                                            QString pathToFile) Q_DECL_OVERRIDE;
    std::tuple<bool, std::string> writeFile(QMap<QString, QString> mapParseKeys,
                                            QMap<QString, QString> mapFileAttribs,
                                            QString pathToFile) Q_DECL_OVERRIDE;
    void setHashMap(const QHash<QString, QString> defaultMetaKeys, QString type) Q_DECL_OVERRIDE;
    QHash<QString, QString> getHashMap(QString type) Q_DECL_OVERRIDE;
};
