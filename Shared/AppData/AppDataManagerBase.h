#ifndef TCS_DATA_APPDATAMANAGERBASE_H
#define TCS_DATA_APPDATAMANAGERBASE_H

#include <QObject>
#include <QDataStream>

namespace TCS {
namespace Data {

struct Data {
    explicit Data(const QString& filename);
    QString filename() const;
	virtual ~Data(){}
    virtual void load(QDataStream& s) = 0;
    virtual void save(QDataStream& s) = 0;

private:
    QString _filename;
};

class AppDataManagerBase : public QObject
{
    Q_OBJECT
public:
    explicit AppDataManagerBase(Data* data, QObject *parent = 0);
    virtual ~AppDataManagerBase();

    bool load();
    bool save();

signals:
    void settingsLoaded();
    void settingsSaved();

protected:
    Data* _data;
};

} // Data
} // TCS
#endif // TCS_DATA_APPDATAMANAGERBASE_H
