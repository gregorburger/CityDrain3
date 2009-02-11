#include "flowtypefactory.h"
#include <node.h>

FlowTypeFactory::FlowTypeFactory() {
}

void FlowTypeFactory::setParameter(Node *n,
							  const std::string &parameter,
							  QDomElement &element) {
	n->setParameter(parameter, flowFromDom(element));
}

void FlowTypeFactory::setState(Node *n,
						  const std::string &state,
						  QDomElement &element) {
	n->setState<Flow>(state, flowFromDom(element));
}

std::auto_ptr<Flow> FlowTypeFactory::flowFromDom(QDomElement &e) {
	std::auto_ptr<Flow> f(new Flow());

	if (e.nodeName() != "flow") {
		std::cerr << "type " << e.nodeName().toStdString() << "not known" << std::endl;
		return std::auto_ptr<Flow>((Flow *)0);
	}

	for (int i = 0; i < e.childNodes().size(); i++) {
		QDomNode node = e.childNodes().at(i);
		QDomNamedNodeMap attr = node.attributes();
		std::string name = attr.namedItem("name").toAttr().value().toStdString();
		QString kind = attr.namedItem("kind").toAttr().value();
		CalculationUnit *unit = 0;
		if (kind == "flow") {
			unit = &CalculationUnit::flow;
		}
		if (kind == "concentration") {
			unit = &CalculationUnit::concentration;
		}
		if (kind == "volume") {
			unit = &CalculationUnit::volume;
		}
		if (unit == 0) {
			int line = attr.namedItem("kind").lineNumber();
			int column = attr.namedItem("kind").columnNumber();
			std::cerr << line << ":" << column << ":" << "wrong unit" << std::endl;
		}
		double value = attr.namedItem("value").toAttr().value().toDouble();
		f->addUnit(name, unit, value);
	}
	return f;
}
