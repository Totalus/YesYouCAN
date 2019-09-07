#ifndef PCANDRIVER_H
#define PCANDRIVER_H

#include <QLibrary>

#include "PCANBasic.h"

extern TPCANHandle m_HandlesArray[59];

// Function pointers
//
typedef TPCANStatus (__stdcall *fpInitialize)(TPCANHandle, TPCANBaudrate, TPCANType, DWORD, WORD);
typedef TPCANStatus (__stdcall *fpInitializeFD)(TPCANHandle, TPCANBitrateFD);
typedef TPCANStatus (__stdcall *fpOneParam)(TPCANHandle);
typedef TPCANStatus (__stdcall *fpRead)(TPCANHandle, TPCANMsg*, TPCANTimestamp*);
typedef TPCANStatus (__stdcall *fpReadFD)(TPCANHandle, TPCANMsgFD*, TPCANTimestampFD*);
typedef TPCANStatus (__stdcall *fpWrite)(TPCANHandle, TPCANMsg*);
typedef TPCANStatus (__stdcall *fpWriteFD)(TPCANHandle, TPCANMsgFD*);
typedef TPCANStatus (__stdcall *fpFilterMessages)(TPCANHandle, DWORD, DWORD, TPCANMode);
typedef TPCANStatus (__stdcall *fpGetSetValue)(TPCANHandle, TPCANParameter, void*, DWORD);
typedef TPCANStatus (__stdcall *fpGetErrorText)(TPCANStatus, WORD, LPSTR);

// Re-define of name for better code-read
//
#define fpUninitialize fpOneParam
#define fpReset fpOneParam
#define fpGetStatus fpOneParam
#define fpGetValue fpGetSetValue
#define fpSetValue fpGetSetValue


enum LibState_t
{
	Ready,
	LibraryNotFound,
	FunctionsNotFound,
	Unloaded
};

class PCANDriver : public QObject
{
	Q_OBJECT

	public:
		PCANDriver(QObject *parent = 0);
		~PCANDriver();

		Q_DIABLE_COPY(PCANDriver);

		QString getStatusStr();
		LibState_t getStatus();

		void load();
		void unload();


		// Library function implementation
		/// <summary>
		/// Initializes a PCAN Channel
		/// </summary>
		/// <param name="Channel">"The handle of a PCAN Channel"</param>
		/// <param name="Btr0Btr1">"The speed for the communication (BTR0BTR1 code)"</param>
		/// <param name="HwType">"NON PLUG&PLAY: The type of hardware and operation mode"</param>
		/// <param name="IOPort">"NON PLUG&PLAY: The I/O address for the parallel port"</param>
		/// <param name="Interupt">"NON PLUG&PLAY: Interrupt number of the parallel port"</param>
		/// <returns>"A TPCANStatus error code"</returns>
		TPCANStatus Initialize(TPCANHandle Channel, TPCANBaudrate Btr0Btr1, TPCANType HwType = 0, DWORD IOPort = 0, WORD Interrupt = 0);

		/// <summary>
		/// Initializes a FD capable PCAN Channel
		/// </summary>
		/// <param name="Channel">"The handle of a FD capable PCAN Channel"</param>
		/// <param name="BitrateFD">"The speed for the communication (FD Bitrate string)"</param>
		/// <remarks>See PCAN_BR_* values
		/// * Parameter and values must be separated by '='
		/// * Couples of Parameter/value must be separated by ','
		/// * Following Parameter must be filled out: f_clock, data_brp, data_sjw, data_tseg1, data_tseg2,
		///   nom_brp, nom_sjw, nom_tseg1, nom_tseg2.
		/// * Following Parameters are optional (not used yet): data_ssp_offset, nom_samp
		///</remarks>
		/// <example>f_clock_mhz=80,nom_brp=0,nom_tseg1=13,nom_tseg2=0,nom_sjw=0,data_brp=0,
		/// data_tseg1=13,data_tseg2=0,data_sjw=0</example>
		/// <returns>"A TPCANStatus error code"</returns>
		TPCANStatus InitializeFD(TPCANHandle Channel, TPCANBitrateFD BitrateFD);

		/// <summary>
		/// Uninitializes one or all PCAN Channels initialized by CAN_Initialize
		/// </summary>
		/// <remarks>Giving the TPCANHandle value "PCAN_NONEBUS",
		/// uninitialize all initialized channels</remarks>
		/// <param name="Channel">"The handle of a PCAN Channel"</param>
		/// <returns>"A TPCANStatus error code"</returns>
		TPCANStatus Uninitialize(TPCANHandle Channel);

		/// <summary>
		/// Resets the receive and transmit queues of the PCAN Channel
		/// </summary>
		/// <remarks>
		/// A reset of the CAN controller is not performed.
		/// </remarks>
		/// <param name="Channel">"The handle of a PCAN Channel"</param>
		/// <returns>"A TPCANStatus error code"</returns>
		TPCANStatus Reset(TPCANHandle Channel);

		/// <summary>
		/// Gets the current status of a PCAN Channel
		/// </summary>
		/// <param name="Channel">"The handle of a PCAN Channel"</param>
		/// <returns>"A TPCANStatus error code"</returns>
		TPCANStatus GetStatus(TPCANHandle Channel);

		/// <summary>
		/// Reads a CAN message from the receive queue of a PCAN Channel
		/// </summary>
		/// <param name="Channel">"The handle of a PCAN Channel"</param>
		/// <param name="MessageBuffer">"A TPCANMsg structure buffer to store the CAN message"</param>
		/// <param name="TimestampBuffer">"A TPCANTimestamp structure buffer to get
		/// the reception time of the message. If this value is not desired, this parameter
		/// should be passed as NULL"</param>
		/// <returns>"A TPCANStatus error code"</returns>
		TPCANStatus Read(TPCANHandle Channel, TPCANMsg* MessageBuffer, TPCANTimestamp* TimestampBuffer);

