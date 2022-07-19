#include<time.h>
#include<QtRedis>
#include<QDebug>
#include "simplicationTaxiTrajectory.h"

TTsimplifier::TTsimplifier()
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
TTsimplifier::TTsimplifier(std::string trackname1)
{
	trackname=trackname1;
    points=new TrackPoint[1024000];
    tree=new MinHeap<DoubleListNode<Triangle> >();
	cout=0;
    Resolution=3;
    redis=new redisIO();
}

TTsimplifier::~TTsimplifier()
{
	delete tree;
    delete redis;
	tree=NULL;
}
int TTsimplifier::addPoints(vector<pair<double,double> > points)
{

    return redis->rpush(trackname,points);
}

bool TTsimplifier::readRedis()
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

void TTsimplifier::SplitString(std::string strSource,char splitchar,std::vector<std::string>& strSub)
{
    int nPos = strSource.find_first_of(splitchar);
    while (nPos!=std::string::npos)
    {
        int nLen = strSource.length();
        std::string strSub1 = strSource.substr(0,nPos);
//        qDebug()<<QString::fromStdString( strSub1)<<endl;
        strSub.push_back(strSub1);
        strSource = strSource.substr(nPos+1,nLen-nPos-1);
        nPos = strSource.find_first_of(splitchar);
    }
//   qDebug()<<QString::fromStdString(strSource)<<endl;
    strSub.push_back(strSource);
}

bool TTsimplifier::readTaxiTrajectoryFormRedis()
{
    bool isblank;
    QStringList * result=redis->getZset(trackname,isblank);
    if (isblank)
        return 0;

    if (!result->empty())
    {
        for(int i=0;i<result->length();i++)
        {
            std::string pointStr=result->at(i).toStdString();
            //分割字符串
            std::vector<std::string> vecMetas;
            SplitString(pointStr,',',vecMetas);

            //转化字符串的坐标为double型
            points[cout].x=atof(vecMetas[0].c_str());
            points[cout].y=atof(vecMetas[1].c_str());
            points[cout].v=atof(vecMetas[2].c_str());
            cout++;
        }
    }
    delete result;
}


int TTsimplifier::initial()
{
    double TotalArea=0;
	//计时开始
	clock_t startTime,endTime,durationTime;
	startTime = clock(); 

    readTaxiTrajectoryFormRedis();
	//计时结束
	endTime = clock();
	durationTime=endTime-startTime;	
//    std::cout<<"time for reading point :"<<durationTime<<"ms"<<endl;

    for (int i=1;i<=cout-2;i++)
	{
		Triangle triangle;
		triangle.vertexs[0]=points[i-1];
		triangle.vertexs[1]=points[i];
		triangle.vertexs[2]=points[i+1];
        if (triangle.caculateAre(addV)>0)
		{			
			triangles.InsertNode(triangle);
            TotalArea+=triangle.Area;
		}
	}
    //设置默认的阈值,阈值越大，越简化。阈值越小越精细
    switch(simplicationLevel)
    {
    case 21:
        Resolution=0; //0;//TotalArea/cout;
        break;
    case 0:
        Resolution=100*TotalArea/cout; //0;//TotalArea/cout;
        break;
    default:
    {
        Resolution=(TotalArea/cout) *150*(1.55-(simplicationLevel/20.0));
        break;
    }
    }
    qDebug()<<"TotalArea/cout:"<<TotalArea/cout<<"  simplicationLevel:"<<simplicationLevel<<"  Resolution:"<<QString::number( Resolution);
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
TrackPoint * TTsimplifier::simplify(bool isAddV,int Level)
{
    addV=isAddV;
    simplicationLevel=Level;
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
            PopNode.pre->nValue.caculateAre(addV);
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
            PopNode.next->nValue.caculateAre(addV);
			tree->insert(PopNode.next);

		}

        //根据设置阈值进行简化
//        if (MinArea>Resolution)
//			break;

        //21级别为显示原始轨迹
        if(Level==21)
            break;

        //通过设置比例进行简化 如1.66666%,简化为1/60；
        if(tree->length()<=cout*(0.0116666+0.005*Level))
            break;
	}

    //更新轨迹点
//    vector<pair<double,double> > pointsForRedis;
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

//        //将简化后的轨迹点存到pointsForRedis，然后将pointsForRedis写入redis
//        pair<double,double> PointForRedis;
//        PointForRedis.first=CoutNode->nValue.vertexs[1].x;;
//        PointForRedis.second=CoutNode->nValue.vertexs[1].y;;
//        pointsForRedis.push_back(PointForRedis);

        //移到下一个节点
        CoutNode=CoutNode->next;
	}
	cout=i;  //记录简化后点的个数

    //将pointsForRedis写入redis
//    addPoints(pointsForRedis);
	//计时结束	
	endTime = clock(); 
	durationTime=endTime-startTime;	

    return points;

}

void TTsimplifier::setResolution(int resolution)
{
    Resolution=resolution;

}
