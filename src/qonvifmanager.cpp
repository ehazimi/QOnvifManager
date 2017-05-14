#include "qonvifmanager.hpp"
#include "devicemanagement.h"
#include "devicesearcher.h"
#include "systemdateandtime.h"

class QOnvifManagerPrivate
{
public:
    QOnvifManagerPrivate(const QString _username, const QString _password)
        : iuserName(_username), ipassword(_password) {}
    ~QOnvifManagerPrivate() {}

    QScopedPointer<QOnvifManagerPrivate> d_ptr;
    QString                              iuserName;
    QString                              ipassword;
    QMap<QString, QOnvifDevice*> idevicesMap;
    QHostAddress           ihostAddress;
    ONVIF::DeviceSearcher* ideviceSearcher;
};

QOnvifManager::QOnvifManager(
    const QString _username, const QString _password, QObject* _parent)
    : QObject(_parent), d_ptr(new QOnvifManagerPrivate(_username, _password)) {
    Q_D(QOnvifManager);
    // device finding
    d->ideviceSearcher = ONVIF::DeviceSearcher::instance(d->ihostAddress);

    // when one device finded
    connect(
        d->ideviceSearcher,
        SIGNAL(receiveData(QHash<QString, QString>)),
        this,
        SLOT(onReciveData(QHash<QString, QString>)),
        Qt::UniqueConnection);

    // when device searching ended
    connect(
        d->ideviceSearcher,
        &ONVIF::DeviceSearcher::deviceSearchingEnded,
        [this]() { emit deviceSearchingEnded(); });
    refreshDevicesList();
}

QOnvifManager::~QOnvifManager() {}

bool
QOnvifManager::refreshDevicesList() {
    Q_D(QOnvifManager);
    qDeleteAll(d->idevicesMap);
    d->idevicesMap.clear();
    d->ideviceSearcher->sendSearchMsg();
    return true;
}

bool
QOnvifManager::refreshDeviceCapabilities(QString _deviceEndPointAddress) {
    if (!cameraExist(_deviceEndPointAddress))
        return false;
    return d_ptr->idevicesMap.value(_deviceEndPointAddress)
        ->refreshDeviceCapabilities();
}

bool
QOnvifManager::refreshDeviceInformations(QString _deviceEndPointAddress) {
    if (!cameraExist(_deviceEndPointAddress))
        return false;
    return d_ptr->idevicesMap.value(_deviceEndPointAddress)
        ->refreshDeviceInformation();
}

bool
QOnvifManager::refreshDeviceScopes(QString _deviceEndPointAddress) {
    if (!cameraExist(_deviceEndPointAddress))
        return false;
    return d_ptr->idevicesMap.value(_deviceEndPointAddress)
        ->refreshDeviceScopes();
}

bool
QOnvifManager::refreshDeviceVideoConfigs(QString _deviceEndPointAddress) {
    if (!cameraExist(_deviceEndPointAddress))
        return false;
    return d_ptr->idevicesMap.value(_deviceEndPointAddress)
        ->refreshVideoConfigs();
}

bool
QOnvifManager::refreshDeviceStreamUris(QString _deviceEndPointAddress) {
    if (!cameraExist(_deviceEndPointAddress))
        return false;
    return d_ptr->idevicesMap.value(_deviceEndPointAddress)
        ->refreshStreamUris();
}

bool
QOnvifManager::refreshDeviceVideoConfigsOptions(
    QString _deviceEndPointAddress) {
    if (!cameraExist(_deviceEndPointAddress))
        return false;
    return d_ptr->idevicesMap.value(_deviceEndPointAddress)
        ->refreshVideoConfigsOptions();
}

bool
QOnvifManager::refreshDeviceProfiles(QString _deviceEndPointAddress) {
    if (!cameraExist(_deviceEndPointAddress))
        return false;
    return d_ptr->idevicesMap.value(_deviceEndPointAddress)->refreshProfiles();
}

bool
QOnvifManager::refreshDeviceInterfaces(QString _deviceEndPointAddress) {
    if (!cameraExist(_deviceEndPointAddress))
        return false;
    return d_ptr->idevicesMap.value(_deviceEndPointAddress)
        ->refreshInterfaces();
}

bool
QOnvifManager::refreshDeviceProtocols(QString _deviceEndPointAddress) {
    if (!cameraExist(_deviceEndPointAddress))
        return false;
    return d_ptr->idevicesMap.value(_deviceEndPointAddress)->refreshProtocols();
}

