
#include "TrackPoint.h"

TrackPoint::TrackPoint()
{

}

//复制构造函数
TrackPoint::TrackPoint(const TrackPoint & a)
{
	x=a.x;
	y=a.y;
    v=a.v;
	time=a.time;
}

TrackPoint::TrackPoint(double longitude,double latitude,long timeStamp)
{
	x=longitude;
	y=latitude;
	time=timeStamp;
}

//重载=
TrackPoint & TrackPoint::operator =(TrackPoint a)
{
	x=a.x;
	y=a.y;
    v=a.v;
	time=a.time;
	return *this;
}


