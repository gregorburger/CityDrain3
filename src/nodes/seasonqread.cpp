/**
 * CityDrain3 is an open source software for modelling and simulating integrated 
 * urban drainage systems.
 * 
 * Copyright (C) 2012 Gregor Burger
 * 
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation; version 2 of the License.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this program; if not, write to the Free Software Foundation, Inc., 51 Franklin 
 * Street, Fifth Floor, Boston, MA 02110-1301, USA.
 **/

#include "seasonqread.h"
#include "cd3assert.h"
#include <fstream>
#include <QtCore/QStringList>
#include <QtCore/QDateTime>

typedef QPair<int, std::vector<double> > ixx_values;



SeasonBuffer::SeasonBuffer(int dt, double *data){
	this->dt=dt;
	this->data = data;
}
SeasonBuffer::~SeasonBuffer(){
	delete [] data;
}
void SeasonBuffer::addtoOut(int *dt, Flow *out){
	if(*dt>this->dt){
		for(size_t i=0; i<Flow::size(); i++){
			(*out)[i]+=data[i];
		}
		*dt-=this->dt;
		this->dt=0;
	}
	else{
		for(size_t i=0; i<Flow::size(); i++){
			(*out)[i]+=data[i]/this->dt*(*dt);
			data[i]-=data[i]/this->dt*(*dt);
		}
		this->dt-=*dt;
		*dt=0;
	}
}


SeasonContext::SeasonContext(){
	availableDt=0;
}

SeasonContext::~SeasonContext(){
	this->clear();
}

void SeasonContext::take(int dt, Flow *out){
	this->availableDt-=dt;
	while(dt!=0){
		buffer.front()->addtoOut(&dt, out);
		if(dt!=0){
			delete buffer.front();
			buffer.pop();
		}
	}
}

void SeasonContext::put(int dt, double *data){
	buffer.push(new SeasonBuffer(dt, data));
	this->availableDt+=dt;
}

int SeasonContext::available(){
	return availableDt;
}

void SeasonContext::clear(){
	availableDt=0;
	while(!buffer.empty()){
		delete buffer.front();
		buffer.pop();
	}
}

CD3_DECLARE_NODE_NAME(SeasonQRead)

SeasonQRead::SeasonQRead() {
	addParameter(ADD_PARAMETERS(rain_folder));
	addOutPort(ADD_PARAMETERS(out));
	file = 0;
}

SeasonQRead::~SeasonQRead() {
}



QDateTime pttoqt3(const boost::posix_time::ptime &pt) {
	boost::gregorian::date gd = pt.date();
	boost::posix_time::time_duration gt = pt.time_of_day();
	QDate qdate(gd.year(), gd.month(), gd.day());
	QTime qtime(gt.hours(), gt.minutes(), gt.seconds());

	return QDateTime(qdate, qtime);
}

ixx_values standardValue(){
	ixx_values v;
	v.first = 86400;
	v.second = std::vector<double>(1);
	return v;
}

bool SeasonQRead::init(ptime start, ptime end, int dt) {
	(void) end;

	if(dt>86400)
		Logger(Error) << "dt oversteps a day!"; //can this happen?

	folder = QDir(QString::fromStdString(rain_folder));
	if (!folder.exists()) {
		Logger(Error) << "can not open folder:" << rain_folder;
		return false;
	}

	QStringList subfolderlist = folder.entryList(QStringList(),
												 QDir::Dirs | QDir::NoDotAndDotDot);
	QString folders[12];
	Q_FOREACH(QString foldername, subfolderlist){
		QStringList monthlist = foldername.split(QRegExp("\\D+"));
		Q_FOREACH(QString month, monthlist){
			bool ok;
			int m = month.toInt(&ok);
			if(ok && m>0 && m<=12){
				folders[m-1] = foldername;
			}
		}
	}
	for(int m=0; m<12; m++){
		if(folders[m]!=QString()){
			QDir subfolder = folder.absoluteFilePath(folders[m]);
			QStringList filelist = subfolder.entryList(QDir::Files);
			Q_FOREACH(QString fs, filelist){

				QString filename = fs;
				//crop start and end till there are only numbers and the separators left
				bool ok = false;
				while(!ok && !filename.isEmpty()){
					filename.right(1).toInt(&ok);
					if(!ok)
						filename.chop(1);
				}
				ok = false;
				while(!ok && !filename.isEmpty()){
					filename.left(1).toInt(&ok);
					if(!ok)
						filename.remove(0, 1);
				}
				QStringList daylist = filename.split(QRegExp("\\D+"));
				Q_FOREACH(QString day, daylist){
					bool ok;
					int d = day.toInt(&ok);
					if(ok && d>0 && d<=7){
						foldermap.insert(QPair<int, int>(m+1, d), folder.absoluteFilePath(folders[m])+QDir::separator()+fs);
					}
				}
			}
		}
	}
	if(foldermap.count() !=84){
		for(int m =1; m <= 12; m++){
			for(int n=1; n <= 7; n++){
				if(foldermap.value(QPair<int, int>(m, n), QString()) == QString())
					Logger(Warning) << "missing file for month " << m << ", day " << n << "my out will be 0 during this time";
			}
		}
	}

	lastDay = MonthDay(-1, -1);

	QDateTime begin = pttoqt3(start);
	begin = begin.addSecs(-dt);

	int day = begin.date().dayOfWeek();
	int month = begin.date().month();

	int secondstoskip = QTime(0,0).secsTo(begin.time());
	if(secondstoskip!=0){
		while(ctxt.available() < secondstoskip){	//fill buffer until we reach the current time...
			double *data = new double[Flow::size()];
			int filet;
			this->parseLine(month, day, data, &filet);
			ctxt.put(filet, data);
		}
		ctxt.take(secondstoskip, &out);				//...and skip till we reach the current time
	}

	return true;
}

