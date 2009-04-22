#include "flowtypefactory.h"
#include <node.h>
#include <flow.h>
#include <calculationunit.h>
#include <boost/format.hpp>
#include <cd3assert.h>


using namespace boost;

FlowTypeFactory::FlowTypeFactory() {
}

void FlowTypeFactory::setParameter(Node *n,
							  const std::string &parameter,
							  QDomElement &element) {
	//n->setParameter(parameter, flowFromDom(element).get());
	Flow f = flowFromDom(element);
	n->setParameter(parameter, f);
}

void FlowTypeFactory::setState(Node *n,
						  const std::string &state,
						  QDomElement &element) {
	Flow f = flowFromDom(element);
	n->setState(state, f);
}

Flow FlowTypeFactory::flowFromDom(QDomElement &e) {
	Flow f;

	if (e.nodeName() != "flow") {
		std::cerr << "type " << e.nodeName().toStdString() << "not known" << std::endl;
		return Flow::nullFlow();
	}

	for (int i = 0; i < e.childNodes().size(); i++) {
		QDomNode node = e.childNodes().at(i);
		QDomNamedNodeMap attr = node.attributes();
		std::string name = attr.namedItem("name").toAttr().value().toStdString();
		QString kind = attr.namedItem("kind").toAttr().value();
		CalculationUnit *unit = CalculationUnit::fromString(kind.toStdString());
		cd3assert(unit != CalculationUnit::null,
			   str(format("unknown unit type: %1%") % kind.toStdString()));
		double value = attr.namedItem("value").toAttr().value().toDouble();
		f.addUnit(name, unit, value);
	}
	return f;
}
