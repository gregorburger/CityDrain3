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

#include "flowreadcontext.h"

#define FLOWREAD_VEROSE

FlowLine::FlowLine(QDateTime timestamp, int dt, double *data){
	this->timestamp=timestamp;
	this->data=data;
	this->dt=dt;
}
FlowLine::~FlowLine(){
	delete [] data;
#ifdef FLOWREAD_VERBOSE
	if(this->dt!=0)
		Logger(Debug) << dt << " seconds left and unused!";
#endif
}

int FlowLine::addtoOut(Flow *out, int *dt){
	int taken;
	if(*dt >= this->dt){
		for(size_t i=0; i<Flow::size(); i++){
				(*out)[i]+=this->data[i];
			}
		taken = this->dt;
		*dt-=this->dt;
		this->dt=0;
	}
	else{
		for(size_t i=0; i<Flow::size(); i++){
			(*out)[i]+=this->data[i]/this->dt**dt;
			data[i]-=this->data[i]/this->dt**dt;
		}
		this->dt-=*dt;
		taken = *dt;
		*dt=0;
	}
	return taken;
}



FlowReadContext::FlowReadContext(){
}

FlowReadContext::~FlowReadContext(){
}

bool FlowReadContext::addLine(){
	if(file.atEnd()){
#ifdef FLOWREAD_VERBOSE
		Logger(Debug) << "File at end";
#endif
		return false;
	}
	char line[1024]; //can line be long?
	memset(line, 0, 1024);
	file.readLine(line, 1023);
	QDateTime time = QDateTime(QDate(0,0,0),QTime(00,00));
	QString qline = QString::fromLatin1(line);
	qline.resize(dateformat.length());
	QString format = QString::fromStdString(dateformat);
	time = QDateTime::fromString(qline, format);
#ifdef FLOWREAD_VERBOSE
	Logger(Debug) << "parsed data: " << time.toString();
#endif

	if(!time.isValid()){
		Logger(Error) << "invalid date/time";
		return false;
	}

	size_t i=dateformat.length(), j=0;
	double *values = new double[Flow::size()];
	for(size_t k =0;k<1+Flow::countUnits(Flow::concentration); k++)
		values[k]=0;
	while(i<=1023){
		while((line[i]==' ' || line[i]=='\t') && i<=1023)
			i++;
		if(line[i]=='\0' || line[i+1]=='\0')
			break;
		sscanf(line+i, "%lf", &values[j++]);
		if(j==Flow::size())
			break;
		while((line[i]!=' ' && line[i]!='\t' && line[i]!='\n') && i<=1023)
			i++;
		if(line[i]=='\0' || line[i+1]=='\0')
			break;
	}
	int dt;
	if(!buffer.empty()){
		dt=buffer.back()->timestamp.secsTo(time);
		this->availabe+=dt;
#ifdef FLOWREAD_VERBOSE
		if(dt>300){
			Logger(Debug) << "dt>300:" << buffer.back()->timestamp.toString() << time.toString();
		}
#endif
	}
	else{
		dt=-1;
	}
	if(!buffer.empty()){
		if(buffer.back()->dt==-1){
			buffer.back()->dt=dt;
			this->availabe+=dt;
		}
	}
	if(values[0]!=0){
		for(size_t i=1; i<Flow::size(); i++){
			values[i]*=values[0];
		}
	}
	else{
		for(size_t i=1; i<Flow::size(); i++){
			values[i]=0;
		}
	}

	buffer.push(new FlowLine(time, dt, values));
#ifdef FLOWREAD_VERBOSE
	Logger(Debug) << "pushed Flowline: time = " << time.toString() << ", dt = " << dt;
#endif
	return true;
}

void FlowReadContext::setOutPort(Flow *out, QDateTime time, int dt){
	//Logger(Debug) << "settingOutport, availibe = " << this->availabe;
	out->clear();
	while(dt!=0){
		while(this->availabe<=dt){
			//Logger(Debug) << "addingLine";
			if(!this->addLine())
				break;
		}
		if(buffer.empty()){
#ifdef FLOWREAD_VERBOSE
			Logger(Debug) << "Reached EOF";
#endif
			return;
		}
		int secsTo = time.secsTo(buffer.front()->timestamp)+dt-buffer.front()->dt;
		//seconds from begin sim timestep till measurement time step
#ifdef FLOWREAD_VERBOSE
		Logger(Debug) << "secsTo:" << secsTo << "->" << buffer.front()->timestamp.toString() << "-" << time.toString() << ", dt = " << buffer.front()->dt;
#endif
		if(secsTo>dt){
#ifdef FLOWREAD_VERBOSE
			Logger(Debug) << "skipping timestep " << time.toString(QString::fromStdString(dateformat));
#endif
			return;
		}
		else if(secsTo>0){
#ifdef FLOWREAD_VERBOSE
			Logger(Debug) << "skipping " << secsTo << " seconds of timestep " << time.toString(QString::fromStdString(dateformat));
#endif
			dt-=secsTo;
			time.addSecs(secsTo);
		}
		if(secsTo<0){
			this->availabe-=buffer.front()->addtoOut(out, &secsTo);
			if(buffer.front()->dt==0){
				delete buffer.front();
				buffer.pop();
			}
			out->clear();
#ifdef FLOWREAD_VERBOSE
			Logger(Debug) << "Removing Line in file";
#endif
		}

		this->availabe-=buffer.front()->addtoOut(out, &dt);
		if(buffer.front()->dt==0){
			delete buffer.front();
			buffer.pop();
		}
	}

	if((*out)[0]!=0){
		for(size_t i=1; i<Flow::size(); i++){
			(*out)[i]/=(*out)[0];
		}
	}
}
