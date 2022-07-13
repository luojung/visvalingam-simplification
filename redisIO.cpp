// 这是主 DLL 文件。


#include <qstringlist.h>
#include<iostream>
#include "redisIO.h"
#include<time.h>
redisIO::redisIO()
{
    ip="localhost";
    port=6379;
    redisDriver = new QtRedis(QString::fromStdString(ip),port);
    //计时开始
    clock_t startTime,endTime,durationTime;
    startTime = clock();
    //****************************************

    if (!redisDriver->openConnection())
    {
        qDebug() << "Could not connect to server...";
        exit(0);
    }

    //计时结束(单位ms，1s=1000ms)
    endTime = clock();
    durationTime=endTime-startTime;
    startTime = clock();
    //********************************************

    qDebug() << "Connected to server...";
}

redisIO::~redisIO()
{
    delete redisDriver;
}


int redisIO::rpush(std::string trackname,vector<pair<double,double> > points)
{

    //计时开始
    clock_t startTime,endTime,durationTime;
    startTime = clock();
    //****************************************

    string cmdvalue="";

    for(int i=0;i<points.size();i++)
    {
        char buf[256];
        sprintf(buf,"%lf,%lf",points[i].first,points[i].second);
        cmdvalue+=buf;
        cmdvalue+=" ";
    }
    //计时结束(单位ms，1s=1000ms)
    endTime = clock();
    durationTime=endTime-startTime;
    //*************************************


     return redisDriver->rpush(QString::fromStdString(trackname),QString::fromStdString(cmdvalue));
}
void SplitString(std::string strSource,char splitchar,std::vector<std::string>& strSub)
{
	int nPos = strSource.find_first_of(splitchar);
	while (nPos!=std::string::npos)
	{
		int nLen = strSource.length();
		std::string strSub1 = strSource.substr(0,nPos);
		strSub.push_back(strSub1);
		strSource = strSource.substr(nPos+1,nLen-nPos-1);
		nPos = strSource.find_first_of(splitchar);
	}
	strSub.push_back(strSource);
}

pair<double,double> redisIO::pop(std::string key,bool &IsBlank)
{
    QStringList result;
    if(redisDriver->exists(QString::fromStdString(key)))
        result=redisDriver->blpop(QString::fromStdString(key),1);

    std::pair<double,double> point;
    if (!result.empty())
	{
		IsBlank=0;
        std::string pointStr=result.at(1).toStdString();

		//分割字符串
		std::vector<std::string> vecMetas;
        SplitString(pointStr,',',vecMetas);
		//转化字符串的坐标为double型
		point.first=atof(vecMetas[0].c_str());
		point.second=atof(vecMetas[1].c_str());
		//输出
		return point;
	}
	IsBlank=1;
	return point;

}

vector<pair<double,double> > redisIO::popAll(string key, bool &IsBlank)
{
    QStringList * result=new QStringList();
    int len;
    if(redisDriver->exists(QString::fromStdString(key)))
    {
        len=redisDriver->llen(QString::fromStdString(key));

        for(int i=0;i<len/280+1;i++)
        {
            *result+=redisDriver->lrange(QString::fromStdString(key),280*i,280*(i+1)-1);
        }
        redisDriver->del(QString::fromStdString(key));

        //输出长度
         std::cout<<"points before simplication:"<<result->length()<<endl;
    }

    vector< pair<double,double> > points;
    std::pair<double,double> point;
    if (!result->empty())
    {
        IsBlank=0;

        for(int i=0;i<result->length();i++)
        {
            QString QstrPoint=result->at(i);
            std::string pointStr=result->at(i).toStdString();
            //分割字符串
            std::vector<std::string> vecMetas;       
            SplitString(pointStr,',',vecMetas);
//            std::cout<<pointStr<<":  "<<vecMetas[0]<<" "<<vecMetas[1]<<endl;
            //转化字符串的坐标为double型
            point.first=atof(vecMetas[0].c_str());
            point.second=atof(vecMetas[1].c_str());
            points.push_back(point);
        }
        delete result;
        //输出
        return points;
    }
    delete result;
    IsBlank=1;
    return points;
}

QStringList * redisIO::getZset(string key, bool &IsBlank)
{
    QStringList * result=new QStringList();
    int len;
    if(redisDriver->exists(QString::fromStdString(key)))
    {
        len=redisDriver->zlen(QString::fromStdString(key));
        qDebug()<<"the total of points:"<<len;

        for(int i=0;i<=(len/280);i++)
        {
            *result+=redisDriver->zrange(QString::fromStdString(key),280*i,280*(i+1)-1);
        }
//        *result+=redisDriver->zrange(QString::fromStdString(key),1900,1950);

        //输出长度
//         std::cout<<"points before simplication:"<<result->length()<<endl;
    }
    if (!result->empty())
    {
        IsBlank=0;
    }
    else
        IsBlank=1;

    return result;
}
