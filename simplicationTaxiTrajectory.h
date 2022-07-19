#pragma once

#include <math.h>
#include "MinHeap.h"
#include "MinHeap.cpp"
#include "DoubleLinkList.h"
#include "DoubleLinkList.cpp"
#include "TrackPoint.h"
#include "Triangle.h"
#include"redisIO.h"
#include <list>
#include <vector>
#include <iostream>
using namespace std;

//浮动车轨迹点
class TaxiTrajectoryPoints
{
public:
    double longitude;
    double latitude;
    double time;
    double velocity;

};

class /*_declspec(dllexport)*/ TTsimplifier
{
private:
	MinHeap< DoubleListNode<Triangle> > *tree;
	double minArea;
	//list<Triangle> triangles;
    DoubleLinkList<Triangle> triangles;//(1024000);
	
	//轨迹点
    //TrackPoint points[1024000];
	TrackPoint *points;
	//点数
	int cout;
    //简化等级
    int simplicationLevel;
	//轨迹线名
	std::string trackname;
	//redis对象
    redisIO *redis;
    //是否将速度带入计算权重
    bool addV;
public:
    TTsimplifier();
    TTsimplifier(std::string trackname);
    ~TTsimplifier();
    int addPoints(vector<pair<double,double> >);
    void SplitString(std::string strSource,char splitchar,std::vector<std::string>& strSub);
    bool readTaxiTrajectoryFormRedis();
	bool readRedis();	
    TrackPoint * simplify(bool isAddV,int simplicationLevel);
    void setResolution(int Resolution);
    int getCountOfPoint()
    {
        return cout;
    }

private:
    int initial();
    double Resolution;

};

