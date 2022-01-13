#ifndef UTILITIES_H
#define UTILITIES_H

#include <QString>
#include <QHash>
#include <QPixmap>
#include <QProgressDialog>
#include <control/MegaController.h>

#include <QDir>
#include <QIcon>
#include <functional>
#include <QLabel>
#include <QEasingCurve>
#include "megaapi.h"
#include "ThreadPool.h"

#include <functional>

#include <sys/stat.h>

#ifdef __APPLE__
#define MEGA_SET_PERMISSIONS chmod("/Applications/MEGAsync.app/Contents/MacOS/MEGAclient", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH); \
                             chmod("/Applications/MEGAsync.app/Contents/MacOS/MEGAupdater", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH); \
                             chmod("/Applications/MEGAsync.app/Contents/PlugIns/MEGAShellExtFinder.appex/Contents/MacOS/MEGAShellExtFinder", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
#endif

#define MegaSyncApp (static_cast<MegaApplication *>(QCoreApplication::instance()))

struct PlanInfo
{
    long long gbStorage;
    long long gbTransfer;
    unsigned int minUsers;
    int level;
    int gbPerStorage;
    int gbPerTransfer;
    unsigned int pricePerUserBilling;
    unsigned int pricePerUserLocal;
    unsigned int pricePerStorageBilling;
    unsigned int pricePerStorageLocal;
    unsigned int pricePerTransferBilling;
    unsigned int pricePerTransferLocal;
    QString billingCurrencySymbol;
    QString billingCurrencyName;
    QString localCurrencySymbol;
    QString localCurrencyName;
};

struct PSA_info
{
    int idPSA;
    QString title;
    QString desc;
    QString urlImage;
    QString textButton;
    QString urlClick;

    PSA_info()
    {
        clear();
    }

    PSA_info(const PSA_info& info)
    {
        idPSA = info.idPSA;
        title = info.title;
        desc = info.desc;
        urlImage = info.urlImage;
        textButton = info.textButton;
        urlClick = info.urlClick;
    }

    void clear()
    {
        idPSA = -1;
        title = QString();
        desc = QString();
        urlImage = QString();
        textButton = QString();
        urlClick = QString();
    }
};

class IStorageObserver
{
public:
    virtual ~IStorageObserver() = default;
    virtual void updateStorageElements() = 0;
};

class IBandwidthObserver
{
public:
    virtual ~IBandwidthObserver() = default;
    virtual void updateBandwidthElements() = 0;
};

class IAccountObserver
{
public:
    virtual ~IAccountObserver() = default;
    virtual void updateAccountElements() = 0;
};

class StorageDetailsObserved
{
public:
    virtual ~StorageDetailsObserved() = default;
    void attachStorageObserver(IStorageObserver& obs)
    {
        storageObservers.push_back(&obs);
    }
    void dettachStorageObserver(IStorageObserver& obs)
    {
        storageObservers.erase(std::remove(storageObservers.begin(), storageObservers.end(), &obs), storageObservers.end());
    }

    void notifyStorageObservers()
    {
        for (IStorageObserver* o : storageObservers)
        {
            o->updateStorageElements();
        }
    }

private:
    std::vector<IStorageObserver*> storageObservers;
};

class BandwidthDetailsObserved
{
public:
    virtual ~BandwidthDetailsObserved() = default;
    void attachBandwidthObserver(IBandwidthObserver& obs)
    {
        bandwidthObservers.push_back(&obs);
    }
    void dettachBandwidthObserver(IBandwidthObserver& obs)
    {
        bandwidthObservers.erase(std::remove(bandwidthObservers.begin(), bandwidthObservers.end(), &obs));
    }

    void notifyBandwidthObservers()
    {
        for (IBandwidthObserver* o : bandwidthObservers)
        {
            o->updateBandwidthElements();
        }
    }

private:
    std::vector<IBandwidthObserver*> bandwidthObservers;
};


class AccountDetailsObserved
{
public:
    virtual ~AccountDetailsObserved() = default;
    void attachAccountObserver(IAccountObserver& obs)
    {
        accountObservers.push_back(&obs);
    }
    void dettachAccountObserver(IAccountObserver& obs)
    {
        accountObservers.erase(std::remove(accountObservers.begin(), accountObservers.end(), &obs));
    }

    void notifyAccountObservers()
    {
        for (IAccountObserver* o : accountObservers)
        {
            o->updateAccountElements();
        }
    }

private:
    std::vector<IAccountObserver*> accountObservers;
};

class ThreadPoolSingleton
{
    private:
        static std::unique_ptr<ThreadPool> instance;
        ThreadPoolSingleton() {}

    public:
        static ThreadPool* getInstance()
        {
            if (instance == nullptr)
            {
                instance.reset(new ThreadPool(1));
            }

            return instance.get();
        }
};


/**
 * @brief The MegaListenerFuncExecuter class
 *
 * it takes an std::function as parameter that will be called upon request finish.
 *
 */
class MegaListenerFuncExecuter : public mega::MegaRequestListener
{
private:
    std::function<void(mega::MegaApi* api, mega::MegaRequest *request, mega::MegaError *e)> onRequestFinishCallback;
    bool mAutoremove = true;
    bool mExecuteInAppThread = true;

public:

    /**
     * @brief MegaListenerFuncExecuter
     * @param func to call upon onRequestFinish
     * @param autoremove whether this should be deleted after func is called
     */
    MegaListenerFuncExecuter(bool autoremove = false,
                             std::function<void(mega::MegaApi* api, mega::MegaRequest *request, mega::MegaError *e)> func = nullptr
                            )
        : mAutoremove(autoremove), onRequestFinishCallback(std::move(func))
    {
    }

    void onRequestFinish(mega::MegaApi *api, mega::MegaRequest *request, mega::MegaError *e);
    virtual void onRequestStart(mega::MegaApi* api, mega::MegaRequest *request) {}
    virtual void onRequestUpdate(mega::MegaApi* api, mega::MegaRequest *request) {}
    virtual void onRequestTemporaryError(mega::MegaApi *api, mega::MegaRequest *request, mega::MegaError* e) {}

    void setExecuteInAppThread(bool executeInAppThread);
};


class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags())
        : QLabel(parent)
    {
#ifndef __APPLE__
        setMouseTracking(true);
#endif
    }

    ~ClickableLabel() {}

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event)
    {
        emit clicked();
    }
