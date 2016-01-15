#include "AppSettingsManager.h"
#include <QCoreApplication>
#include <QFile>
#include "Utils/Utils.h"

namespace TCS {
namespace Data {

AppData::AppData(QObject* p) : JObject(p),  Data(TCS::Utils::dataDir("appData.bin")),
    _removeItemWhenDone (false), _replaceSourceFile(true), _startsOptimizingWhenDrop(true), _fileFormat("_opt"),
    _apiUrl("https://api.tinify.com/shrink"), _apiKey("pjOaZ7vpi3jX-loMkErLvt_VIY2Phxv_")
{
}


void AppData::load(QDataStream& s)
{
    QByteArray jd;
    s >> jd;
    importFromJson(jd);
}


void AppData::save(QDataStream& s)
{
    s << exportToJson();
}


static AppDataSettingsManager* API = 0;
AppDataSettingsManager* AppDataSettingsManager::me()
{
    if (API == 0) {
        API = new AppDataSettingsManager(QCoreApplication::instance());
    }
    return API;
}
AppDataSettingsManager::AppDataSettingsManager(QObject*  p) : AppDataManagerBase(new AppData(), p){}

AppData* AppDataSettingsManager::data() {return (AppData*)_data;}


} // Data
} // TCS
