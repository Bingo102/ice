// **********************************************************************
//
// Copyright (c) 2003-2004 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_CONNECTIONI_H
#define ICE_CONNECTIONI_H

#include <IceUtil/Mutex.h>
#include <IceUtil/Monitor.h>
#include <IceUtil/Time.h>
#include <Ice/ConnectionIF.h>
#include <Ice/ConnectionFactoryF.h>
#include <Ice/InstanceF.h>
#include <Ice/TransceiverF.h>
#include <Ice/TransportInfoF.h>
#include <Ice/ObjectAdapterF.h>
#include <Ice/ServantManagerF.h>
#include <Ice/EndpointF.h>
#include <Ice/LoggerF.h>
#include <Ice/TraceLevelsF.h>
#include <Ice/OutgoingAsyncF.h>
#include <Ice/EventHandler.h>

namespace IceInternal
{

class Outgoing;

}

namespace Ice
{

class LocalException;

class ConnectionI : public IceInternal::EventHandler, public IceUtil::Monitor<IceUtil::Mutex>
{
public:

    void validate();
    enum DestructionReason
    {
	ObjectAdapterDeactivated,
	CommunicatorDestroyed
    };
    void activate();
    void hold();
    void destroy(DestructionReason);

    bool isValidated() const;
    bool isDestroyed() const;
    bool isFinished() const;

    void waitUntilHolding() const;
    void waitUntilFinished(); // Not const, as this might close the connection upon timeout.

    void monitor();

    void prepareRequest(IceInternal::BasicStream*);
    void sendRequest(IceInternal::BasicStream*, IceInternal::Outgoing*, bool);
    void sendAsyncRequest(IceInternal::BasicStream*, const IceInternal::OutgoingAsyncPtr&, bool);

    void prepareBatchRequest(IceInternal::BasicStream*);
    void finishBatchRequest(IceInternal::BasicStream*, bool);
    void flushBatchRequest();

    void sendResponse(IceInternal::BasicStream*, Byte);
    void sendNoResponse();

    int timeout() const;
    IceInternal::EndpointPtr endpoint() const;

    void setAdapter(const ObjectAdapterPtr&);
    ObjectAdapterPtr getAdapter() const;

    TransportInfoPtr getTransportInfo() const;

    //
    // Operations from EventHandler
    //
    virtual bool datagram() const;
    virtual bool readable() const;
    virtual void read(IceInternal::BasicStream&);
    virtual void message(IceInternal::BasicStream&, const IceInternal::ThreadPoolPtr&);
    virtual void finished(const IceInternal::ThreadPoolPtr&);
    virtual void exception(const LocalException&);
    virtual std::string toString() const;

    //
    // Compare connections for sorting purposes.
    //
    virtual bool operator==(const ConnectionI&) const;
    virtual bool operator!=(const ConnectionI&) const;
    virtual bool operator<(const ConnectionI&) const;

private:

    ConnectionI(const IceInternal::InstancePtr&, const IceInternal::TransceiverPtr&, 
		const IceInternal::EndpointPtr&, const ObjectAdapterPtr&);
    virtual ~ConnectionI();
    friend class IceInternal::IncomingConnectionFactory;
    friend class IceInternal::OutgoingConnectionFactory;

    enum State
    {
	StateNotValidated,
	StateActive,
	StateHolding,
	StateClosing,
	StateClosed
    };

    void setState(State, const LocalException&);
    void setState(State);

    void initiateShutdown() const;

    void registerWithPool();
    void unregisterWithPool();

    static void doCompress(IceInternal::BasicStream&, IceInternal::BasicStream&);
    static void doUncompress(IceInternal::BasicStream&, IceInternal::BasicStream&);

    IceInternal::TransceiverPtr _transceiver;
    const TransportInfoPtr _info;
    const IceInternal::EndpointPtr _endpoint;

    ObjectAdapterPtr _adapter;
    IceInternal::ServantManagerPtr _servantManager;

    const LoggerPtr _logger;
    const IceInternal::TraceLevelsPtr _traceLevels;

    bool _registeredWithPool;
    const IceInternal::ThreadPoolPtr _threadPool;

    const bool _warn;

    const int _acmTimeout;
    IceUtil::Time _acmAbsoluteTimeout;

    const std::vector<Byte> _requestHdr;
    const std::vector<Byte> _requestBatchHdr;
    const std::vector<Byte> _replyHdr;

    Int _nextRequestId;

    std::map<Int, IceInternal::Outgoing*> _requests;
    std::map<Int, IceInternal::Outgoing*>::iterator _requestsHint;

    struct AsyncRequest
    {
	IceInternal::OutgoingAsyncPtr p;
	IceUtil::Time t;
    };
    std::map<Int, AsyncRequest> _asyncRequests;
    std::map<Int, AsyncRequest>::iterator _asyncRequestsHint;

    std::auto_ptr<LocalException> _exception;

    IceInternal::BasicStream _batchStream;
    bool _batchStreamInUse;
    int _batchRequestNum;
    bool _batchRequestCompress;

    int _dispatchCount;

    State _state; // The current state.
    IceUtil::Time _stateTime; // The last time when the state was changed.

    //
    // We have a separate mutex for sending, so that we don't block
    // the whole connection when we do a blocking send.
    //
    IceUtil::Mutex _sendMutex;
};

}

#endif
