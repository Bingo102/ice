// **********************************************************************
//
// Copyright (c) 2001
// MutableRealms, Inc.
// Huntsville, AL, USA
//
// All Rights Reserved
//
// **********************************************************************

#include <Ice/Ice.h>
#include <ValueI.h>

using namespace std;

InitialI::InitialI(const Ice::ObjectAdapterPtr& adapter) :
    _adapter(adapter)
{
    _simple = new Simple;
    _simple->_message = "a message 4 u";

    _printer = new PrinterI;
    _printer->_message = "Ice rulez!";
    _printerProxy = PrinterPrx::uncheckedCast(adapter->addTemporary(_printer));

    _derivedPrinter = new DerivedPrinterI;
    _derivedPrinter->_message = _printer->_message;
    _derivedPrinter->_derivedMessage = "Coming soon: the ultimate online game from MutableRealms!";
    adapter->addTemporary(_derivedPrinter);
}

SimplePtr
InitialI::simple()
{
    return _simple;
}

void
InitialI::printer(PrinterPtr& impl, PrinterPrx& proxy)
{
    impl = _printer;
    proxy = _printerProxy;
}

PrinterPtr
InitialI::derivedPrinter()
{
    return _derivedPrinter;
}

void
InitialI::throwDerivedPrinter()
{
    DerivedPrinterException ex;
    ex.derived = _derivedPrinter;
    throw ex;
}

void
PrinterI::printBackwards()
{
    string s;
    s.resize(_message.length());
    reverse_copy(_message.begin(), _message.end(), s.begin());
    cout << s << endl;
}

void
DerivedPrinterI::printUppercase()
{
    string s;
    s.resize(_derivedMessage.length());
    transform(_derivedMessage.begin(), _derivedMessage.end(), s.begin(), toupper);
    cout << s << endl;
}
