#include "model.h"

Model::Model()
{

}

bool Model::load(QString filename)
{
	GLMmodel::GLMgroup *glm;
	GLMmodel *glmodel=new GLMmodel(filename.toLocal8Bit().data());
	map<int,Midedge*> *adjedge=new map<int,Midedge*>[glmodel->numvertices];
	map<int,Midedge*>::iterator it;
	Triangle *mytri;
	Vertex tmpver;
	int verno[3];
	float maxx,maxy,maxz,minx,miny,minz;
	edgenum=0;
	for(int i=0;i<tri.size();i++) 	delete []tri[i].midedge;
	ver.clear();
	tri.clear();
	ver.resize(glmodel->numvertices);
	maxx=minx=glmodel->vertices[3];
	maxy=miny=glmodel->vertices[4];
	maxz=minz=glmodel->vertices[5];
	for(int i=0;i<glmodel->numvertices;i++) {
		copyvector(ver[i].axis,&glmodel->vertices[i*3+3]);
		copyvector(tmpver.axis,&glmodel->vertices[i*3+3]);
		if(maxx<tmpver.axis[0]) maxx=tmpver.axis[0];
		else if(minx>tmpver.axis[0]) minx=tmpver.axis[0];
		if(maxy<tmpver.axis[1]) maxy=tmpver.axis[1];
		else if(miny>tmpver.axis[1]) miny=tmpver.axis[1];
		if(maxz<tmpver.axis[2]) maxz=tmpver.axis[2];
		else if(minz>tmpver.axis[2]) minz=tmpver.axis[2];
	}
	tx=(maxx+minx)/2.0;
	ty=(maxy+miny)/2.0;
	tz=(maxz+minz)/2.0;
	tri.resize(glmodel->numtriangles);
	for(int i=0;i<glmodel->numtriangles;i++) {
		mytri=&tri[i];
		mytri->newedge(3);
		copyvector(mytri->norm,&glmodel->facetnorms[glmodel->triangles[i].findex*3]);
		copyvectori(verno,glmodel->triangles[i].vindices);
		verno[0]-=1;
		verno[1]-=1;
		verno[2]-=1;
		for(int j=0;j<3;j++) {
			mytri->midedge[j].tail=verno[j];
			mytri->midedge[j].no=i;
			if(ver[verno[(j+1)%3]].midedge==NULL) ver[verno[(j+1)%3]].midedge=&mytri->midedge[j];
		}
		for(int j=0;j<3;j++) {
			adjedge[verno[(j+1)%3]][verno[j]]=&mytri->midedge[j];
			it=adjedge[verno[j]].find(verno[(j+1)%3]);
			if(it!=adjedge[verno[j]].end()) {
				mytri->midedge[j].midedge=it->second;
				it->second->midedge=&mytri->midedge[j];
			}
			else {
				edgenum++;
			}
		}
	}
	for(int i=0;i<glmodel->numvertices;i++) ver[i].num=adjedge[i].size();
	for(int i=0;i<glmodel->numvertices;i++)
		for(it=adjedge[i].begin();it!=adjedge[i].end();it++) {
			if(it->second->midedge==NULL) ver[it->second->tail].num++;
		}
	delete []adjedge;
	return true;
}