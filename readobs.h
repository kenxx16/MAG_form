#ifndef READOBS_H
#define READOBS_H
#include <fstream>
#include <iostream>
#include <string>
#include <QString>
#include <regex>
#include <QMap>
#include <QVector>
#include <QDateTime>
#include <math.h>
#include <QObject>
#include <QThread>
#include <QMetaType>
#include "boost/numeric/ublas/matrix.hpp"
#include "boost/numeric/ublas/vector.hpp"

using namespace std;
using namespace boost::numeric;

//---
typedef QMap<QDateTime, QMap<QString, QMap<QString, double>>> Ltype;
typedef QMap<QDateTime,QMap<int, vector<double>>> SPtype;



Q_DECLARE_METATYPE(Ltype);
Q_DECLARE_METATYPE(SPtype);

static Ltype L1;
static SPtype coord, coordInt;
//---


//-----------------------------------------------------------------------
class OBS
{
public:
    OBS();
    fstream loadOBS(QString);
    void getSP3(fstream &file, QMap<QDateTime,QMap<int, vector<double>>> &XYZ);
    int length;
    int interval;
    string name;
    QStringList ObsType;
    Ltype dat;
    QDateTime begin, end;
};

//----------Чтение--OBS---------------------------------------------------
class OBSthread : public QObject
{
    Q_OBJECT
public:
    OBSthread(QString, QMap<QDateTime, QMap<QString, QMap<QString, double>>>);
    void read_2_11(fstream &rnx);
    OBS regTypes(OBS);

public slots:
    void run();

signals:
    void finished(int);
    void send(int);
    void sendData(Ltype);
private:
    QString file_;
    Ltype L_;
};

//-----------------------------------------------------------------------

//----------Чтение--SP3--------------------------------------------------

class SP3thread : public QObject
{
    Q_OBJECT
public:
    SP3thread(QString, SPtype);
    SPtype Interpol(SPtype&);
public slots:
    void run();

signals:
    void finished(int);
    void send(int);
    void sendData(SPtype, SPtype);
private:
    QString file_;
    SPtype XYZ_;
};

//-----------------------------------------------------------------------
class Solve
{
public:
    Solve();

    static void getSolve();

};
#endif // READOBS_H
