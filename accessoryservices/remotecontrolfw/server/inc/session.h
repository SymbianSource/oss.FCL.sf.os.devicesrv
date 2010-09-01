// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @internalComponent
*/

#ifndef REMCONSESSION_H
#define REMCONSESSION_H

#include <e32base.h>
#include <remconserverpanic.h>
#include <remconaddress.h>
#include <remcon/clientinfo.h>
#include <remcon/clienttype.h>
#include <remcon/playertype.h>
#include <remcon/clientid.h>

class CRemConServer;
class CBearerManager;
class CRemConMessage;
class CActiveHelper;
class CMessageQueue;
class CRemConInterfaceDetailsArray;
class CRemConInterfaceDetails;

/**
Rem Con session.
*/
NONSHARABLE_CLASS(CRemConSession) : public CSession2
	{
public:
	/**
	Factory method.
	@param aServer The server.
	@param aBearerManager The bearer manager.
	@param aMessage Handle on the client message triggering this session creation.
	@param aId The unique ID of the new session.
	@return Ownership of a new session.
	*/
	static CRemConSession* NewL(CRemConServer& aServer,
		CBearerManager& aBearerManager,
		const RMessage2& aMessage,
		TUint aId);

	/** Destructor. */
	~CRemConSession();

public: // called by the server
	/** 
	@return The number of remotes the current 'send' has gone to successfully.
	*/
	inline TUint& NumRemotes();

	/** 
	@return The number of remotes the current 'send' is supposed to be going 
	to. NB This is decremented as the sends actually occur.
	*/
	inline TInt& NumRemotesToTry();

	/** 
	@return The error the current 'send' will be completed with.
	*/
	inline TInt& SendError();

	/** 
	@return Information about the client using this session.
	*/
	inline TClientInfo& ClientInfo();

	/** 
	@return Information about the client using this session.
	*/
	inline const TClientInfo& ClientInfo() const;

	/**
	@return This session's type.
	*/
	inline TRemConClientType Type() const;

	/**
	@return This session's unique identifier.
	*/
	inline TUint Id() const;

	/**
	Used by the server to complete a client send request. 
	@param aError Error.					
	*/
	void CompleteSend();
	
	/**
	Used by the server to complete a client send notify request.
	*/
	void CompleteSendNotify();
	
	/** 
	Checks if this message is supported by the session
	@return ETrue if it is, EFalse otherwise
	*/
	TBool SupportedMessage(const CRemConMessage& aMsg);

	/**
	Writes aMsg to the client's Receive message (NB RMessage2::Write may fail) 
	and completes the client's Receive message with any error.
	@return Error.
	*/
	TInt WriteMessageToClient(const CRemConMessage& aMsg);

	/**
	Accessor for the currently outstanding client Send message. This is used 
	to check the client's capabilities to send over a particular bearer.
	@return Handle to client's send message.
	*/
	inline const RMessage2& CurrentSendMessage() const;

	/**
	Accessor for the currently outstanding client Receive message.
	@return Handle to client's receive message.
	*/
	inline const RMessage2& CurrentReceiveMessage() const;

	inline const TPlayerType& PlayerType() const;
	inline const TPlayerSubType& PlayerSubType() const;
	
	inline const TDesC8& Name() const;
	
	TInt SupportedInterfaces(RArray<TUid>& aUids);
	TInt SupportedBulkInterfaces(RArray<TUid>& aUids);
	TInt SupportedOperations(TUid aInterfaceUid, RArray<TUint>& aOperations);

	TBool ClientAvailable() const;

	/**
	Panics the client's current Send message with the given code.
	*/
	void PanicSend(TRemConClientPanic aCode);

public: // called by the bearer manager
	/**
	Indicates that a connect request has been completed. The request was not 
	necessarily from this session- the session must check that the connected 
	address is one it has asked to be connected. If it is, and we have a 
	connect request outstanding, the connect request should be completed.
	@param aAddr The connected address.
	@param aError The error with which the connection attempt was completed.
	*/
	void CompleteConnect(const TRemConAddress& aAddr, TInt aError);

	/**
	Indicates that a disconnect request has been completed. The request was 
	not necessarily from this session- the session must check that the 
	disconnected address is one it has asked to be disconnected. If it is, and 
	we have a disconnect request outstanding, the disconnect request should be 
	completed.
	@param aAddr The disconnected address.
	@param aError The error with which the disconnection attempt was 
	completed.
	*/
	void CompleteDisconnect(const TRemConAddress& aAddr, TInt aError);

	/**
	Indicates that a connection has come up or down. If the session has a 
	'connections notification' outstanding, it should be completed. Note that, 
	when a connection comes up or down, ConnectionsChanged is called before 
	CompleteConnect or DisconnectComplete. These functions are called 
	synchronously by the connection-handling code.
	*/
	void ConnectionsChanged();
	
public: // called by the active helper
	/**
	Process pending messages.
	*/
	void ProcessPendingMsgL();
	
private:
	/**
	Constructor.
	@param aServer The server.
	@param aBearerManager The bearer manager.
	@param aId The unique ID of the new session.
	*/
	CRemConSession(CRemConServer& aServer, 
		CBearerManager& aBearerManager,
		TUint aId);

	/**
	2nd-phase construction.
	@param aMessage The message received from the client.
	*/
	void ConstructL(const RMessage2& aMessage);
		
private: // from CSession2
	/**
	Called when a message is received from the client.
	@param aMessage Message received from the client.
	*/
	void ServiceL(const RMessage2& aMessage);

private: // utility- IPC command handlers
	void SetClientType(const RMessage2& aMessage);
	void GoConnectionOriented(const RMessage2& aMessage);
	void GoConnectionless(const RMessage2& aMessage);
	void ConnectBearer(const RMessage2& aMessage);
	void ConnectBearerCancel(const RMessage2& aMessage);
	void DisconnectBearer(const RMessage2& aMessage);
	void DisconnectBearerCancel(const RMessage2& aMessage);
	void Send(const RMessage2& aMessage);
	void SendNotify(const RMessage2& aMessage);
	void SendUnreliable(const RMessage2& aMessage);
	void SendCancel(const RMessage2& aMessage);
	void Receive(const RMessage2& aMessage);
	void ReceiveCancel(const RMessage2& aMessage);
	void GetConnectionCount(const RMessage2& aMessage);
	void GetConnections(const RMessage2& aMessage);
	void NotifyConnectionsChange(const RMessage2& aMessage);
	void NotifyConnectionsChangeCancel(const RMessage2& aMessage);
	void RegisterInterestedAPIs(const RMessage2& aMessage);
	
private: // utility
	/**
	Utility to complete the given message with the given error code.
	@param aMessage Message to complete.
	@param aError Error to complete with.
	*/
	void CompleteClient(const RMessage2& aMessage, TInt aError);

	void DoSendL(const RMessage2& aMessage);
	void DoSendNotifyL(const RMessage2& aMessage);
	CRemConMessage* DoCreateUnreliableMessageL(const RMessage2& aMessage);
	void DoRegisterInterestedAPIsL(const RMessage2& aMessage);
	void DoSetClientTypeL(const RMessage2& aMessage);
	void DoSendCancel();
	
	CRemConInterfaceDetails* FindInterfaceByUid(TUid aUid) const;

	void SendToServer(CRemConMessage& aMsg);
	
	void CheckForPendingMsg() const; 

	static TInt SendNextCb(TAny* aThis);

	void DoSendNext();
	
	void EmptySendQueue();

	void WriteMessageToClientL(const CRemConMessage& aMsg);
	
private: // unowned
	CRemConServer& iServer;
	CBearerManager& iBearerManager;

private: // message handles for asynchronous IPC requests
	RMessage2 iConnectBearerMsg;
	RMessage2 iDisconnectBearerMsg;
	RMessage2 iSendMsg;
	RMessage2 iReceiveMsg;
	RMessage2 iNotifyConnectionsChangeMsg;
	// Stores pending connect/disconnect request
	// There can be only one pending request at any time
	RMessage2 iPendingMsg;
	
	CMessageQueue* iSendQueue;
	
private: // owned
	// Address of remote device associated with this session (only relevant 
	// for controllers). A null remote address indicates a connectionless 
	// controller; a non-null UID indicates a connection-oriented controller.
	TRemConAddress iRemoteAddress;

	TRemConClientType iType;

	// The client's process ID, secure ID and caps.
	TClientInfo iClientInfo;

	// Unique session identifier.
	const TUint iId;

	// Used to control the completion of the client's send request until we 
	// know the correct values of iNumSuccessfulRemotes and iSendError to use.
	// Generally 0. 
	// 1 when sending a connection-oriented command or a response.
	// N when sending a connectionless command (where N is the number of 
	// remotes the TSP addressed it to). 
	// While processing outgoing commands to multiple remotes, 
	// iNumRemotesToTry is decremented each time we finish trying to address a 
	// remote. This may be at the connection stage or the actual send stage.
	// -1 means that the client's send has been completed already. This is 
	// useful due to the potentially recursive processing of multiple 
	// connections.
	TInt iNumRemotesToTry;
	// For completion of the current send request. NB A send may be completed 
	// only after numerous asynchronous stages.
	TUint iNumRemotes;
	TInt iSendError;
	
	CRemConInterfaceDetailsArray* iInterestedAPIs;
	// the player type information
	TPlayerTypeInformation iPlayerType;
	//the player name 
	RBuf8 iPlayerName;

	// Helps with session's async connect/disconnect requests
	CActiveHelper *iPendingMsgProcessor;
	
	CAsyncCallBack* iSendNextCallBack;
	
	enum TRemConSessionSending
		{
		ENotSending,
		ESendingReliable,
		ESendingUnreliable
		};
	
	TRemConSessionSending iSending;
	
	};

// Inlines

TInt& CRemConSession::NumRemotesToTry()
	{
	return iNumRemotesToTry;
	}

TUint& CRemConSession::NumRemotes()
	{
	return iNumRemotes;
	}

TInt& CRemConSession::SendError()
	{
	return iSendError;
	}

TClientInfo& CRemConSession::ClientInfo()
	{
	return iClientInfo;
	}

const TClientInfo& CRemConSession::ClientInfo() const
	{
	return iClientInfo;
	}

TRemConClientType CRemConSession::Type() const
	{
	return iType;
	}

TUint CRemConSession::Id() const
	{
	return iId;
	}

const RMessage2& CRemConSession::CurrentSendMessage() const
	{
	return iSendMsg;
	}

const RMessage2& CRemConSession::CurrentReceiveMessage() const
	{
	return iReceiveMsg;
	}

const TPlayerType& CRemConSession::PlayerType() const
	{
	return iPlayerType.iPlayerType;
	}

const TPlayerSubType& CRemConSession::PlayerSubType() const
	{
	return iPlayerType.iPlayerSubType;
	}

const TDesC8& CRemConSession::Name() const
	{
	return iPlayerName;
	}

#endif // REMCONSESSION_H

