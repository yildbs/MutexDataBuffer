#include "DataBuffer.h"

IData::IData(){}
IData::~IData(){}
void IData::clear(){}

DataBuffer::DataBuffer() 
	: dropped(0)
	, maxSize(30)
{ }

DataBuffer::~DataBuffer()
{ 
	//TODO
	//check what variable is needed to delete
}

void DataBuffer::setMaxSize(int size)
{
	this->maxSize = size;
}

void DataBuffer::clear()
{
	while(this->buffer.size()>0){
		IData* data = (IData*)buffer.front();
		data->clear();
		delete data;
		buffer.pop();
	}
}

bool DataBuffer::push(void* newData)
{
	bool ret = true;
	std::lock_guard<std::mutex> guard(this->mtxQueueIOLock);
	if( maxSize < buffer.size() ){
		IData* data = (IData*)buffer.front();
		data->clear();
		delete data;
		buffer.pop();
		this->dropped++;
		ret = false;
	}
	buffer.push(newData);
	cvNewData.notify_all();
	return ret;
}

bool DataBuffer::pop(void*& data)
{
	std::lock_guard<std::mutex> guard(this->mtxQueueIOLock);
	int size = buffer.size();
	if( size > 0 ){
		data = buffer.front();
		buffer.pop();
		{
			std::unique_lock<std::mutex> lock(mtxDataPopped);
			cvDataPopped.notify_all();
		}
		return true;
	}
	return false;
}

int DataBuffer::getDropped()
{
	return this->dropped;
}
void DataBuffer::clearDropped()
{
	this->dropped = 0;
}

void DataBuffer::waitUntilNew()
{
	std::unique_lock<std::mutex> lock(mtxNewData);
	cvNewData.wait(lock,[this]{return this->buffer.size()!=0;});
}

void DataBuffer::waitUntilPop()
{
	std::unique_lock<std::mutex> lock(mtxDataPopped);
	bool full = isFull();
	cvDataPopped.wait(lock);
}

bool DataBuffer::isFull()
{
	bool ret = false;
	std::lock_guard<std::mutex> guard(this->mtxQueueIOLock);
	if( maxSize < buffer.size() ){
		ret = true;
	}
	return ret;
}
