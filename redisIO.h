// redisIO.h

#pragma once
#include <string>
#include <QtRedis>

using namespace std;


class redisIO
{
public:
	redisIO();
	~redisIO();

	//их╗пох>0│╔╣д
    int rpush(std::string trackName,vector<pair<double,double> > points);
	pair<double,double> pop(std::string key,bool &IsBlank);
    vector<pair<double,double> > popAll(std::string key,bool &IsBlank);
    QStringList * getZset(std::string key,bool &IsBlank);
private:
    string ip;
    int port;
    QtRedis *redisDriver;
};

	

