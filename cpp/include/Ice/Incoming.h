// **********************************************************************
//
// Copyright (c) 2003-2004 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_INCOMING_H
#define ICE_INCOMING_H

#include <Ice/InstanceF.h>
#include <Ice/ConnectionIF.h>
#include <Ice/ServantLocatorF.h>
#include <Ice/ServantManagerF.h>
#include <Ice/BasicStream.h>
#include <Ice/Current.h>

namespace IceInternal
{

class ICE_API IncomingBase : public IceUtil::noncopyable
{
protected:

    IncomingBase(Instance*, Ice::ConnectionI*, const Ice::ObjectAdapterPtr&, 
		 const Ice::TransportInfoPtr&, bool, Ice::Byte);
    IncomingBase(IncomingBase& in); // Adopts the argument. It must not be used afterwards.
    
    void __finishInvoke();
    void __warning(const Ice::Exception&) const;
    void __warning(const std::string&) const;

    Ice::Current _current;
    Ice::ObjectPtr _servant;
    Ice::ServantLocatorPtr _locator;
    Ice::LocalObjectPtr _cookie;

    bool _response;
    Ice::Byte _compress;

    BasicStream _os;

//
// Cannot be private. IncomingAsync needs _connection to initialize a
// ConnectionIPtr.
//
//private:

    //
    // Optimization. The connection may not be deleted while a
    // stack-allocated Incoming still holds it.
    //
    Ice::ConnectionI* _connection;
};

class ICE_API Incoming : public IncomingBase
{
public:

    Incoming(Instance*, Ice::ConnectionI*, const Ice::ObjectAdapterPtr&, 
	     const Ice::TransportInfoPtr&, bool, Ice::Byte);

    void invoke(const ServantManagerPtr&);

    // Inlined for speed optimization.
    BasicStream* is() { return &_is; }
    BasicStream* os() { return &_os; }

private:

    BasicStream _is;
};

}

#endif
