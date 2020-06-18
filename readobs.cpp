#include "readobs.h"


OBS::OBS(){

}

//---

void count_match(QStringList &ret, const std::string& user_string, const std::string& user_pattern, const std::string& flags = "o" ){

const bool flags_has_i = flags.find( "i" ) < flags.size();
const bool flags_has_g = flags.find( "g" ) < flags.size();

std::regex::flag_type regex_flag = flags_has_i ? std::regex_constants::icase : std::regex_constants::ECMAScript;
std::regex rx( user_pattern, regex_flag );
std::match_results< std::string::const_iterator > mr;

unsigned int counter = 0;
std::string temp = user_string;

while( std::regex_search( temp, mr, rx ) ){
    temp = mr.suffix().str();
    ret.push_back(mr[1].str().c_str());
    ++counter;
    }
}

//---

OBSthread::OBSthread(QString str,Ltype inL_){
    file_ = str;
    L_ = inL_;
}

//---
void OBSthread::read_2_11(fstream &rnx){
    cout<<"!!";
    OBS file;
    bool stop = false;
    int num = 1;
    num++;
    cout << num << endl;
    while (!rnx.eof() and stop == false) {

        cout << num << endl;
        num++;

        string s;
        getline(rnx, s);
        smatch m;

        if(regex_search(s, m, regex("\\s{1,}(\\d{1,})\\s.*# / TYPES OF OBSERV"))){
            file.length = stoi(m[1]);
            int snum = stoi(m[1])/9;
            for (int i=0; i<=snum; i++) {
                smatch n;
                count_match(file.ObsType, s, "([A-Z][0-9])", "g");
                getline(rnx, s);
            }
            continue;
        }

//        if(regex_search(s, regex("???TIME OF FIRST OBS"))){
//            cout << "!!!";
//            QDate DFirst;
//            QTime TFirst;
//            DFirst.setDate(su(str), stoi(m[2]), stoi(m[3]));
//            TFirst.setHMS(stoi(m[4]), stoi(m[5]), stoi(m[6]));
//            file.begin.setDate(DFirst);
//            file.begin.setTime(TFirst);
//        }
//        if(regex_search(s, m, regex("???TIME OF LAST OBS"))){
//            QDate DLast;
//            QTime TLast;
//            DLast.setDate(stoi(m[1]), stoi(m[2]), stoi(m[3]));
//            TLast.setHMS(stoi(m[4]), stoi(m[5]), stoi(m[6]));
//            file.end.setDate(DLast);
//            file.end.setTime(TLast);
//            cout << file.end.toString().toStdString() << endl;
//        }

        if(regex_search(s, regex("END OF HEADER"))){
            stop = true;
        }
    }
}


#include <sstream>
#include <vector>
#include <string>
#include <utility>

#define RNX_MEAS_DATA vector<pair<string, double const *>>



/*	Как может выглядеть функция, которая парсит ринекс измерения
 *  input:
 *		fstream const & 		- адрес константного fstream объекта
 *		vector<const string> &	- адрес массива строк с именами параметров, которые тебе нужны (L1, L2 и т.д.)
 *	output:
 *		RNX_MEAS_DATA 	- тот самый массив данных, который тебе надо прочитать
*/
RNX_MEAS_DATA ParseRinexObs(fstream const &fRef, vector<const string> &measParams){
	// Создаём объект, в котором будем хроанить распарсенные данные
	RNX_MEAS_DATA measData;
	
	vector<string> fMeasParams;
	bool isHeaderEnd {false};
	bool stop {false};
	string fString {};
	smatch regMatch;
	
	// Ищем в заголовке типы измерений
	while(!fRef.eof() && !isHeaderEnd && !regex_match(fString, ".*# / TYPES OF OBSERV")){
		isHeaderEnd = regex_match(fString, ".*END OF HEADER");
		getline(fRef, fString);
	}
	while(!fRef.eof() && !isHeaderEnd && regex_search(fString, regMatch, ".*# / TYPES OF OBSERV")){
		// Тут читаешь параметры измерений (L1, L2, P1, P2)
		//		Если параметр есть в списке measParams, добавляешь его в fMeasParams
		//		Так можно сделать чтение произвольного числа параметров
		getline(fRef, fString);
	}
	while(!fRef.eof() && !isHeaderEnd){
		isHeaderEnd = regex_match(fString, ".*END OF HEADER");
		getline(fRef, fString);
	}
	
	// Если заголовок закончился, а параметры так и не нашлись
	//		Возвращаем пустой массив
	if (isHeaderEnd && fMeasParams.size() == 0){
		return RNX_MEAS_DATA();
	}

	// Читаем массив измерений
	vector<string> satList {};		// List of satellites in 
	while(!fRef.eof() && !stop){
		getline(fRef, fString);
		
		if(isEphHeader(fString)){
			ReadHeader(fString, ephMjd, satList);
		}else{
			ReadEphMeas(fString, measData);
		}
	}
	
	return measData;
}



void OBSthread::run(){
    fstream rnx = OBS().loadOBS(file_);
    string ver;
    bool stop = false;

    while (!rnx.eof() and stop == false) {
        string s;
        getline(rnx, s);
        smatch m;
        if(regex_search(s, m, regex("\\s*(\\S*)\\s*.*RINEX VERSION / TYPE"))){
            cout<<m[1]<<endl;
            ver = m[1];
            stop = true;
            continue;
        }
    }
    if(ver == "2.11"){
        read_2_11(rnx);
    }
    else {
        cout<<qPrintable("Неизвестная версия Rinex")<<endl;
    }
    emit sendData(L_);
    emit finished(-1);
}

