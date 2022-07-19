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


class /*_declspec(dllexport)*/ Simplication
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
	//轨迹线名
	std::string trackname;
	//redis对象
    redisIO *redis;
public:
	Simplication();
	Simplication(std::string trackname);
	~Simplication();
    int addPoints(vector<pair<double,double> >);
	bool readRedis();	
    TrackPoint * simplify();
    void setResolution(int Resolution);
    int getCountOfPoint()
    {
        return cout;
    }

private:
    int initial();
	int Resolution;

};

