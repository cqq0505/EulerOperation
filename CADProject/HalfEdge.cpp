//author : connie cai

#include<iostream>
#include "HalfEdge.h"

using namespace std;

//Solid
Solid::Solid()
{
	static int num = 0;
	num_solid = num++;
	prevs = nexts = NULL;
	sface = NULL;
	sedge = NULL;
	svertex = NULL;

}


bool Solid::addVertex(Vertex *newVertex)
{
	if (!newVertex)
	{
		return false;
	}
	Vertex *tmpv = svertex;
	if (!tmpv)
	{
		svertex = newVertex;

	}
	else
	{
		while (tmpv->nextv)
		{
			tmpv = tmpv->nextv;
		}
		tmpv->nextv= newVertex;
		newVertex->prevv = tmpv;
	}
	return true;
}

bool Solid::addEdge(Edge *newEdge)
{
	if (!newEdge)
	{
		return false;
	}
	Edge *tmpe = sedge;
	if (!tmpe)
	{
		sedge = newEdge;

	}
	else
	{
		while (tmpe->nexte)
		{
			tmpe = tmpe->nexte;
		}
		tmpe->nexte = newEdge;
		newEdge->preve = tmpe;
	}
	return true;
}

bool Solid::addFace(Face *newFace)
{
	if (!newFace)
	{
		return false;
	}
	Face *tmpf = sface;
	if (!tmpf)
	{
		sface = newFace;

	}
	else
	{
		while (tmpf->nextf)
		{
			tmpf = tmpf->nextf;
		}
		tmpf->nextf = newFace;
		newFace->prevf = tmpf;
	}
	newFace->fsolid = this; //
	return true;

}

Vertex* Solid::findVertex(Point *p)
{
	Vertex *tmpv = svertex;
	for (;tmpv;tmpv = tmpv->nextv)
	{
		if (tmpv->point.x == p->x && tmpv->point.y == p->y && tmpv->point.z == p->z)
		{
			return tmpv;
		}

	}
	return NULL;
}

//Face 
Face::Face()
{
	static int num = 0;
	num_face = num++;
	prevf = nextf = NULL;
	fsolid = NULL;
	floop = NULL;

}

bool Face::addLoop(Loop *newLoop)
{
	if (!newLoop)
	{
		return false;
	}
	Loop *tmpl = floop;
	if (!tmpl)
	{
		floop = newLoop;
	}
	else
	{
		while (tmpl->nextl)
		{
			tmpl = tmpl->nextl;
		}
		tmpl->nextl = newLoop;
		newLoop->prevl = tmpl;
	}
	newLoop->lface = this; //
	return true;

}

//loop
Loop::Loop()
{
	static int num = 0;
	num_loop = num++;
	prevl = nextl = NULL;
	lface = NULL;
	lhedg = NULL;

}

bool Loop::addHalfEdge(HalfEdge *he)
{
	if (!he)
	{
		return false;

	}
	HalfEdge *tmp = lhedg;
	if (!lhedg)
	{
		lhedg = he;
	}
	else
	{
		while (tmp->next)
		{
			tmp = tmp->next;
		}
		tmp->next = he;
		he->prev = tmp;
	}
	he->hloop = this;
	return true;
}

HalfEdge* Loop::findHe(Vertex *v)
{
	if (!v)
		return NULL;
	HalfEdge *tmp = lhedg;
	while (tmp)
	{
		if (tmp->startv == v)
			return tmp;
		tmp = tmp->next;
		if (tmp == lhedg)
			return NULL;
	}
	return NULL;
}

HalfEdge::HalfEdge()
{

	hloop = NULL;
	prev = next = adjacent =NULL;
	edge = NULL;
	startv = endv = NULL;

}

Edge::Edge()
{
	preve = nexte = NULL;
	he1 = he2 = NULL;
}

Point::Point()
{
	x, y, z = 0.0;
}
Point::Point(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}
Point& Point::operator=(const Point&  p)
{
	this->x = p.x;
	this->y = p.y;
	this->z = p.z;
	return *this;
}


Vertex::Vertex()
{
	static int num = 0;
	num_vertex = num++;
	prevv = nextv = NULL;
	point.x = point.y = point.z = 0.0;
}
