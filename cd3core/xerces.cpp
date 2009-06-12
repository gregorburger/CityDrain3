#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <iostream>
#include <vector>
#include <boost/foreach.hpp>


#include "dtd.h"

using namespace xercesc_3_0;
using namespace std;

struct LocalManager : public MemoryManager {
	void *allocate(XMLSize_t s) {
		void *ret = XMLPlatformUtils::fgMemoryManager->allocate(s);
		allocs.push_back(ret);
		return ret;
	}
	void deallocate(void *p) {
		(void) p;
	}
	virtual ~LocalManager() {
		void *mem;
		BOOST_FOREACH(mem, allocs) {
			XMLPlatformUtils::fgMemoryManager->deallocate(mem);
		}
	}
	MemoryManager *getExceptionMemoryManager() {
		return XMLPlatformUtils::fgMemoryManager;
	}
private:
	vector<void *> allocs;
};


struct Handler : public DefaultHandler {
	virtual void startElement(const   XMLCh* const    uri,
							  const   XMLCh* const    localname,
							  const   XMLCh* const    qname,
							  const Attributes&	attrs) {
		LocalManager lm;
		XMLCh *node = XMLString::transcode("node", &lm);
		if (XMLString::compareString(node, localname) == 0) {
			cout << "found node" << endl;
		}
		cout << "start element: " << XMLString::transcode(localname, &lm) << endl;
	}

	InputSource* resolveEntity (const XMLCh* const publicId,
								const XMLCh* const systemId) {
		(void) publicId;
		(void) systemId;
		LocalManager lm;
		LocalFileInputSource *fs;
		fs = new LocalFileInputSource(
				XMLString::transcode("/home/gregor/Work/cd3-1/dtd/"),
				XMLString::transcode("model.dtd"));
		return fs;
	}


	void fatalError(const SAXParseException& exception)	{
		char* message = XMLString::transcode(exception.getMessage());
		cout << "Fatal Error: " << message
			 << " at line: " << exception.getLineNumber()
			 << endl;
		XMLString::release(&message);
	}

	void warning(const SAXParseException& exception)	{
		char* message = XMLString::transcode(exception.getMessage());
		cout << "Warning: " << message
			 << " at line: " << exception.getLineNumber()
			 << endl;
		XMLString::release(&message);
	}

	void error(const SAXParseException& exception)	{
		char* message = XMLString::transcode(exception.getMessage());
		cout << "Error: " << message
			 << " at line: " << exception.getLineNumber()
			 << endl;
		XMLString::release(&message);
	}

};

int main(int argc, char *argv[]) {
	XMLPlatformUtils::Initialize();
	SAX2XMLReader *reader = XMLReaderFactory::createXMLReader();
	reader->setFeature(XMLUni::fgSAX2CoreValidation, true);
	reader->setFeature(XMLUni::fgXercesLoadExternalDTD, true);
	Handler *handler = new Handler();
	reader->setContentHandler(handler);
	reader->setErrorHandler(handler);
	reader->setEntityResolver(handler);
	reader->parse(argv[1]);
	XMLPlatformUtils::Terminate();
}
