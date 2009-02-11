#ifndef FLOWTYPEFACTORY_H
#define FLOWTYPEFACTORY_H

#include <typefactory.h>
#include <flow.h>
#include <boost/shared_ptr.hpp>

CD3_DECLARE_TYPE_FACTORY(Flow)
public:
	FlowTypeFactory();
	virtual void setParameter(boost::shared_ptr<Node> n,
							  const std::string &parameter,
							  QDomElement &element);

	virtual void setState(boost::shared_ptr<Node> n,
						  const std::string &parameter,
						  QDomElement &element);
private:
	boost::shared_ptr<Flow> flowFromDom(QDomElement &element);
};

#endif // FLOWTYPEFACTORY_H
