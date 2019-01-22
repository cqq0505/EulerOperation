#pragma once
#include "HalfEdge.h"


//----------------Euler Operation --------------------------
	//构造一个实体，一个面，一个点，一个外环
	Solid* mvfs(Point *p);

	//构造一个新点，同时构造一条连接新点与给定点的边
	HalfEdge* mev(Point *oldPoint, Point *newPoint, Loop *lp);

	//在两给定点间构造一个新边，同时构造一个新面，以及一个外环
	Loop* mef(Point *p1, Point *p2, Loop *lp);

	//消去环中的一条边，构造一个内环
	Loop* kemr(Point *p1, Point *p2, Loop *lp);

	//删除一个面，将其定义为另一个面的内环，在体中构造一个柄/或将两物体合并
	void kfmrh(Loop *OuterLp ,Loop *InnerLp);

    //Sweeping Operation
	void sweeping (Face *face ,Point *sweepPoint);

	Solid* getSolid();//从文件读入基本信息，生成实体


	Solid * getSolid1();//基于欧拉操作和扫成操作得到一个带两个通孔的实体

	void saveVertexToCoord(Solid *solid, double verCoord[1000][3]);
	void output(Solid *solid);//输出

	int output2(Solid *solid);