#ifndef STALLEDISSUE_H
#define STALLEDISSUE_H

#include <FileFolderAttributes.h>

#include <megaapi.h>

#include <QSharedData>
#include <QObject>
#include <QFileInfo>
#include <QSize>
#include <QDebug>

#include <memory>

enum class StalledIssueFilterCriterion
{
    ALL_ISSUES = 0,
    NAME_CONFLICTS,
    ITEM_TYPE_CONFLICTS,
    OTHER_CONFLICTS,
};

class StalledIssueData : public QSharedData
{
public:
    struct Path
    {
        QString path;
        mega::MegaSyncStall::SyncPathProblem mPathProblem = mega::MegaSyncStall::SyncPathProblem::NoProblem;

        Path(){}
        bool isEmpty() const {return path.isEmpty() && mPathProblem == mega::MegaSyncStall::SyncPathProblem::NoProblem;}
    };

    StalledIssueData(std::unique_ptr<mega::MegaSyncStall> originalstall);
    StalledIssueData(const StalledIssueData&);
    StalledIssueData();
    virtual ~StalledIssueData() = default;

    const Path& getPath() const;
    const Path& getMovePath() const;
    virtual bool isCloud() const {return false;}

    QString getFilePath() const;
    QString getMoveFilePath() const;

    QString getNativeFilePath() const;
    QString getNativeMoveFilePath() const;

    QString getNativePath() const;
    QString getNativeMovePath() const;

    QString getFileName() const;

    const std::shared_ptr<const FileFolderAttributes> getAttributes() const {return mAttributes;}
    std::shared_ptr<FileFolderAttributes> getAttributes() {return mAttributes;}

    virtual bool isEqual(const mega::MegaSyncStall*) const {return false;}

    void checkTrailingSpaces(QString& name) const;

    std::shared_ptr<mega::MegaSyncStall> original;

    template <class Type>
    QExplicitlySharedDataPointer<const Type> convert() const
    {
        return QExplicitlySharedDataPointer<const Type>(dynamic_cast<const Type*>(this));
    }

    virtual void initFileFolderAttributes()
    {}

protected:
    friend class StalledIssue;
    friend class NameConflictedStalledIssue;

    Path mMovePath;
    Path mPath;

    std::shared_ptr<FileFolderAttributes> mAttributes;
};

