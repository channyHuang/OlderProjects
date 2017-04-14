#pragma once


#include <vector>
using namespace std;


template<class T>
class MatrixGao
{
public:
	vector<vector<T>> content;
	int sizeR,sizeC;

public:
	MatrixGao<T>(void)
	{
		sizeR = sizeC = 0 ; 

	}
	MatrixGao<T>(int n1, int n2)//the first parameter is the number of row , the other one is the number of column
	{
		int i,j; 
		vector<T> v;
		for( i = 0 ; i < n1 ; ++i)
		{
			content.push_back(v);

			for( j = 0 ; j < n2 ; ++j)
			{
				this->content[i].push_back(0);
			}
		}
		sizeR = n1;
		sizeC = n2 ; 

	}

	/*MatrixGao<T>(MatrixGao<float> a)
	{
	int i,j;
	vector<T> v;
	for(i = 0 ; i < a.sizeR ; ++i)
	{
	content.push_back(v);
	for (j=0 ; j<a.sizeC ; ++j)
	{
	this->content[i].push_back(a[i][j]);
	}
	}

	sizeR = a.sizeR;
	sizeC = a.sizeC;


	}*/
	MatrixGao<T>(int n1, int n2 , T *a)
	{
		int i,j;
		vector<T> v;
		for( i = 0 ; i < sizeR ; ++i)
		{

			this->content[i].clear();

		}
		this->content.clear();


		sizeR=sizeC=0;


		for(i = 0 ; i < n1 ; ++i)
		{
			content.push_back(v);
			for (j=0 ; j<n2 ; ++j)
			{
				this->content[i].push_back(a[i][j]);
			}
		}

		sizeR = n1;
		sizeC = n2;



	}
	vector<T> & operator [] (int i)
	{
		if(i>=sizeR);
		else
		{
			return content[i];
		}

	}
	MatrixGao<T> & operator = (const MatrixGao<T> & a)
	{
		int i,j;
		for( i = 0 ; i < sizeR ; ++i)
		{
			this->content[i].clear();		
		}
		this->content.clear();


		sizeR = sizeC = 0;


		vector<T> v;
		for( i = 0 ; i < a.sizeR ; ++i )
		{
			content.push_back(v);
			for( j = 0 ; j < a.sizeC ; ++j)
			{
				(this->content[i]).push_back(a.content[i][j]);
			}
		}

		this->sizeR=a.sizeR;
		this->sizeC=a.sizeC;

		return *this;



	}
	MatrixGao<T> & operator = (const T* a)//需要等大小的矩阵数组复制
	{
		int i,j;
		for( i = 0 ; i < sizeR ; ++i )
		{
			for( j = 0 ; j < sizeC ; ++j)
			{
				this->content[i][j]=a[i*sizeR+j];
			}
		}
		return *this;


	}
	MatrixGao<T> & operator = ( T a[])//需要等大小的矩阵数组复制
	{
		int i,j;
		for( i = 0 ; i < sizeR ; ++i )
		{
			for( j = 0 ; j < sizeC ; ++j)
			{
				this->content[i][j]=a[i*sizeR+j];
			}
		}
		return *this;


	}
	bool Transpose ()
	{
		MatrixGao temp(sizeR,sizeC);
		int i,j;
		temp=*this;
		for( i = 0 ; i < sizeR ; ++i )
		{
			this->content[i].clear();
		}
		this->content.clear();

		for( i = 0 ; i < sizeC ; ++i )
		{
			this->content.push_back(vector<T>(sizeR,0));
		}


		for( i = 0 ; i < sizeR ; ++i )
		{
			for( j = 0 ; j < sizeC ; ++j)
			{
				this->content[j][i]=temp[i][j];
			}
		}

		sizeC=temp.sizeR;
		sizeR=temp.sizeC;
		return true;

	}
	void clear()
	{
		int i,j;
		for( i = 0 ; i < sizeR ; ++i)
		{
			this->content[i].clear();	
		}
		this->content.clear();


		sizeR=sizeC=0;

	}
public:
	~MatrixGao(void)
	{
		int i,j;
		for( i = 0 ; i < sizeR ; ++i)
		{		
			this->content[i].clear();		
		}
		this->content.clear();


		sizeR = sizeC = 0;

	}
};
