#include "AppDataManagerBase.h"
#include <QFile>
#include <QDebug>

namespace TCS {
namespace Data {

Data::Data(const QString &filename) : _filename(filename){}
QString Data::filename() const {return _filename;}

AppDataManagerBase::AppDataManagerBase(Data* data, QObject *parent) :
    QObject(parent), _data(data) {load();}

AppDataManagerBase::~AppDataManagerBase() {delete _data;}

bool AppDataManagerBase::load()
{
    QFile file(_data->filename());
    if (file.exists()) {
        if (file.open(QIODevice::ReadOnly)) {
            QDataStream in(&file);
            _data->load(in);
            file.close();
            emit settingsLoaded();
            return true;
        }
        else {
            qCritical() << "AppDataManagerBase::load()# Failed to load file " << _data->filename() << ". Error - " << file.errorString();
        }
    }
    else {
        save();
    }
    return false;
}


bool AppDataManagerBase::save()
{
    QFile file(_data->filename());
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        _data->save(out);
        file.close();
        emit settingsSaved();
        return true;
    }else {
        qCritical() << "AppDataManagerBase::load()# Failed to save file " << _data->filename() << ". Error - " << file.errorString();
    }
    return false;
}


} // Data
} // TCS