bool
QOnvifManager::refreshDeviceUsers(QString _deviceEndPointAddress) {
    if (!cameraExist(_deviceEndPointAddress))
        return false;
    return d_ptr->idevicesMap.value(_deviceEndPointAddress)->refreshUsers();
}

bool
QOnvifManager::deviceDateAndTime(
    QString _deviceEndPointAddress, Data::DateTime& _datetime) {
    if (!cameraExist(_deviceEndPointAddress))
        return false;
    return d_ptr->idevicesMap.value(_deviceEndPointAddress)
        ->deviceDateAndTime(_datetime);
}

QOnvifDevice*
QOnvifManager::device(QString _deviceEndPointAddress) {
    return d_ptr->idevicesMap.value(_deviceEndPointAddress);
}

QMap<QString, QOnvifDevice*>&
QOnvifManager::devicesMap() {
    return d_ptr->idevicesMap;
}

bool
QOnvifManager::cameraExist(const QString& endpoinAddress) {
    if (d_ptr->idevicesMap.contains(endpoinAddress))
        return true;
    return false;
}

bool
QOnvifManager::setDeviceDateAndTime(
    QString _deviceEndPointAddress, QDateTime _dateTime) {
    return d_ptr->idevicesMap.value(_deviceEndPointAddress)
        ->setDateAndTime(_dateTime);
}

void
QOnvifManager::setDefaulUsernameAndPassword(
    QString _username, QString _password) {
    d_ptr->iuserName = _username;
    d_ptr->ipassword = _password;
    refreshDevicesList();
}

bool
QOnvifManager::setDeviceScopes(
    QString _deviceEndPointAddress, QString _name, QString _location) {
    return d_ptr->idevicesMap.value(_deviceEndPointAddress)
        ->setScopes(_name, _location);
}

bool
QOnvifManager::setDeviceVideoConfig(
    QString                                 _deviceEndPointAddress,
    Data::MediaConfig::Video::EncoderConfig _videoConfig) {
    return d_ptr->idevicesMap.value(_deviceEndPointAddress)
        ->setVideoConfig(_videoConfig);
}

bool
QOnvifManager::setDeviceNetworkInterfaces(
    QString _deviceEndPointAddress, Data::Network::Interfaces _interfaces) {
    return d_ptr->idevicesMap.value(_deviceEndPointAddress)
        ->setInterfaces(_interfaces);
}

bool
QOnvifManager::setDeviceNetworkProtocols(
    QString _deviceEndPointAddress, Data::Network::Protocols _protocols) {
    return d_ptr->idevicesMap.value(_deviceEndPointAddress)
        ->setProtocols(_protocols);
}

bool
QOnvifManager::refreshDevicePtzConfigs(QString _deviceEndPointAddress) {
    return d_ptr->idevicesMap.value(_deviceEndPointAddress)
        ->refreshPtzConfiguration();
}

bool
QOnvifManager::resetFactoryDevice(QString _deviceEndPointAddress) {
    return d_ptr->idevicesMap.value(_deviceEndPointAddress)
        ->resetFactoryDevice();
}

bool
QOnvifManager::rebootDevice(QString _deviceEndPointAddress) {
    return d_ptr->idevicesMap.value(_deviceEndPointAddress)->rebootDevice();
}

bool
QOnvifManager::continuousMove(
    QString     _deviceEndPointAddress,
    const float _x,
    const float _y,
    const float _z) {
    return d_ptr->idevicesMap.value(_deviceEndPointAddress)
        ->continuousMove(_x, _y, _z);
}

bool
QOnvifManager::stopMovement(QString _deviceEndPointAddress) {
    return d_ptr->idevicesMap.value(_deviceEndPointAddress)->stopMovement();
}

void
QOnvifManager::onReciveData(QHash<QString, QString> _deviceHash) {
    Q_D(QOnvifManager);

    Data::ProbeData probeData;
    probeData.endPointAddress = _deviceHash.value("ep_address");

    if (d->idevicesMap.contains(probeData.endPointAddress))
        return;

    probeData.types    = _deviceHash.value("types");
    probeData.deviceIp = _deviceHash.value("device_ip");
    probeData.deviceServiceAddress =
        _deviceHash.value("device_service_address");
    probeData.scopes          = _deviceHash.value("scopes");
    probeData.metadataVersion = _deviceHash.value("metadata_version");

    QOnvifDevice* device = new QOnvifDevice(
        probeData.deviceServiceAddress, d->iuserName, d->ipassword, this);
    device->setDeviceProbeData(probeData);
    d->idevicesMap.insert(probeData.endPointAddress, device);
    emit newDeviceFinded(device);
}