#ifndef __APPLE__
    void enterEvent(QEvent *event)
    {
        setCursor(Qt::PointingHandCursor);
    }

    void leaveEvent(QEvent *event)
    {
        setCursor(Qt::ArrowCursor);
    }
#endif

};

class Utilities
{
public:
    static QString getSizeString(unsigned long long bytes);
    static QString getSizeString(long long bytes);
    static QString getTimeString(long long secs, bool secondPrecision = true, bool color = true);
    static QString getQuantityString(unsigned long long quantity);
    static QString getFinishedTimeString(long long secs);
    static bool verifySyncedFolderLimits(QString path);
    static QString extractJSONString(QString json, QString name);
    static long long extractJSONNumber(QString json, QString name);
    static QString getDefaultBasePath();
    static void getPROurlWithParameters(QString &url);
    static QString joinLogZipFiles(mega::MegaApi *megaApi, const QDateTime *timestampSince = nullptr, QString appendHashReference = QString());

    static void adjustToScreenFunc(QPoint position, QWidget *what);
    static QString minProPlanNeeded(std::shared_ptr<mega::MegaPricing> pricing, long long usedStorage);
    static QString getReadableStringFromTs(mega::MegaIntegerList* list);
    static QString getReadablePROplanFromId(int identifier);
    static void animateFadeout(QWidget *object, int msecs = 700);
    static void animateFadein(QWidget *object, int msecs = 700);
    static void animatePartialFadeout(QWidget *object, int msecs = 2000);
    static void animatePartialFadein(QWidget *object, int msecs = 2000);
    static void animateProperty(QWidget *object, int msecs, const char *property, QVariant startValue, QVariant endValue, QEasingCurve curve = QEasingCurve::InOutQuad);
    // Returns remaining days until unix timestamp (floored)
    static void getDaysToTimestamp(int64_t msecsTimestamps, int64_t &remaininDays);
    // Returns remaining days or remainig hours until unix timestamp. Note hours are not in addition to remaininDays
    // i.e. for 1 day & 3 hours remaining, remainingHours will be 27, not 3.
    static void getDaysAndHoursToTimestamp(int64_t msecsTimestamps, int64_t &remaininDays, int64_t &remainingHours);

    // shows a ProgressDialog while some progress goes on. it returns a copy of the object,
    // but the object will be deleted when the progress closes
    static QProgressDialog *showProgressDialog(ProgressHelper *progressHelper, QWidget *parent = nullptr);

private:
    Utilities() {}
    static QHash<QString, QString> extensionIcons;
    static QHash<QString, QString> languageNames;
    static void initializeExtensions();
    static QString getExtensionPixmapNameSmall(QString fileName);
    static QString getExtensionPixmapNameMedium(QString fileName);

//Platform dependent functions
public:
    static QString languageCodeToString(QString code);
    static QString getAvatarPath(QString email);
    static bool removeRecursively(QString path);
    static void copyRecursively(QString srcPath, QString dstPath);

    static void queueFunctionInAppThread(std::function<void()> fun);

    static void getFolderSize(QString folderPath, long long *size);
    static qreal getDevicePixelRatio();

    static QIcon getCachedPixmap(QString fileName);
    static QIcon getExtensionPixmapSmall(QString fileName);
    static QIcon getExtensionPixmapMedium(QString fileName);
    static QString getExtensionPixmapName(QString fileName, QString prefix);

    static long long getSystemsAvailableMemory();

    static void sleepMilliseconds(long long milliseconds);

    // Compute the part per <ref> of <part> from <total>. Defaults to %
    static int partPer(long long  part, long long total, uint ref = 100);
};



// This class encapsulates a MEGA node and adds useful information, like the origin
// of the transfer.
class WrappedNode
{
public:
    // Enum used to record origin of trqnsfer
    enum TransferOrigin {
        FROM_UNKNOWN   = 0,
        FROM_APP       = 1,
        FROM_WEBSERVER = 2,
    };

    // Constructor with origin and pointer to MEGA node. Default to unknown/nullptr
    WrappedNode(TransferOrigin from = WrappedNode::TransferOrigin::FROM_UNKNOWN,
                mega::MegaNode* node = nullptr)
        : mTransfersFrom(from), mNode(node){}

    // Destructor
    ~WrappedNode()
    {
        // MEGA node should be deleted when this is deleted.
        delete mNode;
    }

    // Get the transfer orgigin
    WrappedNode::TransferOrigin getTransferOrigin()
    {
        return mTransfersFrom;
    }

    // Get the wrapped MEGA node pointer
    mega::MegaNode* getMegaNode()
    {
        return mNode;
    }

private:
    // Keep track of transfer origin
    WrappedNode::TransferOrigin  mTransfersFrom;

    // Wrapped MEGA node
    mega::MegaNode* mNode;
};

#endif // UTILITIES_H
