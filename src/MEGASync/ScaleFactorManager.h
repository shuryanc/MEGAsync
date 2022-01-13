#pragma once
#include <vector>
#include <memory>
#include <string>

struct ScreenInfo
{
    std::string name;
    int availableWidthPixels;
    int availableHeightPixels;
    double dotsPerInch;
    double devicePixelRatio;

    std::string toString() const
    {
        return name + ", " + std::to_string(availableWidthPixels) + ", " + std::to_string(availableHeightPixels)
                + ", " + std::to_string(dotsPerInch) + ", " + std::to_string(devicePixelRatio);
    }
};

enum class OsType{LINUX=0, WIN, MACOS};

using ScreensInfo = std::vector<ScreenInfo>;

class ScaleFactorManager
{
public:
    ScaleFactorManager(OsType osType);
    ScaleFactorManager(OsType osType, ScreensInfo screensInfo, std::string osName, std::string desktopName);
    void setScaleFactorEnvironmentVariable();
    std::vector<std::string> getLogMessages() const;

private:
    OsType mOsType;
    std::string mOsName;
    ScreensInfo mScreensInfo;
    mutable std::vector<std::string> mLogMessages;
    std::vector<double> mCalculatedScales;
    std::string mDesktopName;

    bool checkEnvironmentVariables() const;
    bool computeScales();
    double computeScaleLinux(const ScreenInfo& screenInfo) const;
};
