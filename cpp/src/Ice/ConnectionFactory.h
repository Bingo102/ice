// **********************************************************************
//
// Copyright (c) 2003-2004 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_CONNECTION_FACTORY_H
#define ICE_CONNECTION_FACTORY_H

#include <IceUtil/Mutex.h>
#include <IceUtil/Monitor.h>
#include <Ice/ConnectionFactoryF.h>
#include <Ice/ConnectionIF.h>
#include <Ice/InstanceF.h>
#include <Ice/ObjectAdapterF.h>
#include <Ice/EndpointF.h>
#include <Ice/AcceptorF.h>
#include <Ice/TransceiverF.h>
#include <Ice/RouterF.h>
#include <Ice/EventHandler.h>
#include <list>
#include <set>

namespace Ice
{

class LocalException;
class ObjectAdapterI;

}

namespace IceInternal
{

class OutgoingConnectionFactory : public ::IceUtil::Shared, public ::IceUtil::Monitor< ::IceUtil::Mutex>
{
public:

    void destroy();

    void waitUntilFinished();

    Ice::ConnectionIPtr create(const std::vector<EndpointPtr>&, bool&);
    void setRouter(const ::Ice::RouterPrx&);
    void removeAdapter(const ::Ice::ObjectAdapterPtr&);
    void flushBatchRequests();

private:

    OutgoingConnectionFactory(const InstancePtr&);
    virtual ~OutgoingConnectionFactory();
    friend class Instance;

    const InstancePtr _instance;
    bool _destroyed;
    std::multimap<EndpointPtr, Ice::ConnectionIPtr> _connections;
    std::set<EndpointPtr> _pending; // Endpoints for which connection establishment is pending.
};

class IncomingConnectionFactory : public EventHandler, public ::IceUtil::Monitor< ::IceUtil::Mutex>
{
public:

    void activate();
    void hold();
    void destroy();

    void waitUntilHolding() const;
    void waitUntilFinished();

    EndpointPtr endpoint() const;
    bool equivalent(const EndpointPtr&) const;
    std::list<Ice::ConnectionIPtr> connections() const;
    void flushBatchRequests();

    //
    // Operations from EventHandler
    //
    virtual bool datagram() const;
    virtual bool readable() const;
    virtual void read(BasicStream&);
    virtual void message(BasicStream&, const ThreadPoolPtr&);
    virtual void finished(const ThreadPoolPtr&);
    virtual void exception(const ::Ice::LocalException&);
    virtual std::string toString() const;
    
private:

    IncomingConnectionFactory(const InstancePtr&, const EndpointPtr&, const ::Ice::ObjectAdapterPtr&);
    virtual ~IncomingConnectionFactory();
    friend class ::Ice::ObjectAdapterI;

    enum State
    {
	StateActive,
	StateHolding,
	StateClosed
    };

    void setState(State);
    void registerWithPool();
    void unregisterWithPool();

    AcceptorPtr _acceptor;
    const TransceiverPtr _transceiver;
    const EndpointPtr _endpoint;

    const ::Ice::ObjectAdapterPtr _adapter;

    bool _registeredWithPool;

    const bool _warn;

    std::list<Ice::ConnectionIPtr> _connections;

    State _state;
};

}

#endif
