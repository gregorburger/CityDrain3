#ifndef FLOWTYPEFACTORY_H
#define FLOWTYPEFACTORY_H

#include <typefactory.h>
#include <flow.h>

CD3_DECLARE_TYPE_FACTORY(Flow)
public:
	FlowTypeFactory();
	virtual void setParameter(Node *n,
							  const std::string &parameter,
							  QDomElement &element);

	virtual void setState(Node *n,
						  const std::string &parameter,
						  QDomElement &element);
private:
	Flow flowFromDom(QDomElement &element);
};

#endif // FLOWTYPEFACTORY_H
