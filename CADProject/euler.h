#pragma once
#include "HalfEdge.h"


//----------------Euler Operation --------------------------
	//����һ��ʵ�壬һ���棬һ���㣬һ���⻷
	Solid* mvfs(Point *p);

	//����һ���µ㣬ͬʱ����һ�������µ��������ı�
	HalfEdge* mev(Point *oldPoint, Point *newPoint, Loop *lp);

	//����������乹��һ���±ߣ�ͬʱ����һ�����棬�Լ�һ���⻷
	Loop* mef(Point *p1, Point *p2, Loop *lp);

	//��ȥ���е�һ���ߣ�����һ���ڻ�
	Loop* kemr(Point *p1, Point *p2, Loop *lp);

	//ɾ��һ���棬���䶨��Ϊ��һ������ڻ��������й���һ����/��������ϲ�
	void kfmrh(Loop *OuterLp ,Loop *InnerLp);

    //Sweeping Operation
	void sweeping (Face *face ,Point *sweepPoint);

	Solid* getSolid();//���ļ����������Ϣ������ʵ��


	Solid * getSolid1();//����ŷ��������ɨ�ɲ����õ�һ��������ͨ�׵�ʵ��

	void saveVertexToCoord(Solid *solid, double verCoord[1000][3]);
	void output(Solid *solid);//���

	int output2(Solid *solid);