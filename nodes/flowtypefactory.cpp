#include "flowtypefactory.h"
#include <node.h>
#include <flow.h>


FlowTypeFactory::FlowTypeFactory() {
}

void FlowTypeFactory::setParameter(Node *n,
							  const std::string &parameter,
							  QDomElement &element) {
	//n->setParameter(parameter, flowFromDom(element).get());
	Flow * f = flowFromDom(element);
	n->setParameter(parameter, *f);
	delete f;
}

void FlowTypeFactory::setState(Node *n,
						  const std::string &state,
						  QDomElement &element) {
	Flow * f = flowFromDom(element);
	n->setState(state, *f);
	delete f;
}

Flow *FlowTypeFactory::flowFromDom(QDomElement &e) {
	Flow *f = new Flow();

	if (e.nodeName() != "flow") {
		std::cerr << "type " << e.nodeName().toStdString() << "not known" << std::endl;
		return 0;
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
	f->prepareData();
	return f;
}
