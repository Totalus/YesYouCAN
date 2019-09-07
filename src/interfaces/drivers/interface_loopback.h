#ifndef INTERFACE_LOOPBACK_H
#define INTERFACE_LOOPBACK_H


#include "interface_hwinterface.h"

class LoopbackInterface : public HwInterface
{
	Q_OBJECT

	public:
		LoopbackInterface(QObject *parent = 0, QString identifier = QString());

		// Device management
		virtual bool initialize();				// Initialize device (load drivers, etc.), returns true on success
		virtual void deinitialize();			// Deinitialize the device (no more connection or communication afterwards)
		virtual bool setCapture(bool enable);	// Enable/disable capturing packets on this device (hardware), returns true on success
		virtual bool isCapturing();				// Returns true if the device is capturing data

		// Device information
		virtual InterfaceType_t interfaceType();	// Returns the interface type
		virtual QString interfaceTypeStr();			// Returns the interface type as a string
		virtual QString getDescription();			// Returns a description string (type, address, port, ...)
		virtual quint32 getMessageCount();			// Number of messages received since capture was enabled
		virtual QString getErrorStr();				// Last error
		//virtual QList<QString> getSupportedBitrates() = 0;	// Returns a list of supported bitrates (in bits/s)

		void writeMessage(CanTraceMessage &msg);


	private:
		quint32 m_msg_count;

};

#endif // INTERFACE_LOOPBACK_H
