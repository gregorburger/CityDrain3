#ifndef SIMULATIONSAVER_H
#define SIMULATIONSAVER_H

#include <QString>
#include <QObject>
#include <QMap>
#include <QStringList>

class ISimulation;
class IModel;
class Node;
class QXmlStreamWriter;
class QFile;

class SimulationSaver : public QObject
{
Q_OBJECT
public:
	SimulationSaver(ISimulation *simulation,
					QString path,
					QStringList plugins,
					QStringList python_modules);
	void setPath(QString path) { this->path = path; }
	QString getPath() const { return this->path; }
	virtual ~SimulationSaver();

public Q_SLOTS:
	void save();

private:
	void saveSimulation(ISimulation *sim);
	void saveModel(IModel *model);
	void saveFlowDefinition();
	void saveNodeParameters(const Node *n);

	void openTag(QString tagName, QMap<QString, QString> attributes);
	void closeTag(QString tagName);
private:
	ISimulation *simulation;
	QString path;
	QStringList plugins;
	QStringList python_modules;
	QXmlStreamWriter *writer;
	QFile *out;
};

#endif // SIMULATIONSAVER_H
