#pragma once

#include "iostream"
using namespace std;

template<class T>class DoubleLinkList;

template <class T>
class DoubleListNode
{
public:
	DoubleListNode();
	DoubleListNode(T item);
	~DoubleListNode();

	//重载运算符
	bool operator >(DoubleListNode a);
	bool operator <=(DoubleListNode a);
	bool operator ==(DoubleListNode a);
	DoubleListNode  operator =(DoubleListNode a);

public:
	friend class DoubleLinkList<T>;
	T nValue;
	DoubleListNode<T> *pre;
	DoubleListNode<T> *next;
};

template<class T>
class DoubleLinkList
{
public:
	DoubleLinkList();
	DoubleLinkList(int size);
	~DoubleLinkList();
	
	//定位指定的位置，返回该位置上的结点指针  
	DoubleListNode<T>* Locate(int nPos); 
	//摧毁双链表
	void DestroyDoubleLinkList();
	//获得双链表长度
	int Length();
	//插入节点
	bool InsertNode(T item,int nPos=-1);
	//打印双链表
	void PrintList();
	//删除结点
	bool RemoveNode(int nPos);
	//获得指定位置的数据
	T getData(int nPos);
	//判断空否
	bool IsEmpty();
	//链表逆置  
	void Reverse(); 

	DoubleListNode<T> *head;
	DoubleListNode<T> *tail;

private:
	
	int maxSize;
	int nLength;

};


