#include "seasonqread.h"
#include "cd3assert.h"
#include <fstream>
#include <QtCore/QStringList>
#include <QtCore/QDateTime>

typedef QPair<int, std::vector<double> > ixx_values;



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
		while(buffer.available() < secondstoskip){    //fill buffer until we reach the current time...
            ixx_values v = parseLine(month, day);
            buffer.put(v.first, v.second);
        }

        buffer.take(secondstoskip);                 //...and skip till we reach the current time
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





	if(after.date() != start.date()){                                   //take care of more dates in one step

        int day = start.date().dayOfWeek();
        int month = start.date().month();

        //fill buffer with values of the first date
        while(buffer.available() < dt){
            ixx_values v = parseLine(month, day);
            buffer.put(v.first, v.second);
        }
        int timetillmidnight = 86400-QTime(0, 0).secsTo(start.time());
        std::vector<double> temp = buffer.take(timetillmidnight);       //take till midnight
        buffer.clear();                                                 //remove the rest
        buffer.put(timetillmidnight, temp);                             //put back values from the day before(or file before)
                                                                        //so the buffer takes care of mixing
    }

    int day = after.date().dayOfWeek();
    int month = after.date().month();

    //fill buffer
    while(buffer.available() < dt){
        ixx_values v = parseLine(month, day);
        buffer.put(v.first, v.second);
    }

    std::vector<double> values = buffer.take(dt);
    for(unsigned int i=0; i<Flow::countUnits(Flow::concentration); i++){
        out[i+1] = 0;
    }
    for(unsigned int i=0; i<qMin(values.size(), Flow::countUnits(Flow::concentration)+1); i++){
        out[i] = values[i];
    }
    return dt;
}

ixx_values SeasonQRead::parseLine(int month, int day){

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
        return standardValue();

    //open the file for the current month and day!
    if(!file){
        file = new QFile(foldermap.value(QPair<int, int>(month, day), ""));
        //an errormessage and a day of no input for bitchy files
        if(!file->open(QIODevice::ReadOnly)){
            Logger(Error) << "can not open file: " << foldermap.value(MonthDay(month, day), QString());
            return standardValue();
        }
    }

    bool ok;
    ixx_values v;
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
            return standardValue();
        }
        line = file->readLine();
        valuestring = line.split(";"); //TODO: RegExp here?
        if(valuestring.size()>1){
            v.first = valuestring.first().toInt(&ok);
            if(ok)
                break;
        }
    }
    valuestring.pop_front();

    //got a dt, lets parse Q and C
    Q_FOREACH(QString s, valuestring){
		v.second.push_back(s.toDouble(&ok));
        if(!ok)
            v.second.pop_back();
    }
    if(v.second.empty())
        v.second.push_back(0.0);
	v.second[0] *= v.first; //"hotfix" for reading values in m3/s instead of m3
    return v;
}