Q_DECLARE_TYPEINFO(StalledIssueData, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(StalledIssueData)

using StalledIssueDataPtr = QExplicitlySharedDataPointer<const StalledIssueData>;
using StalledIssuesDataList = QList<StalledIssueDataPtr>;

Q_DECLARE_METATYPE(StalledIssueDataPtr)
Q_DECLARE_METATYPE(StalledIssuesDataList)

//CLOUD DATA
class CloudStalledIssueData : public StalledIssueData
{
public:

    CloudStalledIssueData(std::unique_ptr<mega::MegaSyncStall> originalstall)
        :StalledIssueData(std::move(originalstall)),
          mPathHandle(mega::INVALID_HANDLE),
          mMovePathHandle(mega::INVALID_HANDLE)
    {}

    CloudStalledIssueData(const CloudStalledIssueData& data)
        :StalledIssueData(data),
         mPathHandle(data.mPathHandle),
         mMovePathHandle(data.mMovePathHandle)
    {}

    CloudStalledIssueData()
        : StalledIssueData(),
          mPathHandle(mega::INVALID_HANDLE),
          mMovePathHandle(mega::INVALID_HANDLE)
    {}

    ~CloudStalledIssueData(){}

    bool isCloud() const override
    {
        return true;
    }

    std::shared_ptr<mega::MegaNode> getNode() const;

    bool isEqual(const mega::MegaSyncStall *stall) const override;

    void initFileFolderAttributes() override
    {
        if(mPathHandle != mega::INVALID_HANDLE)
        {
            mAttributes = std::make_shared<RemoteFileFolderAttributes>(mPathHandle, nullptr);
        }
        else
        {
            mAttributes = std::make_shared<RemoteFileFolderAttributes>(mPath.path, nullptr);
        }
    }

    std::shared_ptr<RemoteFileFolderAttributes> getFileFolderAttributes() const
    {
        return std::dynamic_pointer_cast<RemoteFileFolderAttributes>(mAttributes);
    }

    mega::MegaHandle getPathHandle() const;
    mega::MegaHandle getMovePathHandle() const;

private:
    friend class StalledIssue;
    friend class NameConflictedStalledIssue;

    mutable std::shared_ptr<mega::MegaNode> mRemoteNode;

    mega::MegaHandle mPathHandle;
    mega::MegaHandle mMovePathHandle;
};

Q_DECLARE_TYPEINFO(CloudStalledIssueData, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(CloudStalledIssueData)

using CloudStalledIssueDataPtr = QExplicitlySharedDataPointer<const CloudStalledIssueData>;
using CloudStalledIssuesDataList = QList<CloudStalledIssueDataPtr>;

Q_DECLARE_METATYPE(CloudStalledIssueDataPtr)
Q_DECLARE_METATYPE(CloudStalledIssuesDataList)

//LOCAL DATA
class LocalStalledIssueData : public StalledIssueData
{
public:

    LocalStalledIssueData(std::unique_ptr<mega::MegaSyncStall> originalstall)
        :StalledIssueData(std::move(originalstall))
    {

    }
    LocalStalledIssueData(const CloudStalledIssueData& data)
        :StalledIssueData(data)
    {

    }
    LocalStalledIssueData()
        : StalledIssueData()
    {

    }
    ~LocalStalledIssueData(){}

    bool isCloud() const override
    {
        return false;
    }

    bool isEqual(const mega::MegaSyncStall *stall) const override;

    void initFileFolderAttributes() override
    {
        mAttributes = std::make_shared<LocalFileFolderAttributes>(mPath.path, nullptr);
    }

    std::shared_ptr<LocalFileFolderAttributes> getFileFolderAttributes() const
    {
        return std::dynamic_pointer_cast<LocalFileFolderAttributes>(mAttributes);
    }
};

Q_DECLARE_TYPEINFO(LocalStalledIssueData, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(LocalStalledIssueData)

using LocalStalledIssueDataPtr = QExplicitlySharedDataPointer<const LocalStalledIssueData>;
using LocalStalledIssueDataList = QList<LocalStalledIssueDataPtr>;

Q_DECLARE_METATYPE(LocalStalledIssueDataPtr)
Q_DECLARE_METATYPE(LocalStalledIssueDataList)

class StalledIssue
{
public:
    StalledIssue(){}
    //StalledIssue(const StalledIssue& tdr) : mDetectedMEGASide(tdr.mDetectedMEGASide), mLocalData(tdr.mLocalData), mCloudData(tdr.mCloudData), mReason(tdr.getReason()), mIsSolved(tdr.mIsSolved)  {}
    StalledIssue(const mega::MegaSyncStall *stallIssue);

    const LocalStalledIssueDataPtr consultLocalData() const;
    const CloudStalledIssueDataPtr consultCloudData() const;

    mega::MegaSyncStall::SyncStallReason getReason() const;
    QString getFileName(bool preferCloud) const;
    static StalledIssueFilterCriterion getCriterionByReason(mega::MegaSyncStall::SyncStallReason reason);

    bool operator==(const StalledIssue &data);

    virtual void updateIssue(const mega::MegaSyncStall *stallIssue);

    bool isSolved() const;
    void setIsSolved();
    virtual void autoSolveIssue(){}

    bool canBeIgnored() const;
    QStringList getIgnoredFiles() const;

    bool mDetectedMEGASide = false;

    bool isFile() const;
    uint8_t hasFiles() const;
    uint8_t hasFolders() const;

    enum SizeType
    {
        Header = 0,
        Body
    };

    QSize getDelegateSize(SizeType type) const;
    void setDelegateSize(const QSize &newDelegateSize, SizeType type);
    void removeDelegateSize(SizeType type);

    const std::shared_ptr<mega::MegaSyncStall> &getOriginalStall() const;

    virtual void fillIssue(const mega::MegaSyncStall *stall);
    virtual void endFillingIssue();

protected:
    bool initLocalIssue(const mega::MegaSyncStall *stallIssue);
    const QExplicitlySharedDataPointer<LocalStalledIssueData>& getLocalData() const;
    QExplicitlySharedDataPointer<LocalStalledIssueData> mLocalData;

    bool initCloudIssue(const mega::MegaSyncStall *stallIssue);
    const QExplicitlySharedDataPointer<CloudStalledIssueData>& getCloudData() const;
    QExplicitlySharedDataPointer<CloudStalledIssueData> mCloudData;

    void setIsFile(const QString& path, bool isLocal);

    std::shared_ptr<mega::MegaSyncStall> originalStall;
    mega::MegaSyncStall::SyncStallReason mReason = mega::MegaSyncStall::SyncStallReason::NoReason;
    mutable bool mIsSolved = false;
    uint8_t mFiles = 0;
    uint8_t mFolders = 0;
    QStringList mIgnoredPaths;
    QSize mHeaderDelegateSize;
    QSize mBodyDelegateSize;
};

Q_DECLARE_METATYPE(StalledIssue)

class StalledIssueVariant
{
public:
    StalledIssueVariant(){}
    StalledIssueVariant(const StalledIssueVariant& tdr) : mData(tdr.mData) {}
    StalledIssueVariant(const std::shared_ptr<StalledIssue> data)
        : mData(data)
    {}

    const std::shared_ptr<const StalledIssue> consultData() const
    {
        return mData;
    }

    void updateData(const mega::MegaSyncStall *stallIssue)
    {
        mData->updateIssue(stallIssue);
    }

    void reset()
    {
        mData.reset();
    }

    bool operator==(const StalledIssueVariant &issue)
    {
        return issue.mData == this->mData;
    }

    StalledIssueVariant& operator=(const StalledIssueVariant& other) = default;

    QSize getDelegateSize(StalledIssue::SizeType type) const
    {
        return mData->getDelegateSize(type);
    }
    void setDelegateSize(const QSize &newDelegateSize, StalledIssue::SizeType type)
    {
        mData->setDelegateSize(newDelegateSize, type);
    }
    void removeDelegateSize(StalledIssue::SizeType type)
    {
        mData->removeDelegateSize(type);
    }

    template <class Type>
    const std::shared_ptr<const Type> convert() const
    {
        return std::dynamic_pointer_cast<Type>(mData);
    }

private:
    friend class StalledIssuesModel;
    friend class StalledIssuesReceiver;

    const std::shared_ptr<StalledIssue> &getData() const
    {
        return mData;
    }

    template <class Type>
    std::shared_ptr<Type> convert()
    {
        return std::dynamic_pointer_cast<Type>(mData);
    }

   std::shared_ptr<StalledIssue> mData;
};

Q_DECLARE_METATYPE(StalledIssueVariant)

using StalledIssuesVariantList = QList<std::shared_ptr<StalledIssueVariant>>;
Q_DECLARE_METATYPE(StalledIssuesVariantList)

#endif // STALLEDISSUE_H
