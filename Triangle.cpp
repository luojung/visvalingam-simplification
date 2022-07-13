#include <iostream>
#include "Triangle.h"

//构造函数
Triangle::Triangle()
{
	Area=0;
	index=0;

};
//拷贝构造函数
Triangle::Triangle(Triangle & a)
{
	vertexs[0]=a.vertexs[0];
	vertexs[1]=a.vertexs[1];
	vertexs[2]=a.vertexs[2];
	Area=a.Area;
	index=a.index;

};

int Triangle::compareTriangle(Triangle a,Triangle b)
{
	return a.Area-b.Area;
}

double Triangle::operator -(Triangle a)
{
	return Area-a.Area;
}

//计算面积
double Triangle::caculateAre()
{
    Area=(10000*vertexs[0].x-10000*vertexs[2].x)*(10000*vertexs[1].y-10000*vertexs[0].y)
        -(10000*vertexs[0].x-10000*vertexs[1].x)*(10000*vertexs[2].y-10000*vertexs[0].y);
    Area=abs(Area);

    return Area;
}
double Triangle::caculateAre(bool addV)
{
    Area=(10000*vertexs[0].x-10000*vertexs[2].x)*(10000*vertexs[1].y-10000*vertexs[0].y)
        -(10000*vertexs[0].x-10000*vertexs[1].x)*(10000*vertexs[2].y-10000*vertexs[0].y);
    if(addV)
    {
       if(vertexs[1].v<1)
           vertexs[1].v=1;
//       std::cout<<"abs(Area): "<<Area<<"vertexs[1].v"<<vertexs[1].v<<std::endl;
       Area=Area/vertexs[1].v;
    }

    Area=fabs(Area);

	return Area;
}



//重载大于运算符
bool Triangle::operator >(Triangle a)
{
	if (Area>a.Area)
		return true;
	return false;
}

//重载<=运算符
bool Triangle::operator <=(Triangle a)
{
	if (Area<=a.Area)
		return true;
	return false;
}

//重载==运算符
bool Triangle::operator ==(Triangle a)
{
	return Area==a.Area
		&&vertexs[0].x==a.vertexs[0].x&&vertexs[0].y==a.vertexs[0].y&&vertexs[0].time==a.vertexs[0].time
		&&vertexs[1].x==a.vertexs[1].x&&vertexs[1].y==a.vertexs[1].y&&vertexs[1].time==a.vertexs[1].time
		&&vertexs[2].x==a.vertexs[2].x&&vertexs[2].y==a.vertexs[2].y&&vertexs[2].time==a.vertexs[2].time;
}

//重载=运算符
Triangle Triangle::operator =(Triangle a)
{
	vertexs[0]=a.vertexs[0];
	vertexs[1]=a.vertexs[1];
	vertexs[2]=a.vertexs[2];
	Area=a.Area;
	index=a.index;
	return *this;
}