void SeasonQRead::deinit(){
	foldermap.clear();
	if(file){
		delete file;
		file = 0;
	}
}

int SeasonQRead::f(ptime time, int dt) {
	QDateTime after = pttoqt3(time);
	QDateTime start = after.addSecs(-dt);


	out.clear();

	int totake=dt;

	if(after.date() != start.date()){                                   //take care of more dates in one step

		int day = start.date().dayOfWeek();
		int month = start.date().month();

		//fill buffer with values of the first date
		while(ctxt.available() < dt){
			double *data = new double[Flow::size()];
			int filet;
			this->parseLine(month, day, data, &filet);
			ctxt.put(filet, data);
		}
		int timetillmidnight = 86400-QTime(0, 0).secsTo(start.time());
		ctxt.take(timetillmidnight, &out);
		ctxt.clear();
		totake=dt-timetillmidnight;

	}

	int day = after.date().dayOfWeek();
	int month = after.date().month();

	//fill buffer
	while(ctxt.available() < totake){
		double *data = new double[Flow::size()];
		int filet;
		parseLine(month, day, data, &filet);
		ctxt.put(filet, data);
	}

	ctxt.take(totake, &out);
	if(out[0]!=0){
		for(size_t j=1; j<Flow::size(); j++){
			out[j]/=out[0];
		}
	}
	return dt;
}

void SeasonQRead::parseLine(int month, int day, double *data, int *dt){

	for(size_t i=0; i<Flow::size(); i++){
		data[i]=0;
	}

	//is the current file still valid? delete if not
	if(file){
		if(file->fileName() != foldermap.value(MonthDay(month, day), QString())){
			delete file;
			file = 0;
		}
		else if(MonthDay(month, day) != lastDay){
			file->seek(0); //use the same file as before but reset reading position if the day changed!
		}
	}

	lastDay = MonthDay(month, day);


	//got nothing for this day? return a full day of no input!
	if(foldermap.value(MonthDay(month, day), "") == QString())
		return;

	//open the file for the current month and day!
	if(!file){
		file = new QFile(foldermap.value(QPair<int, int>(month, day), ""));
		//an errormessage and a day of no input for bitchy files
		if(!file->open(QIODevice::ReadOnly)){
			Logger(Error) << "can not open file: " << foldermap.value(MonthDay(month, day), QString());
			return;
		}
	}

	bool ok;
	QString line;
	QStringList valuestring;
	//parse dt
	bool completlyparsed = false;
	Q_FOREVER{                              //skip void and senseless lines
		if(file->atEnd() && !completlyparsed){  //if the file ends
			file->seek(0);                      //return to start
			completlyparsed = true;}
		else if(file->atEnd()){
			Logger(Error) << "no values in file: " << foldermap.value(MonthDay(month, day), QString());
			foldermap.remove(MonthDay(month, day));
			return;
		}
		line = file->readLine();
		valuestring = line.split(";"); //TODO: RegExp here?
		if(valuestring.size()>0){
			*dt = valuestring.first().toInt(&ok);
			if(ok)
				break;
		}
	}
	valuestring.pop_front();

	//got a dt, lets parse Q and C
	size_t j;
	int i=0;
	for(j=0; j<Flow::size(); j++){
		ok=false;
		while(!ok){
			if(i>=valuestring.size())
				break;
			data[j]=valuestring[i].toDouble(&ok);
			i++;
		}
	}

	data[0]*=*dt;
	if(data[0]!=0){
		for(j=1; j<Flow::size(); j++){
			data[j]*=data[0];
		}
	}
}
