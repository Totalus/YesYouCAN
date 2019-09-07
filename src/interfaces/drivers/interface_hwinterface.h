#ifndef HWINTERFACE_H
#define HWINTERFACE_H

#include <QtCore>
#include "struct_canmessage.h"
#include "model_cantrace.h"
//#include "document.h"

class Document;

typedef enum
{
	INITIALIZED,	// Device initialized successfuly
	OFFLINE,		// Not initialized
	UNAVAILABLE		// Can't initialize device, not responding
} InterfaceState_t;

typedef enum
{
	INTERFACE_UNKNOWN,
	INTERFACE_WCAN,
	INTERFACE_PCAN,
	INTERFACE_LOOPBACK
} InterfaceType_t;


class HwInterface : public QObject
{
	Q_OBJECT

	public:
		HwInterface(QObject *parent = 0, QString identifier = QString());

		//
		//	Virtual pure functions
		//

		// Device management
		virtual bool initialize() = 0;				// Initialize device (load drivers, etc.), returns true on success
		virtual void deinitialize() = 0;			// Deinitialize the device (no more connection or communication afterwards)
		virtual bool setCapture(bool enable) = 0;	// Enable/disable capturing packets on this device (hardware), returns true on success
		virtual bool isCapturing() = 0;				// Returns true if the device is capturing data

		// Device information
		virtual InterfaceType_t interfaceType() = 0;	// Returns the interface type
		virtual QString interfaceTypeStr() = 0;			// Returns the interface type as a string
		virtual QString getDescription() = 0;			// Returns a description string (type, address, port, ...)
		virtual quint32 getMessageCount() = 0;			// Number of messages received since capture was enabled
		virtual QString getErrorStr() = 0;				// Last error
		//virtual QList<QString> getSupportedBitrates() = 0;	// Returns a list of supported bitrates (in bits/s)

		//
		//	Virtual functions
		//

		virtual bool isAvailable();			// Check availability (device is responding, seems online)


		//
		//	Other Functions
		//

		// Device information
		InterfaceState_t getState();
		int getRxRate();					// Returns the number of messages received per second
		bool isActive();					// Returns true if the device is active
		QStringList getCapabilities();		// Returns the capabilities of the device

		// Parameters modification and reading
		void setIdentifier(QString id);
		QString getIdentifier();
		Document* getDocument();

		void setName(QString name);
		QString getName();

	public slots:
		virtual void writeMessage(CanTraceMessage &msg);	// Sends a message on the bus

	signals:
		void newCanMessage(CanTraceMessage);

	protected:
		QString m_identifier;		// Unique identifier
		QString m_name;
		int m_rx_rate;				// Messages received / seconds
		bool m_active;				// true Active means the interface is capturing data

		QStringList m_capabilities; // "read","write"
		InterfaceState_t m_state;

		Document* m_document;	// Document of this object
};


#endif // HWINTERFACE_H
