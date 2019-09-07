#include "pcandriver.h"

PCANDriver::PCANDriver(QObject *parent) : QObject(parent)
{
	m_lib.setFileName("PCANBasic");
	m_state = Unloaded;
}

PCANDriver::~PCANDriver()
{
	if(m_state != Unloaded)
		unload();
}


void PCANDriver::resetPointers()
{
	m_pInitialize     = 0;
	m_pUnInitialize   = 0;
	m_pReset          = 0;
	m_pGetStatus      = 0;
	m_pRead           = 0;
	m_pWrite          = 0;
	m_pGetValue       = 0;
	m_pSetValue       = 0;
	m_pGetTextError   = 0;
}

QString PCANDriver::getStatusStr()
{
	switch(m_state)
	{
		case Ready:
			return "The PCANBasic library is loaded and ready";

		case LibraryNotFound:
			return "PCANBasic library not found";

		case FunctionsNotFound:
			return "Functions of the PCANBasic library were not found";

		case Unloaded:
			return "PCANBasic library is not loaded";

	}

	return "Unknown state";
}

LibState_t PCANDriver::getStatus()
{
	return m_state;
}


void PCANDriver::load()
{
	// Si la bibliothèque est déjà chargé, on ne fait rien
	if(m_lib.isLoaded())
	{
		return;
	}

	// Tentative de chargement suivit d'une réccupération des pointeurs sur les différentes fonctions de la bibliothèque
	if(m_lib.load())
	{
		m_pInitialize     = (fpInitialize)     m_lib.resolve("CAN_Initialize");
		m_pUnInitialize   = (fpUninitialize)   m_lib.resolve("CAN_Uninitialize");
		m_pReset          = (fpReset)          m_lib.resolve("CAN_Reset");
		m_pGetStatus      = (fpGetStatus)      m_lib.resolve("CAN_GetStatus");
		m_pRead           = (fpRead)           m_lib.resolve("CAN_Read");
		m_pWrite          = (fpWrite)          m_lib.resolve("CAN_Write");
		m_pGetValue       = (fpGetValue)       m_lib.resolve("CAN_GetValue");
		m_pSetValue       = (fpSetValue)       m_lib.resolve("CAN_SetValue");
		m_pGetTextError   = (fpGetErrorText)   m_lib.resolve("CAN_GetErrorText");
		m_pFilterMessages = (fpFilterMessages) m_lib.resolve("CAN_FilterMessages");
		m_pReadFD		  = (fpReadFD)		   m_lib.resolve("CAN_ReadFD");
		m_pWriteFD		  = (fpWriteFD)		   m_lib.resolve("CAN_WriteFD");
		m_pInitializeFD	  = (fpInitializeFD)   m_lib.resolve("CAN_InitializeFD");


		// Si on a tout réccupéré, alors on entre dans l'état ReadyToRun et sinon dans FunctionsNotFound
		m_state =	m_pInitialize &&
					m_pUnInitialize &&
					m_pReset &&
					m_pGetStatus &&
					m_pRead &&
					m_pWrite &&
					m_pGetValue &&
					m_pSetValue &&
					m_pGetTextError &&
					m_pFilterMessages &&
					m_pReadFD &&
					m_pWriteFD &&
					m_pInitializeFD
					? Ready : FunctionsNotFound;

		// Si on a pas trouvé toutes les fonctions, on décharge la bibliothèque (sans appeler unload() car cela chargerait l'état)
		if(m_state != Ready) {
			m_lib.unload();
		}

	}
	else
	{
		m_state = LibraryNotFound;
	}

	// Petit message de débogage indiquant l'état
	/*
	if(m_state == Ready) {
		qDebug() << "PCANBasicLoader loaded the library successfully";
	}
	else {
		qCritical() << "PCANBasicLoader failed to load the library";
		qDebug() << "PCANBasicLoader state error :" << getStateText();
	}//*/
}

void PCANDriver::unload()
{
	resetPointers();

	if(m_lib.isLoaded())
		m_lib.unload();

	m_state = Unloaded;
}



//
//	Library functions
//


