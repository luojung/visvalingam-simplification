#pragma once
//航迹点类，
class TrackPoint
{

public:
	TrackPoint();
	TrackPoint(const TrackPoint & a);
	TrackPoint(double longitude,double latitude,long timeStamp);
	
	//重载=
	TrackPoint & operator =(TrackPoint a);
    double x;  //经度
    double y;   //纬度
    double v;   //速度
    long time;  //时间
	
};