//------------------------------------------------------------------------------------------

SP3thread::SP3thread(QString str,SPtype inXYZ_){
    file_ = str;
    XYZ_ = inXYZ_;
}

//---

void SP3thread::run(){
    fstream filest = OBS().loadOBS(file_);

    QMap<int, vector<double>> sat;
    QDateTime epoch;
    int number = 0;

    while (!filest.eof()) {
        string str;
        smatch m;
        getline(filest, str);
        QDate date;
        QTime time;

        if(regex_search(str, m, regex("\\*\\s*(20\\w*)\\s*(\\w*)\\s*(\\w*)\\s*(\\w*)\\s*(\\w*)\\s*(\\w*).*"))){
            sat.clear();


            cout<<m[1]<<" "<<m[2]<<" "<<m[3]<<" "<<m[4]<<" "<<m[5]<<" "<<m[6]<<endl;

            date.setDate(stoi(m[1]), stoi(m[2]), stoi(m[3]));
            time.setHMS(stoi(m[4]),stoi(m[5]),stoi(m[6]));
            epoch.setDate(date);
            epoch.setTime(time);
            number++;
            emit send(floor((number/2880.0)*100));
        }

        if(regex_search(str, m, regex("PG0*(\\S*)\\s*(\\S*)\\s*(\\S*)\\s*(\\S*)\\s*(\\S*)\\s*"))){
            sat[stoi(m[1])] =  {stod(m[2]), stod(m[3]), stod(m[4])};
            sat[4] =  {0, 0, 0};
            cout<<stoi(m[1])<<" | "<<sat[stoi(m[1])][0]<<"  "<<sat[stoi(m[1])][1]<<"  "<<sat[stoi(m[1])][2]<<endl;
            XYZ_[epoch] = sat;
        }
    }

    emit sendData(XYZ_, Interpol(XYZ_));
    emit finished(-2);
}

//---

SPtype SP3thread::Interpol(SPtype &XYZ){
    SPtype xyzInt;

    for (int num=1; num<=32; num++) {
        int n=5, m=5;
        QMapIterator<QDateTime,QMap<int, vector<double>>> i(XYZ);
        QVector<double> X, Y, Z, L, time;

        int e=0;
        i.toFront();
        while (i.hasNext()) {
            e++;
            i.next();
            if(i.value()[num].empty()) continue;
            time.append(e);
            X.append(i.value()[num][0]);
            Y.append(i.value()[num][1]);
            Z.append(i.value()[num][2]);
        }
        QDateTime DT;
        DT = XYZ.firstKey();

        for (double XL=0; XL<=95; XL+=1.0/30) {

            if(XL<=5){m=0; n=10;}
            else {m=5; n=5;}
            if(XL>=45){m=10; n=0;}

            double sumX, sumY, sumZ;
            sumX=0; sumY=0; sumZ=0;
            for (int i=floor(XL)-m; i<floor(XL)+n; i++)
                    {
                        double l = 1;
                        for (int j=floor(XL)-m; j<floor(XL)+n; j++)
                        {
                            if(i!=j)
                                l = l*(XL-j)/(i-j);
                        }
                        sumX = sumX + X[i]*l;
                        sumY = sumY + Y[i]*l;
                        sumZ = sumZ + Z[i]*l;
                    }
            xyzInt[DT][num]={sumX, sumY, sumZ};
            cout<<DT.toString().toStdString()<<endl;
            DT = DT.addSecs(30);

        }
        cout<<endl;
    }
    return xyzInt;
}


//------------------------------------------------------------------------------------------

fstream OBS::loadOBS(QString OBS_path){
    fstream file;
    file.open(OBS_path.toStdString(), ios::in);
    if (!file.is_open())
    {
        cout <<"!!!"<<endl;
    }
    return file;
}

//------------------------------------------------------------------------------------------

//double Po(QMapIterator<QDateTime,QMap<int, vector<double>>> c,
//          boost::numeric::ublas::vector<double> XYZ,
//          int num){
//    return sqrt(pow((c.value()[num][0]-XYZ[0]),2)+
//                pow((c.value()[num][1]-XYZ[1]),2)+
//                pow((c.value()[num][2]-XYZ[2]),2));
//}

void Solve::getSolve(){

//    boost::numeric::ublas::matrix<double> A;
//    boost::numeric::ublas::vector<double> X;
//    boost::numeric::ublas::vector<double> XYZ0(4);
//    XYZ0(0) = 0; XYZ0(1) = 0; XYZ0(2) = 0; XYZ0(3) = 0;
//    boost::numeric::ublas::vector<double> L, P0;
//    boost::numeric::ublas::vector<double> Ux, Uy, Uz;
    //ublas::vector<double> V;
//    QMapIterator<QDateTime,QMap<int, vector<double>>> c(coordInt);
//    QMapIterator<QDateTime,QMap<int, double>> s(L1);
//    c.toFront();
//    s.toFront();

//    while (s.hasNext() and c.hasNext()) {
//        s.next();
//        c.next();
//        for (int num=1; num<=32; num++) {
//            //P0[num]=Po(c,XYZ0,num);
//            //cout<<P0[num]<<endl;
//        }

//        cout<<"----------"<<endl;
//        for (int num=1; num<=32; num++) {
//            if(s.value()[num]!=0){
//            }
//        }
//    }
}

//------------------------------------------------------------------------------------------