TPCANStatus PCANDriver::Initialize(TPCANHandle Channel, TPCANBaudrate Btr0Btr1, TPCANType HwType, DWORD IOPort, WORD Interrupt)
{
	if(m_state != Ready)
		return PCAN_ERROR_UNKNOWN;

	return (TPCANStatus)m_pInitialize(Channel, Btr0Btr1, HwType, IOPort, Interrupt);
}

TPCANStatus PCANDriver::InitializeFD(TPCANHandle Channel, TPCANBitrateFD BitrateFD)
{
	if(m_state != Ready)
		return PCAN_ERROR_UNKNOWN;

	return (TPCANStatus)m_pInitializeFD(Channel, BitrateFD);
}

TPCANStatus PCANDriver::Uninitialize(TPCANHandle Channel)
{
	if(m_state != Ready)
		return PCAN_ERROR_UNKNOWN;

	return (TPCANStatus)m_pUnInitialize(Channel);
}

TPCANStatus PCANDriver::Reset( TPCANHandle Channel)
{
	if(m_state != Ready)
		return PCAN_ERROR_UNKNOWN;

	return (TPCANStatus)m_pReset(Channel);
}

TPCANStatus PCANDriver::GetStatus(TPCANHandle Channel)
{
	if(m_state != Ready)
		return PCAN_ERROR_UNKNOWN;

	return (TPCANStatus)m_pGetStatus(Channel);
}

TPCANStatus PCANDriver::Read(TPCANHandle Channel, TPCANMsg* MessageBuffer, TPCANTimestamp* TimestampBuffer)
{
	if(m_state != Ready)
		return PCAN_ERROR_UNKNOWN;

	return (TPCANStatus)m_pRead(Channel, MessageBuffer, TimestampBuffer);
}

TPCANStatus PCANDriver::ReadFD(TPCANHandle Channel, TPCANMsgFD* MessageBuffer, TPCANTimestampFD *TimestampBuffer)
{
	if(m_state != Ready)
		return PCAN_ERROR_UNKNOWN;

	return (TPCANStatus)m_pReadFD(Channel, MessageBuffer, TimestampBuffer);
}

TPCANStatus PCANDriver::Write(TPCANHandle Channel, TPCANMsg* MessageBuffer)
{
	if(m_state != Ready)
		return PCAN_ERROR_UNKNOWN;

	return (TPCANStatus)m_pWrite(Channel, MessageBuffer);
}

TPCANStatus PCANDriver::WriteFD(TPCANHandle Channel, TPCANMsgFD* MessageBuffer)
{
	if(m_state != Ready)
		return PCAN_ERROR_UNKNOWN;

	return (TPCANStatus)m_pWriteFD(Channel, MessageBuffer);
}

TPCANStatus PCANDriver::FilterMessages(TPCANHandle Channel, DWORD FromID, DWORD ToID, TPCANMode Mode)
{
	if(m_state != Ready)
		return PCAN_ERROR_UNKNOWN;

	return (TPCANStatus)m_pFilterMessages(Channel, FromID, ToID, Mode);
}

TPCANStatus PCANDriver::GetValue(TPCANHandle Channel, TPCANParameter Parameter, void* Buffer, DWORD BufferLength)
{
	if(m_state != Ready)
		return PCAN_ERROR_UNKNOWN;

	return (TPCANStatus)m_pGetValue(Channel, Parameter, Buffer, BufferLength);
}

TPCANStatus PCANDriver::SetValue(TPCANHandle Channel, TPCANParameter Parameter, void* Buffer, DWORD BufferLength)
{
	if(m_state != Ready)
		return PCAN_ERROR_UNKNOWN;

	return (TPCANStatus)m_pSetValue(Channel, Parameter, Buffer, BufferLength);
}

TPCANStatus PCANDriver::GetErrorText(TPCANStatus Error, WORD Language, LPSTR Buffer)
{
	if(m_state != Ready)
		return PCAN_ERROR_UNKNOWN;

	return (TPCANStatus)m_pGetTextError(Error, Language, Buffer);
}

