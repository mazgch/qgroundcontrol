/*=====================================================================
 
 QGroundControl Open Source Ground Control Station
 
 (c) 2009 - 2014 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 
 This file is part of the QGROUNDCONTROL project
 
 QGROUNDCONTROL is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 QGROUNDCONTROL is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with QGROUNDCONTROL. If not, see <http://www.gnu.org/licenses/>.
 
 ======================================================================*/

/// @file
///     @author Don Gagne <don@thegagnes.com>

#ifndef APMFirmwarePlugin_H
#define APMFirmwarePlugin_H

#include "FirmwarePlugin.h"
#include "QGCLoggingCategory.h"
#include "APMParameterMetaData.h"

Q_DECLARE_LOGGING_CATEGORY(APMFirmwarePluginLog)

class APMFirmwareVersion
{
public:
    APMFirmwareVersion(const QString &versionText = "");
    bool isValid() const;
    bool isBeta() const;
    bool isDev() const;
    bool operator<(const APMFirmwareVersion& other) const;
    QString versionString() const { return _versionString; }
    QString vehicleType() const { return _vehicleType; }
    int majorNumber() const { return _major; }
    int minorNumber() const { return _minor; }
    int patchNumber() const { return _patch; }

private:
    void _parseVersion(const QString &versionText);
    QString _versionString;
    QString _vehicleType;
    int     _major;
    int     _minor;
    int     _patch;
};

class APMCustomMode
{
public:
    APMCustomMode(uint32_t mode, bool settable);
    uint32_t modeAsInt() const { return _mode; }
    bool canBeSet() const { return _settable; }
    QString modeString() const;

protected:
    void setEnumToStringMapping(const QMap<uint32_t,QString>& enumToString);

private:
    uint32_t               _mode;
    bool                   _settable;
    QMap<uint32_t,QString> _enumToString;
};

/// This is the base class for all stack specific APM firmware plugins
class APMFirmwarePlugin : public FirmwarePlugin
{
    Q_OBJECT
    
public:
    // Overrides from FirmwarePlugin

    QList<VehicleComponent*> componentsForVehicle(AutoPilotPlugin* vehicle) final;
    QList<MAV_CMD> supportedMissionCommands(void) final;

    bool        isCapable(FirmwareCapabilities capabilities);
    QStringList flightModes(void) final;
    QString     flightMode(uint8_t base_mode, uint32_t custom_mode) const final;
    bool        setFlightMode(const QString& flightMode, uint8_t* base_mode, uint32_t* custom_mode) final;
    bool        isGuidedMode(const Vehicle* vehicle) const final;
    void        pauseVehicle(Vehicle* vehicle);
    int         manualControlReservedButtonCount(void) final;
    void        adjustIncomingMavlinkMessage(Vehicle* vehicle, mavlink_message_t* message) final;
    void        adjustOutgoingMavlinkMessage(Vehicle* vehicle, mavlink_message_t* message) final;
    void        initializeVehicle(Vehicle* vehicle) final;
    bool        sendHomePositionToVehicle(void) final;
    void        addMetaDataToFact(QObject* parameterMetaData, Fact* fact, MAV_TYPE vehicleType) final;
    QString     getDefaultComponentIdParam(void) const final { return QString("SYSID_SW_TYPE"); }
    void        missionCommandOverrides(QString& commonJsonFilename, QString& fixedWingJsonFilename, QString& multiRotorJsonFilename) const final;
    QString     getVersionParam(void) final { return QStringLiteral("SYSID_SW_MREV"); }
    QString     internalParameterMetaDataFile   (void) final { return QString(":/FirmwarePlugin/APM/APMParameterFactMetaData.xml"); }
    void        getParameterMetaDataVersionInfo (const QString& metaDataFile, int& majorVersion, int& minorVersion) final { APMParameterMetaData::getParameterMetaDataVersionInfo(metaDataFile, majorVersion, minorVersion); }
    QObject*    loadParameterMetaData           (const QString& metaDataFile);

protected:
    /// All access to singleton is through stack specific implementation
    APMFirmwarePlugin(void);
    void setSupportedModes(QList<APMCustomMode> supportedModes);
    
private:
    void _adjustSeverity(mavlink_message_t* message) const;
    void _adjustCalibrationMessageSeverity(mavlink_message_t* message) const;
    static bool _isTextSeverityAdjustmentNeeded(const APMFirmwareVersion& firmwareVersion);
    void _setInfoSeverity(mavlink_message_t* message) const;
    QString _getMessageText(mavlink_message_t* message) const;
    void _handleParamValue(Vehicle* vehicle, mavlink_message_t* message);
    void _handleParamSet(Vehicle* vehicle, mavlink_message_t* message);
    void _handleStatusText(Vehicle* vehicle, mavlink_message_t* message);
    void _handleHeartbeat(Vehicle* vehicle, mavlink_message_t* message);

    APMFirmwareVersion      _firmwareVersion;
    bool                    _textSeverityAdjustmentNeeded;
    QList<APMCustomMode>    _supportedModes;
};

#endif
