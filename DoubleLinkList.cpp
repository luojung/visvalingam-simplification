
#include "DoubleLinkList.h"

//节点类
template<class T>
DoubleListNode<T>::DoubleListNode()
{
	pre=NULL;
	next=NULL;
}

template<class T>
DoubleListNode<T>::DoubleListNode(T item)
{
	nValue=item;
	pre=NULL;
	next=NULL;
}

template<class T>
DoubleListNode<T>::~DoubleListNode()
{
	pre=NULL;
	next=NULL;
}

//重载大于运算符
template<class T>
bool DoubleListNode<T>::operator >(DoubleListNode<T> a)
{
	if (nValue>a.nValue)
		return true;
	return false;
}

//重载<=运算符
template<class T>
bool DoubleListNode<T>::operator <=(DoubleListNode<T> a)
{
	if (nValue<=a.nValue)
		return true;
	return false;
}

//重载==运算符
template<class T>
bool DoubleListNode<T>::operator ==(DoubleListNode<T> a)
{
	return nValue==a.nValue;
}

//重载==运算符
template<class T>
DoubleListNode<T> DoubleListNode<T>::operator =(DoubleListNode<T> a)
{
	nValue=a.nValue;
	pre=a.pre;
	next=a.next;
	return *this;
}




//链表类
template<class T>
DoubleLinkList<T>::DoubleLinkList()
{
	//head =new DoubleListNode<T>();
    //maxSize=1024000;
	//nLength=0;
	head =new DoubleListNode<T>();
    maxSize=1024000;
	nLength=0;
}

template<class T>
DoubleLinkList<T>::DoubleLinkList(int size)
{
	head =new DoubleListNode<T>();
	maxSize=size;
	nLength=0;
}

template<class T>
DoubleLinkList<T>::~DoubleLinkList()
{
	DestroyDoubleLinkList();
	//delete head;
}

template<class T>
DoubleListNode<T>* DoubleLinkList<T>::Locate(int nPos)
{
	DoubleListNode<T> *p = head;//head和p指向共同的内容,头结点无数据 

	if (nPos < 0)  
	{  
		cerr<<"位置参数有错误"<<endl;  
		return NULL;  
	}  

	int i = 0;  
	while (p != NULL && i < nPos)  
	{  
		p = p->next;  
		i++;  
	}  

	return p;  
}


template<class T>
bool DoubleLinkList<T>::InsertNode(T item,int nPos)
{
	if (Length() >= maxSize)  
	{  
		cout<<"错误：双链表已满"<<endl;  
		exit(1);  
	} 
	DoubleListNode<T> *newNode=new DoubleListNode<T>(item);
	if (newNode==NULL)
	{
		cerr<<"内存分配错误"<<endl;
		exit(1);
	}

	if (nPos>0)
	{
		DoubleListNode<T> *p=Locate(nPos);

		if (p->next==NULL)//如果当前节点是末尾节点的话
		{
			newNode->pre=p;
			p->next=newNode;
			tail=newNode;

		}else
		{
			newNode->pre=p;
			newNode->next=p->next;
			p->next=newNode;
			p->next->pre=newNode;
		}
	}
	else
	{
		if(nLength==0)
		{
			newNode->pre=head;
			head->next=newNode;
			tail=newNode;
		}
		else
		{
			newNode->pre=tail;
			tail->next=newNode;
			tail=newNode;

		}
		
		
	}
	
	nLength++;

	return true;
}


template<class T>
void DoubleLinkList<T>::DestroyDoubleLinkList()
{
	DoubleListNode<T> *pMove=head->next;
	DoubleListNode<T> *pDel;
	while (pMove!=NULL)
	{
		pDel=pMove;
		pMove=pMove->next;
		delete pDel;
	}
	head->next=NULL;//这句话必须有，不然有错误
}


template<class T>
int DoubleLinkList<T>::Length()
{
	return nLength;
}


template<class T>
void DoubleLinkList<T>::PrintList()
{
	int count = 0;  
	DoubleListNode<T> *p = head;  
	while (NULL != p->next)  
	{  
		p = p->next;  
		cout << p->nValue << " ";  
		if (++count % 15 == 0)  //每隔十个元素，换行打印  
			cout << endl;  
	}  
}


template<class T>
bool DoubleLinkList<T>::RemoveNode(int nPos)
{
	if ((nPos+1)>Length())
	{
		cerr<<"位置参数错误"<<endl;
		exit(1);
	}
	DoubleListNode<T> *p=Locate(nPos);
	DoubleListNode<T> *del=p->next;

	if (del->next==NULL)
	{
		p->next=NULL;
		delete del;
		nLength--;
		return true;
	}
	else
	{
		p->next=del->next;
		del->next->pre=p;
		delete del;
		nLength--;
		return true;
	}
}


template<class T>
T DoubleLinkList<T>::getData(int nPos)
{
	if ((nPos+1)>Length())
	{
		cerr<<"位置参数错误"<<endl;
		exit(1);
	}
	DoubleListNode<T> *p=Locate(nPos);

	return  p->next->nValue;

}


template<class T>
bool DoubleLinkList<T>::IsEmpty()
{
	if (Length()==0)
	{
		return true;
	}else
	{
		return false;
	}
}

template<class T>
void DoubleLinkList<T>::Reverse()
{
	for (int i=1;i<=Length()/2;i++)
	{
		DoubleListNode<T> *p=Locate(i);
		DoubleListNode<T> *q=Locate(nLength-i);
		T temp;
		temp=p->nValue;
		p->nValue=q->next->nValue;
		q->next->nValue=temp;
	}

}
