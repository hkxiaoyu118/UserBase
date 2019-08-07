#include <stdlib.h>
#include "winsock2.h"

// 节点
template <class T>class DNode
{
private:
	// 指向左、右结点的指针
	DNode<T>* left;
	DNode<T>* right;
public:
	// data 为公有成员 
	T data;

	// 构造函数
	DNode(void);
	DNode(const T& item);


	// 改变表的方法
	void InsertRight(DNode<T>* p);
	void InsertLeft(DNode<T>* p);
	DNode<T>* DeleteNode(void);


	// 取得指向左、右方向结点的指针
	DNode<T>* NextNodeRight(void) const;
	DNode<T>* NextNodeLeft(void) const;
};

// 创建空表并保留数据未定义，用于 header
template <class T>DNode<T>::DNode(void)
{
	// 初始化结点使其指向自身
	left = right = this;
}

// 创建空表并初始化数据
template <class T>DNode<T>::DNode(const T& item)
{
	// 建立一个指向自身的结点并初始化 data 域
	left = right = this;
	data = item;
}

// 将结点 p 插入到双向链表中当前结点的右边
template <class T>void DNode<T>::InsertRight(DNode<T>* p)
{
	// 将 p 和当前结点的右后继结点相连
	p->right = right;
	right->left = p;

	// 将 p 的左边和当前结点相连
	p->left = this;
	right = p;
}

// 将结点 p 插入到当前结点左边
template <class T>void DNode<T>::InsertLeft(DNode<T>* p)
{
	// 将 p 和当前结点的左后继结点相连
	p->left = left;
	left->right = p;

	// 将 p 的右边和当前结点相连
	p->right = this;
	left = p;
}

// 从链表中删除当前结点并返回其地址
template <class T>DNode<T>* DNode<T>::DeleteNode(void)
{
	// 左边的结点必须链接到当前结点的右边
	left->right = right;

	// 右边的结点必须链接到当前结点的左边
	right->left = left;

	// 返回当前结点的指针
	return this;
}

// 返回指向右边结点的指针
template <class T>DNode<T>* DNode<T>::NextNodeRight(void) const
{
	return right;
}

// 返回指向左边结点的指针
template <class T>DNode<T>* DNode<T>::NextNodeLeft(void) const
{
	return left;
}



//双向链表
template <class T>
class DCLinkedList
{
private:
	// 指向表头的指针和当前结点的指针
	DNode<T>* header, * currPtr;

	// 表中的元素个数和当前的位置值
	int size;

	// 将表 L 拷贝到当前表尾
	void CopyList(const DCLinkedList<T>& L);

public:
	// 构造函数
	DCLinkedList(void);
	DCLinkedList(const DCLinkedList<T>& L);

	// 析构函数
	~DCLinkedList(void);

	// 赋值运算符
	DCLinkedList<T>& operator= (const DCLinkedList<T>& L);

	// 检查表状态的函数
	int ListSize(void) const;  //返回结点数             
	bool ListEmpty(void) const;//链表是否为空

	// 遍历表的函数
	void Reset(bool bheader = true);	// 是从表头开始遍历，还是表尾（反向遍历）
	void Next(void);                    //
	void Prev(void);                    //
	bool EndOfList(void) const;         //是否到表尾

	// 插入函数
	void InsertFront(const T& item);    //表头插入
	void InsertRear(const T& item);     //表尾插入
	void InsertAt(const T& item);       //当前插入
	void InsertAfter(const T& item);    //其后插入

	// 删除函数
	void DeleteFront(void);      //删除表头
	void DeleteRear(void);       //删除表尾
	void DeleteAt(void);         //删除当前

	// 访问／修改数据
	T& Data(void);               //访问当前结点数据
	bool Find(const T& item);    //查找是否有item结点

	// 清空表的函数
	void ClearList(void);        //
};

// 构造函数
template <class T>DCLinkedList<T>::DCLinkedList(void) :size(0)
{
	// 创建“哨位”结点，注意 new 可能会抛出异常
	currPtr = header = new DNode<T>();
}

// 构造函数
template <class T>DCLinkedList<T>::DCLinkedList(const DCLinkedList<T>& L) :size(0)
{
	// 创建“哨位”结点，注意 new 可能会抛出异常
	currPtr = header = new DNode<T>();
	CopyList(L);
}

// 析构函数
template <class T>DCLinkedList<T>::~DCLinkedList(void)
{
	ClearList();
	delete header;
}

// 将 L 拷贝到当前表尾
template <class T>void DCLinkedList<T>::CopyList(const DCLinkedList<T>& L)
{
	// 用指针 P 遍历表
	DNode<T>* p = L.header->NextNodeRight();

	// 往当前表的表尾插入 L 的每个元素
	while (p != L.header)
	{
		InsertRear(p->data);
		p = p->NextNodeRight();
	}
}

template <class T>int DCLinkedList<T>::ListSize(void) const
{
	// 不包括哨位结点
	return size;
}

template <class T>bool DCLinkedList<T>::ListEmpty(void) const
{
	return (size == 0);
}

template <class T>void DCLinkedList<T>::Reset(bool bheader)
{
	if (bheader)
		currPtr = header->NextNodeRight();	// 表头
	else
		currPtr = header->NextNodeLeft();	// 表尾
}

template <class T>void DCLinkedList<T>::Next(void)
{
	currPtr = currPtr->NextNodeRight();
}

template <class T>void DCLinkedList<T>::Prev(void)
{
	currPtr = currPtr->NextNodeLeft();
}

template <class T>bool DCLinkedList<T>::EndOfList(void) const
{
	return (currPtr == header);
}

// 插入函数
template <class T>void DCLinkedList<T>::InsertFront(const T& item)
{
	Reset();
	InsertAt(item);
}

template <class T>void DCLinkedList<T>::InsertRear(const T& item)
{
	currPtr = header;
	InsertAt(item);
}

template <class T>void DCLinkedList<T>::InsertAt(const T& item)
{
	DNode<T>* newNode = new DNode<T>(item);
	currPtr->InsertLeft(newNode);
	currPtr = newNode;
	size++;
}

template <class T>void DCLinkedList<T>::InsertAfter(const T& item)
{
	Next();
	InsertAt(item);
}

// 删除函数
template <class T>void DCLinkedList<T>::DeleteFront(void)
{
	Reset();
	DeleteAt();
}

template <class T>void DCLinkedList<T>::DeleteRear(void)
{
	Reset(false);
	DeleteAt();
}

template <class T>void DCLinkedList<T>::DeleteAt(void)
{
	// 若表为空或已到表尾，则返回
	if (currPtr == header)
		return;

	DNode<T>* p = currPtr->NextNodeRight();
	delete (currPtr->DeleteNode());
	currPtr = p;
	size--;
}

// 访问／修改数据
template <class T>T& DCLinkedList<T>::Data(void)
{
	// 若表为空或已到表尾，则出错
	if (currPtr == header)
		throw "DCLinkedList::Data: invalid reference";

	return currPtr->data;
}

template <class T>bool DCLinkedList<T>::Find(const T& item)
{
	for (Reset(); !EndOfList(); Next())
		if (Data() == item)
			return true;

	return false;
}

template <class T>DCLinkedList<T>& DCLinkedList<T>::operator= (const DCLinkedList<T>& L)
{
	if (this == &L)      // 无法赋值给自身
		return *this;

	ClearList();
	CopyList(L);
	return *this;
}

template <class T>void DCLinkedList<T>::ClearList(void)
{
	Reset();
	while (currPtr != header)
		DeleteAt();
}
