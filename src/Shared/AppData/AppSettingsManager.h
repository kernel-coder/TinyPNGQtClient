#ifndef TCS_DATA_APPFACTORYINFOMANAGER_H
#define TCS_DATA_APPFACTORYINFOMANAGER_H

#include "AppDataManagerBase.h"
#include <QVariant>
#include <QDateTime>
#include "JObject.h"

#define ADSM TCS::Data::AppDataSettingsManager::me()

class QWidget;

namespace TCS {
namespace Data {

class AppData : public JObject, public Data {
    Q_OBJECT
  public:
      Q_INVOKABLE explicit AppData(QObject* parent = 0);
      MetaPropertyPublicSet(QString, apiUrl)
      MetaPropertyPublicSet(QString, apiKey)
      MetaPropertyPublicSet(bool, removeItemWhenDone)
      MetaPropertyPublicSet(bool, replaceSourceFile)
      MetaPropertyPublicSet(bool, startsOptimizingWhenDrop)
      MetaPropertyPublicSet(QString, fileFormat)
      MetaPropertyPublicSet(bool, removeErrorItems)

      bool isValid() const;


      void load(QDataStream& s);
      void save(QDataStream& s);
};


class AppDataSettingsManager : public AppDataManagerBase
{
    Q_OBJECT
    explicit AppDataSettingsManager(QObject *parent = 0);
public:
    static AppDataSettingsManager* me();
    AppData* data();

};

} // Data
} // TCS
#endif // TCS_DATA_APPFACTORYINFOMANAGER_H
