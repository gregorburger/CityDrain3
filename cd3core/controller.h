#ifndef ICONTROLLER_H
#define ICONTROLLER_H

class IModel;

class IController
{
public:
	virtual ~Controller() = 0;
	virtual const char *getControllerName() = 0;
	virtual void controll(IModel *model) = 0;
};

#endif // ICONTROLLER_H
