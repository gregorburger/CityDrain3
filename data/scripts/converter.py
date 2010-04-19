from xml.dom.minidom import *
import sys

dom = parse(sys.argv[1])
for parameter in dom.getElementsByTagName('parameter'):
    if parameter.getAttribute('kind') == 'array':
        values = []
        ptype = parameter.getAttribute('type')
        for child in parameter.childNodes:
            if (child.nodeType <> 1):
#                parameter.removeChild(child)
                continue
            values.append(child.getAttribute('value'))
            parameter.removeChild(child)
        parameter.setAttribute("value", ";".join(values))
        parameter.setAttribute("type", "array[%s]" % ptype)
        parameter.removeAttribute("kind")

    if parameter.getAttribute('kind') == 'complex':
        flow = {}
        for child in parameter.childNodes[1].childNodes:
            if (child.nodeType <> 1):
                continue
            flow[child.getAttribute('name')] = child.getAttribute('value')
        parameter.removeChild(parameter.childNodes[1])
        value_string = ""
        for name in flow:
            value_string = value_string + ";%s;%s" % (name, flow[name])
        value_string = value_string[1:]
        parameter.setAttribute("value", value_string)
        parameter.setAttribute("type", "flow")
        parameter.removeAttribute("kind")


if len(sys.argv) > 2:
    f = open(sys.argv[2], "w")
    dom.writexml(f)
else:
    print dom.toxml()
