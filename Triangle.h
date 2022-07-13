#pragma once
#include "TrackPoint.h"
#include <math.h>
//三角形类
class Triangle
{
public:
	TrackPoint vertexs[3];
	double Area;
	int index;
	//构造函数
	Triangle();
	//拷贝构造函数
	Triangle(Triangle &);

	//计算面积
    double caculateAre();
    double caculateAre(bool addV);
	static int compareTriangle(Triangle,Triangle);
	double operator -(Triangle a);

	//重载运算符
	bool operator >(Triangle a);
	bool operator <=(Triangle a);
	bool operator ==(Triangle a);
	Triangle  operator =(Triangle a);
};
