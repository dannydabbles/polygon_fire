//$Id: 3Dmodel.h,v 1.22 2009-12-09 06:13:30-08 - - $
//3Dmodel.h
//Written by Danny Key
#ifndef __3DMODEL_H__
#define __3DMODEL_H__

#include <vector>
#include <cstdlib>
#include <math.h>
#include <iostream>
#include <string>

//#define _CRTDBG_MAP_ALLOC

#include <stdio.h>

//#include <crtdbg.h>

#include <cassert>
#include<iostream>
#include<fstream>
#include <time.h>
#include <GL/glut.h>         /* glut.h includes gl.h and glu.h*/


using namespace std;

typedef struct Shape* ShapeRef;

typedef struct Poly* PolyRef;

typedef struct Parti* PartiRef;

typedef struct Flame* FlameRef;

typedef struct Cam* CamRef;

double *normalize(double a, double b, double c);
double *normalvec(double *P1, double *P2, double *P3);
double timer(double nowtime, double starttime);
double *camPlane(double x, double y, double z);
double *pFN(double x,double y,double z);
double *crossProd(double *, double *);
void loadpolylist(ShapeRef shaper, PolyRef poly);
void startPicking();
void stopPicking();
void findstart(PolyRef);
void spread(PolyRef);
void fspread(PolyRef);

struct Shape{

  int thisnum;

  int numvec;
  int numpoly;

  //booleans for what is being done
  bool   istranslating[3];
  bool   islrotating[3];
  bool   isscaling[1];   

  //values for transformations

  double starttranslation[3];
  double translation[3];

  double theta;

  double startlrotation[3];
  double lrotation[3];

  double scalar;
  double startscalar;

  double delX;
  double delY;

  double prevX;
  double prevY;

  vector< PolyRef > polygon;
  vector< vector<double>* > vertex;
  vector<bool> firevert;

  Shape();
  ~Shape();

  void draw();
  void update();

  void loadframe(char *, char*);

  void transform();

  bool fspread(PolyRef poly);


  void reset(PolyRef poly);
};

struct Poly{

  ShapeRef myShape;

  FlameRef myFlame;

  //double normal;

  int myNumber;

  vector<int> verticies;
  vector<PolyRef> subpoly;
  int center;
  double *aveVec;
  void findCenter();
  void makeTri();

  Poly(ShapeRef myShape);  ~Poly();

  void draw();
  void drawFlame();
  void update();
  void subdivide();

  bool spread();
};

struct Parti{

  bool on;
  double *startcoor;
  double *coor;
  double *lastcoor;
  double starttime;
  double endtime;
  double *velocity;
  double startvelY;
  double tran;
  double *color;
  
  Parti(double *coor, double *velocity, double time,double colorx,double colory, double tran);

  ~Parti();
  
  void draw();
  void update(double, double);

};

struct Flame{

  PolyRef myPoly;

  bool on;
  PartiRef *particles;
  double aveTime;
  double rangeTime;
  double rangeNDir;
  double rangeADir;
  double rangeBDir;
  double numparti;
  double *origin;
  double *norm;
  double *aVec;
  double *bVec;

  Flame(PolyRef);
  Flame();
  ~Flame();
  void addParti();
  void update();
  void draw();
};



struct Cam{
  double *point;
  double *eye;
  double *movepoint;
  double *moveeye;
  int angle;
  double acangle;
  double length;


  Cam();
  ~Cam();
  Cam(double pointx,double pointy, double pointz, double eyex, double eyey, double eyez);

  void update() {
    point[0]+=movepoint[0];
    point[1]+=movepoint[1];
    point[2]+=movepoint[2];
    eye[0]+=moveeye[0];
    eye[1]+=moveeye[1];
    eye[2]+=moveeye[2];
    double p=3.14159265;
    
    //printf("%f  ,%f  ,%f",point[0], point[1],point[2]);
  };
  void lookAt(){
    gluLookAt(eye[0],eye[1],eye[2],point[0],point[1],point[2],0,1,0);
  };
  void moveeyes(){
    moveeye[0]=((point[0]-eye[0]));
    moveeye[1]=((point[1]-eye[1]));
    moveeye[2]=((point[2]-eye[2]));
  }
  void setmove(double *move){
    double sc=1;
    moveeye[0]=move[0]*sc;
    moveeye[1]=move[1]*sc;
    moveeye[2]=move[2]*sc;
    movepoint[0]=moveeye[0];
    movepoint[1]=moveeye[1];
    movepoint[2]=moveeye[2];
  }
  double *getmove(double dir){
    return normalize(moveeye[0]*dir,moveeye[1]*dir,moveeye[2]*dir);
  }
  void moveWS(bool forward){
    moveeyes();
    double *move;
    double dir=1;
    if(forward==false)
      dir=-1;
    
   move=getmove(dir);
   setmove(move);
  };
  void moveAD(bool left){
    double temp[3];
    temp[0]=eye[0];
    temp[1]=eye[1]-10;
    temp[2]=eye[2];
    double *norm=(double *)normalvec(point,eye,temp);
    //printf("%f  ,%f  ,%f",norm[0], norm[1],norm[2]);
    double dir=-1;
    if(left==false)
      dir=1;
    norm=normalize(norm[0]*dir,norm[1]*dir,norm[2]*dir);
    norm[1]=0;
    setmove(norm);
    
  };
  void setYaw(bool left){

      
    int dir=1;
    if(left==true)
      dir=-1;
    angle = 1*dir;
  };
  void yaw(){
    acangle+=(.5*angle);
    double p=3.14159265;
    point[0]=eye[0]-(sqrt(length*length*2)*sin(acangle*p/180));
    point[2]=eye[2]-(sqrt(length*length*2)*cos(acangle*p/180));
  };
  void pitch(bool up){
  };

  void moveud(bool up){
    int dir=1;
    if(up==false)
      dir=-1;
    moveeye[1]=dir;
    setmove(moveeye);
  }
  void reset(){
    movepoint[0]=movepoint[1]=movepoint[2]=moveeye[0]=moveeye[1]=moveeye[2]=0;
    angle=0;
  };
  void comreset(){
    eye[0]=400;
    eye[1]=0;
    eye[2]=400;
    point[0]=0;
    point[1]=0;
    point[2]=0;
    angle=0;
    acangle=0;
  };
};



#endif
