#ifndef INCLUDE_NODE
#define INCLUDE_NODE

#pragma once

template <class T>
class Node
{
public:
	Node(T value)
	{
		data = value;
	};
	~Node()
	{
		delete next;
	};
	T getData()
	{
		return data;
	};
	void setData(T d)
	{
		this->data = d;
	};
	Node<T>* getNext()
	{
		return next;
	};
	void setNext(Node<T>* n)
	{
		this->next = n;
	};

private:
	Node<T>* next;
	T data;
};

#endif