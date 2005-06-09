// **********************************************************************
//
// Copyright (c) 2003-2005 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package IceInternal;

public class IncomingBase
{
    protected
    IncomingBase(Instance instance, Ice.ConnectionI connection, Ice.ObjectAdapter adapter, boolean response)
    {
	_response = response;
        _os = new BasicStream(instance);
	_connection = connection;

        _current = new Ice.Current();
        _current.id = new Ice.Identity();
        _current.adapter = adapter;
        _current.con = _connection;

        _cookie = new Ice.LocalObjectHolder();
    }

    protected
    IncomingBase(IncomingBase in) // Adopts the argument. It must not be used afterwards.
    {
	_current = in._current;
	in._current = null;

	_servant = in._servant;
	in._servant = null;

	_cookie = in._cookie;
	in._cookie = null;

	_response = in._response;
	in._response = false;

	_os = in._os;
	in._os = null;

	_connection = in._connection;
	in._connection = null;
    }

    //
    // This function allows this object to be reused, rather than reallocated.
    //
    public void
    reset(Instance instance, Ice.ConnectionI connection, Ice.ObjectAdapter adapter, boolean response)
    {
        //
        // Don't recycle the Current object, because servants may keep a reference to it.
        //
        _current = new Ice.Current();
        _current.id = new Ice.Identity();
        _current.adapter = adapter;
        _current.con = connection;

	_servant = null;

	if(_cookie == null)
	{
	    _cookie = new Ice.LocalObjectHolder();
	}
	else
	{
	    _cookie.value = null;
	}

	_response = response;

	if(_os == null)
	{
	    _os = new BasicStream(instance);
	}
	else
	{
	    _os.reset();
	}

	_connection = connection;
    }

    final protected void
    __warning(java.lang.Exception ex)
    {
	if(IceUtil.Debug.ASSERT)
	{
	    IceUtil.Debug.Assert(_os != null);
	}

	java.io.ByteArrayOutputStream sw = new java.io.ByteArrayOutputStream();
	java.io.PrintStream pw = new java.io.PrintStream(sw);
	IceUtil.OutputBase out = new IceUtil.OutputBase(pw);
	out.setUseTab(false);
	out.print("dispatch exception:");
	out.print("\nidentity: " + Ice.Util.identityToString(_current.id));
	out.print("\nfacet: " + IceUtil.StringUtil.escapeString(_current.facet, ""));
	out.print("\noperation: " + _current.operation);
	out.print("\n");
	out.print(ex.toString());
	pw.flush();
	_os.instance().logger().warning(sw.toString());
    }

    protected Ice.Current _current;
    protected Ice.Object _servant;
    protected Ice.LocalObjectHolder _cookie;

    protected boolean _response;

    protected BasicStream _os;

    protected Ice.ConnectionI _connection;
}