		/// <summary>
		/// Reads a CAN message from the receive queue of a FD capable PCAN Channel
		/// </summary>
		/// <param name="Channel">"The handle of a FD capable PCAN Channel"</param>
		/// <param name="MessageBuffer">"A TPCANMsgFD structure buffer to store the CAN message"</param>
		/// <param name="TimestampBuffer">"A TPCANTimestampFD buffer to get
		/// the reception time of the message. If this value is not desired, this parameter
		/// should be passed as NULL"</param>
		/// <returns>"A TPCANStatus error code"</returns>
		TPCANStatus ReadFD(TPCANHandle Channel, TPCANMsgFD* MessageBuffer, TPCANTimestampFD *TimestampBuffer);

		/// <summary>
		/// Transmits a CAN message
		/// </summary>
		/// <param name="Channel">"The handle of a PCAN Channel"</param>
		/// <param name="MessageBuffer">"A TPCANMsg buffer with the message to be sent"</param>
		/// <returns>"A TPCANStatus error code"</returns>
		TPCANStatus Write(TPCANHandle Channel, TPCANMsg* MessageBuffer);

		/// <summary>
		/// Transmits a CAN message over a FD capable PCAN Channel
		/// </summary>
		/// <param name="Channel">"The handle of a FD capable PCAN Channel"</param>
		/// <param name="MessageBuffer">"A TPCANMsgFD buffer with the message to be sent"</param>
		/// <returns>"A TPCANStatus error code"</returns>
		TPCANStatus WriteFD(TPCANHandle Channel, TPCANMsgFD* MessageBuffer);

		/// <summary>
		/// Configures the reception filter.
		/// </summary>
		/// <remarks>The message filter will be expanded with every call to
		/// this function. If it is desired to reset the filter, please use
		/// the CAN_SetParameter function</remarks>
		/// <param name="Channel">"The handle of a PCAN Channel"</param>
		/// <param name="FromID">"The lowest CAN ID to be received"</param>
		/// <param name="ToID">"The highest CAN ID to be received"</param>
		/// <param name="Mode">"Message type, Standard (11-bit identifier) or
		/// Extended (29-bit identifier)"</param>
		/// <returns>"A TPCANStatus error code"</returns>
		TPCANStatus FilterMessages(TPCANHandle Channel, DWORD FromID, DWORD ToID, TPCANMode Mode);

		/// <summary>
		/// Retrieves a PCAN Channel value
		/// </summary>
		/// <remarks>Parameters can be present or not according with the kind
		/// of Hardware (PCAN Channel) being used. If a parameter is not available,
		/// a PCAN_ERROR_ILLPARAMTYPE error will be returned</remarks>
		/// <param name="Channel">"The handle of a PCAN Channel"</param>
		/// <param name="Parameter">"The TPCANParameter parameter to get"</param>
		/// <param name="Buffer">"Buffer for the parameter value"</param>
		/// <param name="BufferLength">"Size in bytes of the buffer"</param>
		/// <returns>"A TPCANStatus error code"</returns>
		TPCANStatus GetValue(TPCANHandle Channel, TPCANParameter Parameter, void* Buffer, DWORD BufferLength);

		/// <summary>
		/// Configures or sets a PCAN Channel value
		/// </summary>
		/// <remarks>Parameters can be present or not according with the kind
		/// of Hardware (PCAN Channel) being used. If a parameter is not available,
		/// a PCAN_ERROR_ILLPARAMTYPE error will be returned</remarks>
		/// <param name="Channel">"The handle of a PCAN Channel"</param>
		/// <param name="Parameter">"The TPCANParameter parameter to set"</param>
		/// <param name="Buffer">"Buffer with the value to be set"</param>
		/// <param name="BufferLength">"Size in bytes of the buffer"</param>
		/// <returns>"A TPCANStatus error code"</returns>
		TPCANStatus SetValue(TPCANHandle Channel, TPCANParameter Parameter, void* Buffer, DWORD BufferLength);

		/// <summary>
		/// Returns a descriptive text of a given TPCANStatus error
		/// code, in any desired language
		/// </summary>
		/// <remarks>The current languages available for translation are:
		/// Neutral (0x00), German (0x07), English (0x09), Spanish (0x0A),
		/// Italian (0x10) and French (0x0C)</remarks>
		/// <param name="Error">"A TPCANStatus error code"</param>
		/// <param name="Language">"Indicates a 'Primary language ID'"</param>
		/// <param name="Buffer">"Buffer for a null terminated char array"</param>
		/// <returns>"A TPCANStatus error code"</returns>
		TPCANStatus GetErrorText(TPCANStatus Error, WORD Language, LPSTR Buffer);




	private:
		QLibrary m_lib;
		LibState_t m_state;

		// Library function pointers
		fpInitialize m_pInitialize;
		fpUninitialize m_pUnInitialize;
		fpReset m_pReset;
		fpGetStatus m_pGetStatus;
		fpRead m_pRead;
		fpWrite m_pWrite;
		fpGetValue m_pGetValue;
		fpSetValue m_pSetValue;
		fpGetErrorText m_pGetTextError;
		fpFilterMessages m_pFilterMessages;
		fpWriteFD m_pWriteFD;
		fpReadFD m_pReadFD;
		fpInitializeFD m_pInitializeFD;


		// Private functions
		void resetPointers();


};

#endif // PCANDRIVER_H
