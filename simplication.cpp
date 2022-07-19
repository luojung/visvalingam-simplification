#include<time.h>
#include<QtRedis>
#include "simplication.h"

Simplication::Simplication()
{
    tree=new MinHeap<DoubleListNode<Triangle> >();
	cout=0;
	Resolution=80;
    for (;cout<1024000;cout++)
	{
		double x=0.1*cout;
		points[cout].x=1+10*cout;
		points[cout].y=10*sin(1+2*x*x);
		cout++;
	}
}
Simplication::Simplication(std::string trackname1)
{
	trackname=trackname1;
    points=new TrackPoint[1024000];
    tree=new MinHeap<DoubleListNode<Triangle> >();
	cout=0;
    Resolution=3;
    redis=new redisIO();
}

Simplication::~Simplication()
{
	delete tree;
    delete redis;
    delete[] points;
	tree=NULL;
}
int Simplication::addPoints(vector<pair<double,double> > points)
{

    return redis->rpush(trackname,points);
}

bool Simplication::readRedis()
{
    bool isblank;
    vector<pair<double,double> >allpoint=redis->popAll(trackname,isblank);
    if (isblank)
        return 0;

    for (vector<pair<double,double> >::iterator it=allpoint.begin();it!=allpoint.end();it++)
	{
        points[cout].x=it->first;
        points[cout].y=it->second;
        cout++;
	}
	return 1;
}

int Simplication::initial()
{
    double TotalArea=0;
	//计时开始
	clock_t startTime,endTime,durationTime;
	startTime = clock(); 

	readRedis();
	//计时结束
	endTime = clock();
	durationTime=endTime-startTime;	
    std::cout<<"time for reading point :"<<durationTime<<"ms"<<endl;

	for (int i=1;i<cout-2;i++)
	{
		Triangle triangle;
		triangle.vertexs[0]=points[i-1];
		triangle.vertexs[1]=points[i];
		triangle.vertexs[2]=points[i+1];
		if (triangle.caculateAre())
		{			
			triangles.InsertNode(triangle);
            TotalArea+=triangle.Area;
		}
	}
    //设置默认的阈值
    Resolution=TotalArea/cout;

	DoubleListNode<Triangle> * currentNode;
	currentNode=triangles.head->next;
	while(currentNode!=NULL)
	{
		tree->insert(currentNode);
		currentNode=currentNode->next;
	}

	//计时结束
	endTime = clock();
	durationTime=endTime-startTime;	

	return 1;
}

//简化
TrackPoint * Simplication::simplify()
{
    initial();
	//计时开始
	clock_t startTime,endTime,durationTime;  
	startTime = clock();  

	//用二叉树排序，找出最小面积的三角形
    double MinArea=0;
	DoubleListNode<Triangle> PopNode;
	DoubleListNode<Triangle> *pPopNode;
	while (pPopNode=tree->pop())
	{
		PopNode=*pPopNode;
		//输出三角形节点的面积
//        DoubleListNode<Triangle> * CoutNode;
//        CoutNode=triangles.head->next; //&PopNode;//
//        while (CoutNode)
//        {
////            std::cout<<CoutNode->nValue.Area<<" ";
//            CoutNode=CoutNode->next;
//        }
//        std::cout<<endl;


		// If the area of the current point is less than that of the previous point
		// to be eliminated, use the latter’s area instead. This ensures that the
		// current point cannot be eliminated without eliminating previously-
		// eliminated points.
        if (PopNode.nValue.Area<MinArea)
            PopNode.nValue.Area=MinArea;
		else
            MinArea=PopNode.nValue.Area;

		if (PopNode.pre!=triangles.head&&PopNode.pre)
		{
			PopNode.pre->next=PopNode.next;
			PopNode.pre->nValue.vertexs[2]=PopNode.nValue.vertexs[2];
			//重新计算前驱节点的三角形面积，重新插入二叉堆排序
			tree->remove(PopNode.pre);
			PopNode.pre->nValue.caculateAre();
			tree->insert(PopNode.pre);

		}
        //*******************************************找错误
//		if (PopNode.pre==triangles.head)
//		{
//			PopNode.pre->next=PopNode.next;
//		}

		if (PopNode.next)
		{
			PopNode.next->pre=PopNode.pre;
			PopNode.next->nValue.vertexs[0]=PopNode.nValue.vertexs[0];
			//重新计算前驱节点的三角形面积，重新插入二叉堆排序
			tree->remove(PopNode.next);
			PopNode.next->nValue.caculateAre();
			tree->insert(PopNode.next);

		}

        if (MinArea>Resolution)
			break;
	}

    //更新轨迹点
    vector<pair<double,double> > pointsForRedis;
	DoubleListNode<Triangle> * CoutNode;
	CoutNode=triangles.head->next; //&PopNode;//
	int i=0;
	while (CoutNode)
	{
        //将简化后的轨迹点存到simplification的points里
		//std::cout<<CoutNode->nValue.Area<<" ";
		points[i].x=CoutNode->nValue.vertexs[1].x;
		points[i].y=CoutNode->nValue.vertexs[1].y;
		i++;

        //将简化后的轨迹点存到pointsForRedis，然后将pointsForRedis写入redis
        pair<double,double> PointForRedis;
        PointForRedis.first=CoutNode->nValue.vertexs[1].x;;
        PointForRedis.second=CoutNode->nValue.vertexs[1].y;;
        pointsForRedis.push_back(PointForRedis);

        //移到下一个节点
        CoutNode=CoutNode->next;
	}
	cout=i;  //记录简化后点的个数

    //将pointsForRedis写入redis
    addPoints(pointsForRedis);
	//计时结束	
	endTime = clock(); 
	durationTime=endTime-startTime;	

    return points;

}

void Simplication::setResolution(int resolution)
{
    Resolution=resolution;

}
