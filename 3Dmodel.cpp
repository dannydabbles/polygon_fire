//$Id: 3Dmodel.cpp,v 1.30 2009-12-09 06:13:53-08 - - $
//3Dmodel.cc
//Written by Danny Key
//dkey@unix.ic.ucsc.edu:~/private/CMPS160/FINAL/transfer/.
//yqf748163
//smedalen@unix.ic.ucsc.edu:~/private/CMPS160/final/transfer/.
//Ab199872
#include "3Dmodel.h"

#define BUFSIZE 50000
GLuint selectBuf[BUFSIZE];

//#define _HAS_ITERATOR_DEBUGGING 0
vector<PolyRef> PolyList;
int runthrough;

int spreadslow;
int spreadvar;
int each;

vector<ShapeRef> shapes;
int snum;

double currX;
double currY;

double mouseX;
double mouseY;

double nowtime;
double timevar;

int rotchar;

bool firstdown;
bool mousedown;

bool arepicking;
bool started;

bool axison;

bool useperspective;

int viewW;
int viewH;

double firegrav;

double starter;

FlameRef truce;

CamRef theCam;

bool fire;
bool cfire;
bool showpoly;
bool tranobj;
bool flaming;
bool pauses;



void setfire(PolyRef poly){
  //return;
  for(vector<int>::iterator iter=poly->verticies.begin();
      iter!=poly->verticies.end();iter++){
    poly->myShape->firevert[(*iter)]=true;
  }
  if(poly->myFlame == NULL)
    poly->myFlame = new Flame(poly);
}


double *pFN(double x,double y,double z){
  double *norm = new double[3];
  double *tvec = new double[3];
  norm[0] = x;
  norm[1] = y;
  norm[2] = z;

  tvec[0] = y / 7.0;
  tvec[1] = z / 3.0;
  tvec[2] = x / 5.0;

  if(norm[0] == tvec[0] &&
     norm[1] == tvec[1] &&
     norm[2] == tvec[2]){
    tvec[1] = (norm[0] + norm[1] + norm[2] + 1) / 3.0;
  }
  double *perp = crossProd(norm, tvec);
  double *temp = normalize(perp[0], perp[1], perp[2]);
  delete perp;
  return temp;
}
/*
  double *norm = new double[3];
  if(x == 0){
  norm[0] = 1;
  norm[1] = 0;
  norm[2] = 0;
  }
  else if(y == 0){
  norm[0] = 0;
  norm[1] = 1;
  norm[2] = 0;
  }
  else if(z == 0){
  norm[0] = 0;
  norm[1] = 0;
  norm[2] = 1;
  }
  else{
  norm[0] = -((y+z)/x);
  norm[1] = 1;
  norm[2] = 1;
  }
  norm[0] = 1;
  norm[1] = 1;
  norm[2] = 0;
  double *temp = normalize(norm[0],norm[1],norm[2]);
  delete norm;
  return temp;
  };
*/

double *normalize(double a, double b, double c){
  double length=sqrt(a*a+b*b+c*c);
  double *norm = new double[3];
  norm[0] = a/length;
  norm[1] = b/length;
  norm[2] = c/length;
  //printf("normalized end: X:|%6f| Y:|%6f| Z:|%6f|\n",norm[0],norm[1],norm[2]);
  return norm;
}

double *crossProd(double *A, double *B){
  double *norm= new double[3];
  norm[0] = A[1]*B[2]-A[2]*B[1];
  norm[1] = A[2]*B[0]-A[0]*B[2];
  norm[2] = A[0]*B[1]-A[1]*B[0];
  //printf("normalvec end: X:|%6f| Y:|%6f| Z:|%6f|\n",norm[0],norm[1],norm[2]);
  double *temp = normalize(norm[0],norm[1],norm[2]);
  delete norm;
  return temp;
}

double *normalvec(double *P1, double *P2, double *P3){
  //printf("P1: X:|%6f| Y:|%6f| Z:|%6f|\n",P1[0],P1[1],P1[2]);
  //printf("P2: X:|%6f| Y:|%6f| Z:|%6f|\n",P2[0],P2[1],P2[2]);
  //printf("P3: X:|%6f| Y:|%6f| Z:|%6f|\n",P3[0],P3[1],P3[2]);
  double *A = new double[3];
  A[0] = P2[0]-P1[0];
  A[1] = P2[1]-P1[1];
  A[2] = P2[2]-P1[2];
  //printf("A: X:|%6f| Y:|%6f| Z:|%6f|\n",A[0],A[1],A[2]);
  double *B = new double[3];
  B[0] = P3[0]-P2[0];
  B[1] = P3[1]-P2[1];
  B[2] = P3[2]-P2[2];
  //printf("B: X:|%6f| Y:|%6f| Z:|%6f|\n",B[0],B[1],B[2]);
  double *norm= new double[3];
  norm[0] = A[1]*B[2]-A[2]*B[1];
  norm[1] = A[2]*B[0]-A[0]*B[2];
  norm[2] = A[0]*B[1]-A[1]*B[0];
  delete A;
  delete B;
  //printf("normalvec end: X:|%6f| Y:|%6f| Z:|%6f|\n",norm[0],norm[1],norm[2]);
  double *temp = normalize(norm[0],norm[1],norm[2]);
  delete norm;
  return temp;
}

double timer(double nowtime, double starttime){
  return (nowtime - starttime);
}

double *camPlane(double x, double y, double z){
  //printf("camPlane() Got here!!!!!!!!!!!!!!!!!\n");
  double scale = 5;
  double normx = theCam->eye[0]-x;
  double normy = theCam->eye[1]-y;
  double normz = theCam->eye[2]-z;
  //printf("theCam->eye: %f, %f, %f\n", theCam->eye[0], theCam->eye[1], theCam->eye[2]);
  //printf("xyz        : %f, %f, %f\n", x, y, z);
  //printf("normX      : %f, %f, %f\n", normx, normy, normz);
  double *norm = normalize(normx,normy,normz);
  double *avec = pFN(norm[0],norm[1],norm[2]);
  //printf("avec       : %f, %f, %f\n", avec[0], avec[1], avec[2]);
  double point0[3]={x,y,z};
  double point1[3]={x+norm[0],y+norm[1],z+norm[2]};
  double point2[3]={x+avec[0],y+avec[1],z+avec[3]};
  double *bvec=normalvec(point0,point1,point2);
  double *square = new double[12];
  square[0]  =x+avec[0]*scale;
  square[1]  =y+avec[1]*scale;
  square[2]  =z+avec[2]*scale;
  square[3]  =x+bvec[0]*scale;
  square[4]  =y+bvec[1]*scale;
  square[5]  =z+bvec[2]*scale;
  square[6]  =x-avec[0]*scale;
  square[7]  =y-avec[1]*scale;
  square[8]  =z-avec[2]*scale;
  square[9]  =x-bvec[0]*scale;
  square[10] =y-bvec[1]*scale;
  square[11] =z-bvec[2]*scale;
  delete norm;
  delete avec;
  delete bvec;
  return square;
}


GLuint textured; //the array for our texture

GLfloat angle = 0.0;



GLuint  LoadTexture( const char * filename, int width, int height )
{
    GLuint  texture;
    unsigned char * data;
    FILE * file;

    //The following code will read in our RAW file
    file = fopen( filename, "rb" );
    if ( file == NULL ) return 0;
    data = (unsigned char *)malloc( width * height * 3 );
    fread( data, width * height * 3, 1, file );
    fclose( file );

    glGenTextures( 1, &texture ); //generate the texture with the loaded data
    glBindTexture( GL_TEXTURE_2D, texture );
 //bind the texture to it's array
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); //set texture environment parameters

    //here we are setting what textures to use and when. The MIN filter is which quality to show
    //when the texture is near the view, and the MAG filter is which quality to show when the texture
    //is far from the view.

    //The qualities are (in order from worst to best)
    //GL_NEAREST
    //GL_LINEAR
    //GL_LINEAR_MIPMAP_NEAREST
    //GL_LINEAR_MIPMAP_LINEAR

    //And if you go and use extensions, you can use Anisotropicfiltering textures which are of an
    //even better quality, but this will do for now.
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );

    //Here we are setting the parameter to repeat the texture innstead of clamping the texture
    //to the edge of our shape.
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP );

    //Generate the texture with mipmaps
    //gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height, 
    //		       GL_RGBA, GL_UNSIGNED_BYTE, data );
    gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, data );
    //glTexImage2D(GL_TEXTURE_2D,0,3,width,height,0,GL_RGB,GL_UNSIGNED_BYTE, data);
    free( data ); //free the texture
    return texture; //return whether it was successfull
}

void FreeTexture( GLuint &texture )
{
  glDeleteTextures( 1, &texture );
}



Cam::Cam(){
  this->movepoint = new double[3];
  this->moveeye = new double[3];
  this->point = new double[3];
  this->point[0]=0;
  this->point[1]=0;
  this->point[2]=0;
  this->eye = new double[3];
  this->length=400;
  this->eye[0]=length;
  this->eye[1]=0;
  this->eye[2]=length;
  movepoint[0]=0;
  movepoint[1]=0;
  movepoint[2]=0;
  moveeye[0]=0;
  moveeye[1]=0;
  moveeye[2]=0;
  angle=0;
  acangle=0;
}
/*
Cam::Cam(double pointx,double pointy, double pointz, double eyex, double eyey, double eyez){
  this->point[0]=pointx;
  this->point[1]=pointy;
  this->point[2]=pointz;
  this->eye[0]=eyex;
  this->eye[1]=eyey;
  this->eye[2]=eyez;
}
*/
Cam::~Cam(){
  delete point;
  delete eye;
  delete movepoint;
  delete moveeye;
}


/*
  double timer(double nowtime, double starttime){
  return (nowtime - starttime);
  }

  double *normalize(double a, double b, double c){
  double length=sqrt(a*a+b*b+c*c);
  double *norm = new double[3];
  norm[0] = a/length;
  norm[1] = b/length;
  norm[2] = c/length;
  //printf("normalized end: X:|%6f| Y:|%6f| Z:|%6f|\n",norm[0],norm[1],norm[2]);
  return norm;
  }
  double *normalvec(double *P1, double *P2, double *P3){
  //printf("P1: X:|%6f| Y:|%6f| Z:|%6f|\n",P1[0],P1[1],P1[2]);
  //printf("P2: X:|%6f| Y:|%6f| Z:|%6f|\n",P2[0],P2[1],P2[2]);
  //printf("P3: X:|%6f| Y:|%6f| Z:|%6f|\n",P3[0],P3[1],P3[2]);
  double *A = new double[3];
  A[0] = P2[0]-P1[0];
  A[1] = P2[1]-P1[1];
  A[2] = P2[2]-P1[2];
  //printf("A: X:|%6f| Y:|%6f| Z:|%6f|\n",A[0],A[1],A[2]);
  double *B = new double[3];
  B[0] = P3[0]-P2[0];
  B[1] = P3[1]-P2[1];
  B[2] = P3[2]-P2[2];
  //printf("B: X:|%6f| Y:|%6f| Z:|%6f|\n",B[0],B[1],B[2]);
  double *norm= new double[3];
  norm[0] = A[1]*B[2]-A[2]*B[1];
  norm[1] = A[2]*B[0]-A[0]*B[2];
  norm[2] = A[0]*B[1]-A[1]*B[0];
  //printf("normalvec end: X:|%6f| Y:|%6f| Z:|%6f|\n",norm[0],norm[1],norm[2]);
  return normalize(norm[0], norm[1], norm[2]);
  }
*/

Parti::Parti(double *coor, double *velocity, double time, double colorx, double colory, double tran){
  this->startcoor = new double[3];
  this->coor = new double[3];
  this->lastcoor = new double[3];
  this->velocity = new double[3];
  this->color = new double[3];
  
  for(int i = 0; i < 3 ;++i){
    this->lastcoor[i] = coor[i];
    this->coor[i] = coor[i];
    this->startcoor[i] = coor[i];
    this->velocity[i] = velocity[i];
  }

  this->color[0]=colorx;
  this->color[1]=colory;
  this->tran=tran;
  this->startvelY = velocity[1];
  this->endtime = time + nowtime;
  this->starttime = nowtime;
  this->on = true;
}

Parti::~Parti() {
  delete startcoor;
  delete coor;
  delete lastcoor;
  delete velocity;
}

/*
  void Parti::update(){
  if(nowtime>endtime){
  this->on=false;
  return;
  }

  for(int i = 0; i<3 ;++i){
  lastcoor[i] = coor[i];
  }
  coor[0] = startcoor[0] + velocity[0] * timer(nowtime, starttime);
  velocity[1] = startvelY + firegrav * timer(nowtime, starttime);
  coor[1] = startcoor[1] + startvelY * timer(nowtime, starttime) + 
  .5 * firegrav * timer(nowtime, starttime) * timer(nowtime, starttime);
  coor[2] = startcoor[2] + velocity[2] * timer(nowtime, starttime);
  }
  void Parti::draw(){
  //cout<<"Got here!"<<endl;


  glBegin(GL_LINES);
  glColor3d( ((double)(rand()%1000))/2000.0 + .5, ((double)(rand()%1000))/2000.0, 0.0);
  glVertex3d(lastcoor[0], lastcoor[1], lastcoor[2]);
  glVertex3d(coor[0],coor[1],coor[2]);
  //printf("X:|%6f| Y:|%6f| Z:|%6f|\n",coor[0],coor[1],coor[2]);
  glEnd();
  }
*/

void Parti::update(double nowtime, double firegrav){

  //return;
  if(nowtime>endtime){
    this->on=false;
    return;
  }

  for(int i = 0; i<3 ;++i){
    lastcoor[i] = coor[i];
  }

  coor[0] = startcoor[0] + velocity[0] * timer(nowtime, starttime);
  velocity[1] = startvelY + firegrav * timer(nowtime, starttime);
  coor[1] = startcoor[1] + startvelY * timer(nowtime, starttime) + 
    .5 * firegrav * timer(nowtime, starttime) * timer(nowtime, starttime);
  coor[2] = startcoor[2] + velocity[2] * timer(nowtime, starttime);
  tran-=(.01*timevar);
  /*
    printf("coor[0]:%f|  coor[1]:%f|  coor[2]:%f|\n",
    coor[0],
    coor[1],
    coor[2]);

    printf("startcoor[0]:%f|  startcoor[1]:%f|  startcoor[2]:%f|\n",
    startcoor[0],
    startcoor[1],
    startcoor[2]);

    printf("Change in X:%f\nChange in Y:%f\nChange in z:%f\n",
    coor[0] - startcoor[0],
    coor[1] - startcoor[1],
    coor[2] - startcoor[2]);

    printf("velocity[0]:%f|  velocity[1]:%f|  velocity[2]:%f|\n",
    velocity[0],
    velocity[1],
    velocity[2]);
  */
}
void Parti::draw(){
  //cout<<"Got here!"<<endl;
  //return;    
  if(this == NULL) return;
  assert(this != NULL);

  //double *q=camPlane(coor[0], coor[1], coor[2]);


  
//glBegin(GL_QUADS);
  glBegin(GL_POINTS);

  //return;

  //glColor4d(color[0],color[1],.0,tran);
  glColor4d( ((double)(rand()%1000))/2000.0 + .5, ((double)(rand()%1000))/2000.0, 0.0, tran);
  glVertex3d(coor[0],coor[1],coor[2]);
	  //glVertex3d(lastcoor[0],lastcoor[1],lastcoor[2]);
  //glVertex3d(q[0],q[1],q[2]);
  //glVertex3d(q[3],q[4],q[5]);
  //glVertex3d(q[6],q[7],q[8]);
  //glVertex3d(q[9],q[10],q[11]);
  //glTexCoord2d(1,1);
  //glVertex3d(q[0],q[1],q[2]);
  // glTexCoord2d(1,0); 
  //glVertex3d(q[3],q[4],q[5]);
  //glTexCoord2d(0,0);
  // glVertex3d(q[6],q[7],q[8]);
  // glTexCoord2d(0,1);
  // glVertex3d(q[9],q[10],q[11]);

  glEnd();

  //delete q;
  //q = NULL;

  //printf("Parti::draw() Got here finished!!!!!!!!!!!!!!!!!\n");

  /*
    glBegin(GL_LINES);
    //glColor4d( ((double)(rand()%1000))/2000.0 + .5, ((double)(rand()%1000))/2000.0, 0.0, 0.5);
    glColor4d(color[0],color[1],.0,tran);
    glVertex3d(lastcoor[0], lastcoor[1], lastcoor[2]);
    glVertex3d(coor[0],coor[1],coor[2]);

    glEnd();
  */
  //printf("X:|%6f| Y:|%6f| Z:|%6f|\n",coor[0],coor[1],coor[2]);
}









void Poly::subdivide(){
  //return;
  double maxlen   = 20.0;
  double maxperim = 8 * maxlen;
  double minperim = 7 * maxlen;
  if(this == NULL) return;
  if(verticies.size() < 3) return;
  if(subpoly.size() == 0){
    if(verticies.size() > 3){
      cout<<"Subdivision error:  Trying to subdivide non-triangle"<<endl;
      exit(1);
    }
    //return;
  }
  if(subpoly.size() != 0){
    for(vector<PolyRef>::iterator itor = subpoly.begin();
	itor != subpoly.end(); ++itor){
      (*itor)->subdivide();
    }
    return;
  }
  //printf("Huh????????????????????!!!!!!!!!!!!!!\n");

  //printf("verticies size =%d\n", verticies.size());
  //exit(1);
  //assert(verticies.size() == 3);
  //printf("verticies size =%d\n", verticies.size());
  /*
  double **arm = new double*[3];
  for(int i = 0; i < 3 ;++i){
    arm[i] = new double[3];
  }
  for(int n = 0; n < 3 ;++n){
    int m;
    if(n == 2) m = 0;
    else m = n + 1;
    for(int i = 0; i < 3 ;++i){ 
      arm[n][i] = myShape->vertex[verticies[m]]->at(i) 
	- myShape->vertex[verticies[n]]->at(i);
    }
  }


  double bigleng = 0.0;
  int largest = 0;
  for(int i = 0; i < 3 ;++i){
    double length=sqrt(arm[i][0]*arm[i][0]+arm[i][1]*arm[i][1]+arm[i][2]*arm[i][2]);
    if(length > bigleng){
      bigleng = length;
      largest = i;
    }
  }

  ++jter;
  for(int i = 0; i < largest ;++i){
    ++iter;
    ++jter;
    if(jter == verticies.end()) jter = verticies.begin();
  }
  */

  //printf("*iter=%d||*jter=%d\n",*iter,*jter);
  //printf("vertex size=%d\n",myShape->vertex.size());

  double **arm = new double*[3];
  for(int i = 0; i < 3 ;++i){
    arm[i] = new double[3];
  }
  for(int n = 0; n < 3 ;++n){
    int m;
    if(n == 2) m = 0;
    else m = n + 1;
    for(int i = 0; i < 3 ;++i){ 
      arm[n][i] = myShape->vertex[verticies[m]]->at(i) 
	- myShape->vertex[verticies[n]]->at(i);
    }
  }


  double tlength = 0.0;
  for(int i = 0; i < 3 ;++i){
    double length=sqrt(arm[i][0]*arm[i][0]+arm[i][1]*arm[i][1]+arm[i][2]*arm[i][2]);
    tlength += length;
  }  

  vector<int>::iterator iter = verticies.begin();
  vector<int>::iterator jter = verticies.begin();

  for(; iter != verticies.end();++iter){
    ++jter;
    if(jter == verticies.end()) jter = verticies.begin();
    if(iter == verticies.end()) iter = verticies.begin();
    double *side = new double[3];
    for(int i = 0; i < 3 ;++i){
      //printf("verticies size =%d\n", verticies.size());
      //printf("jtor =%d\n", *jter);
      //printf("itor =%d\n", *iter);
      side[i] = myShape->vertex[(*jter)]->at(i) 
	- myShape->vertex[(*iter)]->at(i);
    }
    double length=sqrt(side[0]*side[0]+side[1]*side[1]+side[2]*side[2]);
    //printf("Before: length =%f\n", length);

    if(length <= maxlen){
      continue;
    }else{
      /*
      if(tlength < maxperim && tlength > minperim){
	this->makeTri();
	this->subdivide();
	return;
      }
      */
      //printf("After:  length =%f\n", length);
      double *H = new double[3];
      double *point = new double[3];
      for(int i = 0; i < 3 ;++i){
	H[i] = side[i] / 2.0;
	point[i] = myShape->vertex[(*iter)]->at(i) + H[i];
      }
      
      int place = myShape->vertex.size();
      myShape->vertex.push_back(new vector<double>());
      myShape->firevert.push_back(false);
      ++myShape->numvec;
      for(int i = 0; i < 3 ;++i){
	myShape->vertex[place]->push_back(point[i]);
      }
      PolyRef tri1 = new Poly(myShape);
      tri1->verticies.push_back(*iter);
      /*
      printf("tri1: *iter = %d\n", *iter);
      printf("X: %f\n",myShape->vertex[*iter]->at(0));
      printf("Y: %f\n",myShape->vertex[*iter]->at(1));
      printf("Z: %f\n",myShape->vertex[*iter]->at(2));
      */
      ++iter;
      ++jter;
      if(jter == verticies.end()) jter = verticies.begin();
      if(iter == verticies.end()) iter = verticies.begin();
      tri1->verticies.push_back(place);
      /*
      printf("tri1: place = %d\n", place);
      printf("X: %f\n",myShape->vertex[place]->at(0));
      printf("Y: %f\n",myShape->vertex[place]->at(1));
      printf("Z: %f\n",myShape->vertex[place]->at(2));
      */
      tri1->verticies.push_back(*jter);
      /*
      printf("tri1: *jter = %d\n", *jter);
      printf("X: %f\n",myShape->vertex[*jter]->at(0));
      printf("Y: %f\n",myShape->vertex[*jter]->at(1));
      printf("Z: %f\n",myShape->vertex[*jter]->at(2));
      */
      //if(tri1->myFlame == NULL) tri1->myFlame = new Flame(tri1);
      subpoly.push_back(tri1);
      subpoly[0]->subdivide();
      PolyRef tri2 = new Poly(myShape);
      tri2->verticies.push_back(place);
      /*
      printf("tri2: place = %d\n", place);
      printf("X: %f\n",myShape->vertex[place]->at(0));
      printf("Y: %f\n",myShape->vertex[place]->at(1));
      printf("Z: %f\n",myShape->vertex[place]->at(2));
      */
      tri2->verticies.push_back(*iter);
      /*
      printf("tri2: *iter = %d\n", *iter);
      printf("X: %f\n",myShape->vertex[*iter]->at(0));
      printf("Y: %f\n",myShape->vertex[*iter]->at(1));
      printf("Z: %f\n",myShape->vertex[*iter]->at(2));
      */
      tri2->verticies.push_back(*jter);
      /*
      printf("tri2: *jter = %d\n", *jter);
      printf("X: %f\n",myShape->vertex[*jter]->at(0));
      printf("Y: %f\n",myShape->vertex[*jter]->at(1));
      printf("Z: %f\n",myShape->vertex[*jter]->at(2));
      */
      //if(tri2->myFlame == NULL) tri2->myFlame = new Flame(tri2);
      subpoly.push_back(tri2);
      subpoly[1]->subdivide();
      delete myFlame;
      myFlame = NULL;
      break;
    }
  }
  /*
  printf("subpoly size =%d\n", subpoly.size());
  if(subpoly.size() != 0){
    for(int i = 0; i < 2 ;++i){
      subpoly[i]->subdivide();
    }
  }
  */
  //return;
}











Poly::Poly(ShapeRef myShape){
  this->myShape = myShape;
  myFlame = NULL;
  center = 0;
  aveVec = NULL;
  myNumber = 0;
}




Poly::~Poly(){
  for(vector<int>::iterator itor = verticies.begin();
      itor != verticies.end(); ++itor){
    //delete (*itor);
  }
  for(vector<PolyRef>::iterator itor = subpoly.begin();
      itor != subpoly.end(); ++itor){
    delete (*itor);
  }
  verticies.clear();
  subpoly.clear();
  delete aveVec;
  delete myFlame;
}

void Poly::findCenter(){
  if(verticies.size() == 0){
    //printf("Size of this poly is 0\n");
    return;
  }
  if(center != 0) return;
  aveVec = new double[3];
  for(int i = 0; i < 3 ;++i){
    aveVec[i] = 0.0;
  }
  for(vector<int>::iterator iter = verticies.begin();
      iter != verticies.end(); ++iter){
    aveVec[0] += myShape->vertex[(*iter)]->at(0);
    aveVec[1] += myShape->vertex[(*iter)]->at(1);
    aveVec[2] += myShape->vertex[(*iter)]->at(2);
  }
  //printf("GOT HERE!!!!!!!!!!!!!!\n");
  myShape->vertex.push_back(new vector<double>());
  myShape->firevert.push_back(false);
  center = myShape->vertex.size() - 1;
  ++myShape->numvec;
  for(int i = 0; i < 3 ;++i){
    aveVec[i] = aveVec[i] / (double)(verticies.size());
    //myShape->vertex.push_back(new vector<double>());
    myShape->vertex[center]->push_back(aveVec[i]);
  }
}

void Poly::draw(){
  if(this == NULL) return;
  //if(subpoly.empty()){
  int ttt=1;
  if(tranobj)
    ttt=0;
  if(arepicking && myNumber != 0) glPushName(myNumber);
  
  if(!arepicking || subpoly.empty()){
  if(showpoly) glBegin(GL_POLYGON);
    else glBegin(GL_LINE_LOOP);
    
    if( !showpoly) glColor4d(.9,.9,.9,ttt);
    else if(myFlame!=NULL && showpoly && cfire) glColor4d(1,0,0,ttt);
    else if(myFlame==NULL && showpoly)glColor4f(.9,.9,.9,ttt);
    for(vector<int>::iterator iter = verticies.begin();
	iter != verticies.end(); ++iter){
      if(myShape->numvec == 0){
	//printf("Shape has no verticies\n");
      }
      assert(myShape->vertex[(*iter)]!=NULL);
      //printf("GOTHERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      glVertex3f(myShape->vertex[(*iter)]->at(0),
		 myShape->vertex[(*iter)]->at(1),
		 myShape->vertex[(*iter)]->at(2));
    }
    
    //myFlame->draw();
    glEnd();
  }
    if(arepicking && myNumber != 0) glPopName();
    
    //myFlame->draw();
    //Code for drawing all polygons
    for(vector<PolyRef>::iterator iter = subpoly.begin();
	iter != subpoly.end(); ++iter){
      (*iter)->draw();
    }
  //printf("Poly::draw() ALL DONE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
}

void Poly::drawFlame(){
  if(this == NULL) return;
  if(subpoly.empty()){
    myFlame->draw();
  }else{
    for(vector<PolyRef>::iterator iter = subpoly.begin();
	iter != subpoly.end(); ++iter){
      (*iter)->drawFlame();
    }
  }
  //printf("Poly::draw() ALL DONE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");  
}

void Poly::update(){
  if(this == NULL) return;

  if(subpoly.empty()){
    int var=(rand()%(spreadvar));
    if(var==0 && !pauses) flaming = spread();
    //if(myFlame == NULL) flaming = true;
    myFlame->update();

  }else{
    for(vector<PolyRef>::iterator iter = subpoly.begin();
	iter != subpoly.end(); ++iter){
      (*iter)->update();
    }
  }
  //printf("Poly::draw() ALL DONE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

}

void Poly::makeTri(){
  //return;
  if(this == NULL) return;
  if(each == 0) return;
  else each--;
  if(verticies.size() == 0) return;
  if(center == 0) findCenter();
  vector<int>::iterator iter = verticies.begin();
  vector<int>::iterator jter = verticies.begin();
  for(; iter != verticies.end() ;++iter){
    ++jter;
    if(jter == verticies.end()) jter = verticies.begin();
    PolyRef temp = new Poly(myShape);
    temp->verticies.push_back((*iter));
    temp->verticies.push_back((*jter));
    temp->verticies.push_back(center);
    if(temp->verticies.size() != 0){
      temp->findCenter();
      if(temp->myFlame == NULL)
	//temp->myFlame = new Flame(temp);
      subpoly.push_back(temp);
    }
  }
}




Flame::Flame(){
  on = true;
  //this->myPoly = myPoly;
  aveTime = .25;
  aveTime *= .50;
  rangeTime = .1;
  rangeNDir = 1;
  rangeADir = 1;
  rangeADir *= .7;
  rangeBDir = 1;
  rangeBDir *= .7;
  numparti = 1;
  origin = new double[3];
  aVec = new double[3];
  bVec = new double[3];

  for(int i = 0; i < 3; ++i){
    origin[i] = 0;
    aVec[i] = 0;
    bVec[i] = 0;
  }

  aVec[0] = 5;
  bVec[2] = 5;

  double *temp = aVec;
  aVec = normalize(aVec[0], aVec[1], aVec[2]);
  delete temp;
  temp = bVec;
  bVec = normalize(bVec[0], bVec[1], bVec[2]);
  delete temp;
  norm = (double *)normalvec(aVec, origin, bVec);
  delete bVec;
  bVec = (double *)normalvec(aVec, origin, norm);

  //printf("norm: X:|%6f| Y:|%6f| Z:|%6f|\n",norm[0],norm[1],norm[2]);
  //printf("bVec: X:|%6f| Y:|%6f| Z:|%6f|\n",bVec[0],bVec[1],bVec[2]);
  //norm = normalvec(bVec, aVec, origin);
  //assert(particles != NULL);
  //particles.push_back(NULL);
  //particles.pop_back();

  //must be the same as numparti!!!!
  particles = new PartiRef[1];

  for(int i = 0; i < numparti ;++i){
    particles[i] = NULL;
  }
  addParti();
}

Flame::Flame(PolyRef myPoly){
  on = true;
  this->myPoly = myPoly;
  aveTime = .25;
  aveTime *= .5;
  rangeTime = .1;
  rangeNDir = 1;
  rangeADir = 1;
  rangeADir *= .5;
  rangeBDir = 1;
  rangeBDir *= .5;
  numparti = 10;
  origin = new double[3];
  aVec = new double[3];
  bVec = new double[3];

  if(myPoly->center == 0) myPoly->findCenter();
  if(myPoly->center == 0) return;
  for(int i = 0; i<3 ;++i){
    aVec[i] = myPoly->myShape->vertex[myPoly->verticies[0]]->at(i);
    bVec[i] = myPoly->myShape->vertex[myPoly->verticies[1]]->at(i);
    //printf("Got here in flame!!!!!!!!!!!!!!!!!!!!!!\n");
    //printf("Center = %d\n", myPoly->center);
    origin[i] = myPoly->myShape->vertex[myPoly->center]->at(i);
  }
  double *temp = aVec;
  aVec = normalize(aVec[0], aVec[1], aVec[2]);
  delete temp;
  temp = bVec;
  bVec = normalize(bVec[0], bVec[1], bVec[2]);
  delete temp;
  norm = (double *)normalvec(aVec, origin, bVec);
  delete bVec;
  bVec = (double *)normalvec(aVec, origin, norm);

  //printf("norm: X:|%6f| Y:|%6f| Z:|%6f|\n",norm[0],norm[1],norm[2]);
  //printf("bVec: X:|%6f| Y:|%6f| Z:|%6f|\n",bVec[0],bVec[1],bVec[2]);
  //exit(1);
  norm = normalvec(bVec, aVec, origin);
  //assert(particles != NULL);
  //particles.push_back(NULL);
  //particles.pop_back();

  //must be the same as numparti!!!!
  particles = new PartiRef[10];

  for(int i = 0; i < numparti ;++i){
    particles[i] = NULL;
  }
  addParti();
}

Flame::~Flame(){
  for(int i = 0; i < numparti ;++i){
    delete particles[i];
  }
  delete particles;
  delete origin;
  delete norm;
  delete aVec;
  delete bVec;
}



void Flame::addParti(){
  double *velocity = new double[3];
  velocity[0]  = (((double)(rand() % 1000) / 1.0) * norm[0]
		  + ((double)(rand() % 2000) / 1.0 - 1000.0) * aVec[0]
		  + ((double)(rand() % 2000) / 1.0 - 1000.0) * bVec[0])*rangeADir;
  velocity[1]  = (((double)(rand() % 1000) / 1.0) * norm[1]
		  + ((double)(rand() % 2000) / 1.0 - 1000.0) * aVec[1] 
		  + ((double)(rand() % 2000) / 1.0 - 1000.0) * bVec[1])*rangeNDir;
  velocity[2]  = (((double)(rand() % 1000) / 1.0) * norm[2]
		  + ((double)(rand() % 2000) / 1.0 - 1000.0) * aVec[2] 
		  + ((double)(rand() % 2000) / 1.0 - 1000.0) * bVec[2])*rangeBDir;
  double time  = ((double)(rand() % 2000) / 4000.0 +.5) * aveTime;
  int color=(rand()%3);
  double colorx=((double)(rand()%1000))/2000.0 + .5;
  double colory=((double)(rand()%1000))/2000.0;
  if(color==0)
    {
      colorx=1;
      colory=0;
    }
  else if(color=1)
    {
      colorx=1;
      colory=1;
    }
  else
    {
      colorx=.5;
      colory=.25;
    }


  double tran=.10;
  //printf("origin[0]:%f|  origin[1]:%f|  origin[2]:%f|\n", origin[0], origin[1], origin[2]);

  //exit(1);
	
  for(int i = 0; i < numparti ;++i){
    if(particles[i] == NULL) {
      particles[i] = new Parti(origin, velocity, time,colorx,colory,tran);
    }
  }
}

void Flame::update(){
  if(this == NULL) return;
  if(myPoly == NULL) return;
  if(particles == NULL) return;
  //printf("myPoly->myNumber=%d\n", myPoly->myNumber);
  //exit(1);
  //return;
  bool del = false;
  for(int i = 0; i < numparti ;++i){
    if(particles[i] == NULL) continue;
    assert(particles != NULL);
    assert(particles[i] != NULL);
    //assert(particles[i]->on == false);

    if(!(particles[i]->on)){
      delete particles[i];
      particles[i] = NULL;
      del = true;
    }else{
      particles[i]->update(nowtime, firegrav);
    }
  }
  if(del) addParti();
  //printf("Particles allocation ==%d\n", particles.capacity());
}
/*
  vector<PartiRef>::iterator itor = particles.begin();
  int numdel = 0;
  for(int count = 0; count < 10; ++count){
  //++count;
  if((*itor)->on){
  (*itor)->update(nowtime, firegrav);
  ++itor;
  }else{
  itor = particles.erase(itor);
  ++numdel;
  continue;
  }
  }
  }
  }
*/
//return;
//if(this == NULL) return;
//printf("Flame::update() Got here!!!!!!!!!!!!!!!!!\n");
//assert(this != NULL);
/*
  if(!particles.empty()){
  assert(particles.size() != 0);
  for(vector<PartiRef>::iterator itor = particles.begin(); itor != particles.end(); ++itor){
  if((*itor)->on){
  (*itor)->update(nowtime, firegrav);
  }else{
  itor = particles.erase(itor);
  addParti();
  //if(itor == NULL) break;
  //assert(particles != NULL);
  if(itor == (particles.end())) break;
  }
  }
  }
  printf("Particles allocation ==%d\n", particles.capacity());
  }
*/

void Flame::draw(){
  //return;
  if(this == NULL) return;
  for(int i = 0; i < numparti ;++i){
    if(particles[i] != NULL) particles[i]->draw();
  }
}



/*
void processHits (GLint hits, GLuint buffer[], double x, double y){
  unsigned int i, j;
  GLint names, *ptr;

  ptr = (GLint *) buffer; 
  for (i = 0; i < hits; i++) {	//  for each hit  *
    names = *ptr;
    ptr+=3;
    for (j = 0; j < names; j++) { //  for each name *
      int temp = 0;
      for(int polyIter = 1; polyIter < PolyList.size(); ++polyIter){
	if(*ptr==polyIter){ 
	  //set PolyList[polyIter] on fire
	}
      }
      ptr++;
    }
    //printf ("\n");
  }
}

*/



void display(){ 
  //printf("Got to display!!!!!!!!!!!!!!!!!!!!!!\n");
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, viewW, viewH);
  if(!useperspective)
    glOrtho(-100.0, 100.0, -100.0, 100.0, -1000.0, 1000.0);
  else
    gluPerspective(45, ((double)viewW) / ((double)viewH), 10, 1000);
  glClear(GL_COLOR_BUFFER_BIT); 
  glMatrixMode(GL_MODELVIEW);
  //glFlush();
  //glInitNames();
  glLoadIdentity();
  //glPushName(0);

  glPushMatrix();
  glLoadIdentity();
  theCam->lookAt();
  if(axison){
    //X
    glBegin(GL_LINES);
    glColor3f(0,1,0);
    glVertex3f(0,0,0);
    glVertex3f(50,0,0);
    glEnd();

    //Y
    glBegin(GL_LINES);
    glColor3f(0,0,1);
    glVertex3f(0,0,0);
    glVertex3f(0,50,0);
    glEnd();

    //Z
    glBegin(GL_LINES);
    glColor3f(1,0,0);
    glVertex3f(0,0,0);
    glVertex3f(0,0,50);
    glEnd();
  }
  glPopMatrix();

  //loop for displaying model information

  glPushMatrix();
  glLoadIdentity();
  theCam->lookAt();
  shapes[0]->transform();
  //glScaled(2.5,2.5,2.5);
  //Print current shape only
  
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  shapes[snum]->draw();

  //Print all shapes
  /*
    for(vector<ShapeRef>::iterator iter = shapes.begin();
    iter != shapes.end(); ++iter){
    (*iter)->draw();
    }
  */
  glPopMatrix();


  //printf("GOTHERE!!!
  //glScaled(10,10,10);
  /*  
      glPushMatrix();
      glLoadIdentity();
      theCam->lookAt();
      shapes[0]->transform();

      for(int pter = 0; pter < shapes[snum]->numpoly ;++pter){
      glBegin(GL_LINE_LOOP);
      glColor3f(1,1,1);
      for (vector<int>::iterator xter = shapes[snum]->polygon[pter]->verticies.begin(); 
      xter != shapes[snum]->polygon[pter]->verticies.end(); ++xter){

      glVertex3f((shapes[snum]->vertex[(*xter)])->at(0),
      (shapes[snum]->vertex[(*xter)])->at(1),
      (shapes[snum]->vertex[(*xter)])->at(2));


      printf("X:%f || Y:%f || Z:%f ||\n",
      (shapes[snum]->vertex[(*xter)])->at(0),
      (shapes[snum]->vertex[(*xter)])->at(1),
      (shapes[snum]->vertex[(*xter)])->at(2));


      }

      glEnd();
      }

      glPopMatrix();
      //exit(1);
      */


  glPushMatrix();
  glLoadIdentity();
  theCam->lookAt();
  //glScaled(2.5,2.5,2.5);
  //truce->draw();
  glPopMatrix();
  

  glutSwapBuffers();
  glFlush();

}

//Code designed based on example from
//http://www.lighthouse3d.com/opengl/picking/index.php?openglway3
void processHits (GLint hits, GLuint buffer[]){
   unsigned int i, j;
   GLuint names, *ptr, minZ,*ptrNames, numberOfNames;

   //printf ("hits = %d\n", hits);
   ptr = (GLuint *) buffer;
   minZ = 0xffffffff;
   for (i = 0; i < hits; i++) {	
      names = *ptr;
	  ptr++;
	  if (*ptr < minZ) {
		  numberOfNames = names;
		  minZ = *ptr;
		  ptrNames = ptr+2;
	  }
	  
	  ptr += names+2;
	}

  ptr = ptrNames;
  //printf ("The closest hit names are ");
  int largest = 0;
  int ind = 0;
  for (j = 0; j < numberOfNames; j++, ptr++) {
    //printf ("%d ", *ptr);
    if(*ptr > largest){
      largest = *ptr;
      ind = j;
    }
  }
  //printf ("\n");

  if(PolyList[ptrNames[ind]] == NULL){
    //printf("PolyList[%d] is NULL\n", ptrNames[ind]);
    return;
  }
  if(PolyList[ptrNames[ind]]->myFlame == NULL){
    assert(PolyList[ptrNames[ind]] != NULL);
    findstart(PolyList[ptrNames[ind]]);
  }  
  //printf("minZ =%d\n", *ptr);
  //exit(1);
   
}

//Code designed from example from:
//http://www.lighthouse3d.com/opengl/picking/index.php?openglway3
void startPicking() {
	GLint viewport[4];

	glSelectBuffer(BUFSIZE,selectBuf);
	glRenderMode(GL_SELECT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glGetIntegerv(GL_VIEWPORT,viewport);
	gluPickMatrix(mouseX,viewport[3]-mouseY,1,1,viewport);

	if(!useperspective)
	  glOrtho(-100.0, 100.0, -100.0, 100.0, -1000.0, 1000.0);
	else
	  gluPerspective(45, ((double)viewW) / ((double)viewH), 10, 1000);

	glMatrixMode(GL_MODELVIEW);
	glInitNames();
	glPushName(0);
}

//Code designed from example from:
//http://www.lighthouse3d.com/opengl/picking/index.php?openglway3
void stopPicking() {

	int hits;
	
	// restoring the original projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glFlush();
	
	// returning to normal rendering mode
	hits = glRenderMode(GL_RENDER);
	
	// if there are hits process them
	if (hits != 0) processHits(hits,selectBuf);
}

void reshape(int w, int h)
{
  viewW = w;
  viewH = h;
  //glutPostRedisplay();
  //glutReshapeWindow(w,h);
}

Shape::Shape(){
  numvec = 0;
  numpoly = 0;
  for(int i = 0; i < 3 ;++i){
    translation[i]     = 0;
    starttranslation[i]=0;
    startlrotation[i]  = 0;
    lrotation[i]       = 0;
    istranslating[i]   = false;
    islrotating[i]     = false;
  }

  isscaling[1] = false;   

  delX = 0;
  delY = 0;
  prevX = 0;
  prevY = 0;
  scalar = .25;
  startscalar = 0;
  theta = 0;

}

Shape::~Shape(){
  delete istranslating;
  delete islrotating;
  delete isscaling;
  delete starttranslation;
  delete translation;
  delete startlrotation;
  delete lrotation;
  for(vector<PolyRef>::iterator itor = polygon.begin(); 
      itor != polygon.end(); ++ itor){
    delete (*itor);
  }
  polygon.clear();
  for(vector<vector<double>*>::iterator itor = vertex.begin(); 
      itor != vertex.end(); ++ itor){
    delete (*itor);
  }
  vertex.clear();
}


void Shape::draw(){
  if(this == NULL) return;

  for(vector<PolyRef>::iterator iter = polygon.begin();
      iter != polygon.end(); ++iter){
    //printf("Shape::draw() numvec is %d\n", numvec);
    if((*iter) != NULL) (*iter)->draw();
    //printf("Done drawing shape!!!!!!!!!!!!!!!!!\n");
    //printf("\n");
  }

  bool boolsave = showpoly;
  showpoly = true;
  arepicking = true;
  startPicking(); 
  for(vector<PolyRef>::iterator iter = polygon.begin();
      iter != polygon.end(); ++iter){
    //printf("Shape::draw() numvec is %d\n", numvec);
    if((*iter) != NULL) (*iter)->draw();
    //printf("Done drawing shape!!!!!!!!!!!!!!!!!\n");
    //printf("\n");
  }
  stopPicking();
  arepicking = false;
  showpoly = boolsave;
 /*
  for(vector<PolyRef>::iterator iter = polygon.begin();
      iter != polygon.end(); ++iter){
    //printf("Shape::draw() numvec is %d\n", numvec);
    if((*iter) != NULL) (*iter)->draw();
    //printf("Done drawing shape!!!!!!!!!!!!!!!!!\n");
    //printf("\n");
  }
  */

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  textured =LoadTexture( "alpha256", 256, 256);//load the texture


  //glBindTexture( GL_TEXTURE_2D, textured );



  glBlendFunc( GL_SRC_ALPHA, GL_ONE );
  glPointSize(25 );
 
  glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );
   /*
   glBegin(GL_QUADS);
     glTexCoord2d(1,1);
     glVertex3d(10,10,0);
   glTexCoord2d(1,0); 
   glVertex3d(10,0,0);
  glTexCoord2d(0,0);
  glVertex3d(0,0,0);
   glTexCoord2d(0,1);
   glVertex3d(0,10,0);
   glEnd();
   */
   
  for(vector<PolyRef>::iterator iter = polygon.begin();
      iter != polygon.end(); ++iter){
    //printf("Shape::draw() numvec is %d\n", numvec);
    if((*iter) != NULL) if(fire) (*iter)->drawFlame();
    //printf("Done drawing shape!!!!!!!!!!!!!!!!!\n");
    //printf("\n");
  }
   
  FreeTexture(textured);
  //glDisable(GL_POINT_SPRITE_ARB);
}

void Shape::update(){
  if(!started) return;
  if(this == NULL) return;
  flaming=false;
  for(vector<PolyRef>::iterator iter = polygon.begin();
      iter != polygon.end(); ++iter){
    //printf("Shape::draw() numvec is %d\n", numvec);
    if((*iter) != NULL)
	(*iter)->update();
    //printf("vertex capacity ==%d\n", vertex.capacity());
    //printf("polygon capacity ==%d\n", polygon.capacity());    
    //printf("Done drawing shape!!!!!!!!!!!!!!!!!\n");
    //printf("\n");
  }
  if(flaming==false){
    int var = rand() % (spreadvar*10);
    if(var==0 && !pauses){
    for(vector<PolyRef>::iterator iter=polygon.begin();
  	  iter!=polygon.end();iter++){
  	fspread((*iter));
      }
    }
  }
}

bool Shape::fspread(PolyRef poly){
  if(!started) return false;
  if(flaming) return true;
  if(poly==NULL) return false;
  if(poly->subpoly.empty()){
    if(poly->myFlame==NULL){
      // if(poly!=NULL) poly->myFlame=new Flame(poly);
      for(vector<int>::iterator iter=poly->verticies.begin();
	  iter!=poly->verticies.end();iter++){
	firevert[(*iter)]=true;
	flaming=true;
	poly->myFlame=new Flame(poly);
      }
    }
  }
  else{
    for(vector<PolyRef>::iterator iter=poly->subpoly.begin();
	iter!=poly->subpoly.end();iter++){
      if(fspread(*iter)) return true;
    }    
  }
  return false;
}

bool Poly::spread(){
  //return;
  if(subpoly.size() != 0||myFlame!=NULL){
    return false;
  }
  int num=0;
  for(vector<int>::iterator iter=verticies.begin();
      iter !=verticies.end(); ++iter){
    if(myShape->firevert[(*iter)]) num++;
  }
  if(num > 0){
    for(vector<int>::iterator iter=verticies.begin();
	iter !=verticies.end(); ++iter){
      myShape->firevert[(*iter)]=true;
    }
    if(myFlame == NULL){
      myFlame=new Flame(this);
      return true;
    }
  }
  return false;
}

//function which handels transformations stack
void Shape::transform(){
  if(istranslating[0] && mousedown) translation[0] = starttranslation[0] + delX * 100;
  if(istranslating[1] && mousedown) translation[1] = starttranslation[1] + delX * 100;
  if(istranslating[2] && mousedown) translation[2] = starttranslation[2] + delX * 100;

  if(islrotating[0] && mousedown) lrotation[0] = startlrotation[0] + delX * 100; 
  if(islrotating[1] && mousedown) lrotation[1] = startlrotation[1] + delX * 100; 
  if(islrotating[2] && mousedown) lrotation[2] = startlrotation[2] + delX * 100; 

  //Translation
  //printf("X:%f|| Y:%f|| Z:%f||\n",translation[0], translation[1], translation[2]);
  glTranslated(translation[0], translation[1], translation[2]);
  //Object coordinate frame rotation
  //if(mousedown) printf("X:%f|| Y:%f|| Z:%f||\n",lrotation[0], lrotation[1], lrotation[2]);
  glRotated(lrotation[0],1,0,0);
  glRotated(lrotation[1],0,1,0);
  glRotated(lrotation[2],0,0,1);
  //Object scaling
  //put back
  //glScaled(scalar,scalar,scalar);
}

//function handels loading of coordinate and polygon files






void Shape::reset(PolyRef poly){
  if(poly->subpoly.empty()){
    poly->myFlame=NULL;
    for(vector<int>::iterator iter=poly->verticies.begin();
	iter!=poly->verticies.end(); iter++){
      firevert[*iter]=false;
    }
  }
else
  for(vector<PolyRef>::iterator iter=poly->subpoly.begin();
      iter!=poly->subpoly.end();iter++){
    this->reset((*iter));
  }
  
}






void Shape::loadframe(char *coor, char *poly){
  //Read in coordinate file
  cout<<"Opening file: "<<coor<<endl;
  ifstream inFile1 (coor);
  if (inFile1.is_open()){
    double n1 = 0;
    double n2 = 0;
    double n3 = 0;
    int index = 0;
    char *word;
    if(! inFile1.eof() ){
      word = (char*)calloc(1000, sizeof(char));
      inFile1.getline (word, 1000, '\n');
      this->numvec = (int)atof(word);
      //printf("numvec set as %d\n", numvec);
      //vertex.push_back(new vector<double>());
      //vertex.resize(numvec+1);
      //vertex.clear();
      vertex.push_back(new vector<double>());
      firevert.push_back(false);
    }
    for (int iter = 0; iter < numvec ;++iter){
      word = (char*)calloc(1000, sizeof(char));
      inFile1.getline (word, 1000, '\n');
      if(inFile1.eof()) break;
      char * pch;
      pch = strtok (word, ",");
      int index = (int)atof(pch);
      pch = strtok (NULL, ",");
      double n1 = atof(pch);
      pch = strtok (NULL, ",");
      double n2 = atof(pch);
      pch = strtok (NULL, ",");
      double n3 = atof(pch);
      vertex.push_back(new vector<double>());
      firevert.push_back(false);
      //printf("Vertex.size() is |%d|\n", vertex.size());
      //vertex[vertex.size() - 1]->resize(3);
      vertex[vertex.size() - 1]->push_back(n1);
      //printf("vertex[%d]->at(0) = |%f|\n", vertex.size() - 1, vertex[vertex.size() - 1]->at(0) );
      vertex[vertex.size() - 1]->push_back(n2);
      //printf("vertex[%d]->at(1) = |%f|\n", vertex.size() - 1, vertex[vertex.size() - 1]->at(1) );
      vertex[vertex.size() - 1]->push_back(n3);
      //printf("vertex[%d]->at(2) = |%f|\n", vertex.size() - 1, vertex[vertex.size() - 1]->at(2) );
    }
    inFile1.close();
  }else{
    cout<<"Could not open file "<<coor<<" for reading."<<endl;
  }
  //Read in polygon file
  cout<<"Opening file: "<<poly<<endl;
  ifstream inFile2 (poly);
  if (inFile2.is_open()){
    int index = 0;
    char *word;
    if(! inFile2.eof() ){
      word = (char*)calloc(1000, sizeof(char));
      inFile2.getline (word, 1000, '\n');
      numpoly = (int)atof(word);
      polygon.push_back(new Poly(this));
      polygon.resize(numpoly+2);
    }
    char *pch = NULL;
    word = (char*)calloc(1000, sizeof(char));
    for (; index <= numpoly ;){
      if(pch != NULL) pch = strtok(NULL, " ");
      if(pch == NULL){
	//Make all polygons triangles
	if(polygon[index] != NULL){
	  //printf("HERE!!!!!!!!!\n");
	  if(polygon[index]->verticies.size() != 0){
	    //if(polygon[index]) 
	    //if(polygon[index]->myFlame == NULL)
	    //polygon[index]->myFlame = new Flame(polygon[index]);
	    polygon[index]->makeTri();
	  }
	}
	//printf("\n");

	++index;
	//printf("Polygon index is %d\n", index);
	polygon[index] = new Poly(this);
	word = (char*)calloc(1000, sizeof(char));
	inFile2.getline (word, 1000, '\n');
	pch = strtok (word, " ");

	continue;
      }
      //printf("|%s|",pch);
      if(strcmp(pch,"name") == 0) continue;
      if(polygon[index] != NULL)
	polygon[index]->verticies.push_back((int)atof(pch));
    }
    inFile2.close();

    //Make polygons triangles.

  }else{
    cout<<"Could not open file "<<poly<<" for reading."<<endl;
  }
  

  for(int i = 0; i < 1 ;++i){
    for(vector<PolyRef>::iterator itor = polygon.begin();
	itor != polygon.end(); ++itor){
      (*itor)->subdivide();
    }
  }

  for(vector<PolyRef>::iterator itor = polygon.begin();
      itor != polygon.end(); ++itor){
    loadpolylist(this, *itor);
  }

  printf("numpoly         =%d\n", numpoly);
  printf("Size of PolyList=%d\n", PolyList.size());
  
  printf("Shape loaded...\n");

}

void loadpolylist(ShapeRef shaper, PolyRef poly){
  if(poly == NULL || shaper == NULL) return;
  if(PolyList.size() == 0) PolyList.push_back(NULL);
  if(poly->subpoly.size() > 0){
    for(vector<PolyRef>::iterator itor = poly->subpoly.begin();
	itor != poly->subpoly.end(); ++itor){
      loadpolylist(shaper, *itor);
    } 
  }else if(poly->subpoly.size() == 0){
    poly->myNumber = PolyList.size();
    PolyList.push_back(poly);
    //printf("arg! myNumber=%d\n", poly->myNumber);
  }
}


/*
bool test(PolyRef curr){
  double maxlen = 5.0;
  bool ret = false;
  if(curr->subpoly.size() == 0){
    
  }else{
    for(vector
  }
}
*/

//function resets object to inital position
void reset(){
  for(int i = 0; i < 3 ;++i){
    shapes[snum]->translation[i]     = 0;
    shapes[snum]->startlrotation[i]  = 0;
    shapes[snum]->lrotation[i]       = 0;
  }

  started = false;

  shapes[snum]->delX = 0;
  shapes[snum]->delY = 0;
  shapes[snum]->prevX = 0;
  shapes[snum]->prevY = 0;
  shapes[snum]->scalar = 1;
  shapes[snum]->startscalar = 0;
  shapes[snum]->theta = 0;
  theCam->comreset();
  for(int i=0;i<shapes.size();i++)
    {
      for(vector<PolyRef>::iterator iter=shapes[i]->polygon.begin();
	  iter!=shapes[i]->polygon.end();iter++){
	shapes[i]->reset(*iter);
      }
    }
  pauses=true;
}


void findstart(PolyRef poly){
  //return;
  //printf("myNumber=%d\n", poly->myNumber);
  started = true;
  if(poly->subpoly.empty()){
    setfire(poly);
    }
    else{
      findstart(poly->subpoly[0]);
    }
}


void init(int argc, char** argv){
  glClearColor (0.0, 0.0, 0.0, 0.0); 
  glColor3f(1.0, 1.0, 1.0); 

  viewW = 0;
  viewH = 0;

  runthrough=0;

  glutIgnoreKeyRepeat(1);

  shapes.push_back(new Shape());

  each = 5000;  
  spreadslow=10;
  spreadvar=100;

snum = 0;

  for(int i = 0; i < (argc - 1) / 2 ;++i){

    shapes[i]->loadframe(argv[2*i + 1], argv[2*i + 2]);
    printf("%d, %d\n",shapes[0]->vertex.size(), shapes[0]->firevert.size());
    shapes[i]->thisnum = i;

    shapes.push_back(new Shape());
  }

  shapes.pop_back();
  for(int i=0;i<shapes.size();i++){
    //findstart(shapes[i]->polygon[1]);
  }
  nowtime = 0.0;
  timevar = .01;
  timevar *= 1.0/2.0;
  firegrav = 15000;

  axison = true;
  useperspective = true;
  showpoly = true;
  arepicking = false;
  started = false;
  currX = 0;
  currY = 0;

  starter = nowtime;

  firstdown = true;

 fire=false;
 cfire=false;
 showpoly=false;
 tranobj=false;
 flaming=false;
 pauses=true;


  

  //truce = new Flame();

  theCam = new Cam();
glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
glEnable( GL_POINT_SPRITE_ARB );
}

void mouse (int button, int state, int x, int y) {
  if(state == GLUT_DOWN){

    mousedown = true;
    mouseX = x;
    mouseY = y;

    currX =  1 * (double)(x - 250) /  250.0;
    currY = -1 * (double)(y - 250) /  250.0;    

    if(firstdown){
      shapes[snum]->prevX = currX;
      shapes[snum]->prevY = currY;
    }

    shapes[snum]->delX = currX - shapes[0]->prevX;
    shapes[snum]->delY = currY - shapes[0]->prevY;

    if(firstdown){
      for(int i = 0; i < 3 ;++i){
	if(shapes[snum]->istranslating[i])
	  shapes[snum]->starttranslation[i] = shapes[snum]->translation[i];
	if(shapes[snum]->islrotating[i])
	  shapes[snum]->startlrotation[i] = shapes[snum]->lrotation[i];
      }
      if(shapes[snum]->isscaling[1])
	shapes[snum]->startscalar = shapes[snum]->scalar;
    }
    firstdown = false;
  }else{
    firstdown = true;
    mousedown = false;
  }
}

void motion (int x, int y){
  currX =  1 * (double)(x - 250) /  250.0;
  currY = -1 * (double)(y - 250) /  250.0;
  shapes[snum]->delX = currX - shapes[0]->prevX;
  shapes[snum]->delY = currY - shapes[0]->prevY;


}

void key(unsigned char k, int x, int y){

  if(k == 'w') theCam->moveWS(true);
  if(k == 's') theCam->moveWS(false);
  
  if(k == 'a') theCam->moveAD(true);
  if(k == 'd') theCam->moveAD(false);
  
  if(k == 'j') theCam->setYaw(false);
  if(k == 'l') theCam->setYaw(true);

  if(k == 'i') theCam->moveud(true);
  if(k == 'k') theCam->moveud(false);

  if(k == 'f') {
    if(fire) fire=false;
    else fire=true;
  }
  if(k == ';'){
    if(showpoly) showpoly=false;
    else showpoly=true;
  }

  if(k == 'g'){
    if(cfire) cfire=false;
    else cfire=true;
  }

  if(k == 't'){
    if(tranobj) tranobj=false;
    else tranobj=true;
  }

if(k == ' '){
    if(pauses) pauses=false;
    else pauses=true;
  }

  if(k == 'q') {
    shapes.clear();
    exit(0);
  }

  if(k == 'p'){
    if(useperspective) useperspective = false;
    else if(!useperspective) useperspective = true;
  }

  if(k == '\\'){
    if(axison) axison = false;
    else if(!axison) axison = true;
  }

  if(k == 'r') reset();

  if(k == '1') snum = 0;

  if(k == '2' && shapes.size()>1) snum = 1;

  if(k == '3' && shapes.size()>2 && shapes[2]!=NULL) snum = 2;

  if(k == '4'&& shapes.size()>3 && shapes[3]!=NULL) snum = 3;

  if(k == '5'&& shapes.size()>4 && shapes[4]!=NULL) snum = 4;

  if(k == '6'&& shapes.size()>5 && shapes[5]!=NULL) snum = 5;

  if(k == '7'&& shapes.size()>6 && shapes[6]!=NULL) snum = 6;

  if(k == '8'&& shapes.size()>7 && shapes[7]!=NULL) snum = 7;

  if(k == '9'&& shapes.size()>8 && shapes[8]!=NULL) snum = 8;

  if(k == '0'&& shapes.size()>9 && shapes[9]!=NULL) snum = 9;

  if(k == 'x'){
    shapes[snum]->istranslating[0] = true;
    shapes[snum]->starttranslation[0] = shapes[snum]->translation[0];
  }
  if(k == 'y'){
    shapes[snum]->istranslating[1] = true;
    shapes[snum]->starttranslation[1] = shapes[snum]->translation[1];
  }
  if(k == 'z'){
    shapes[snum]->istranslating[2] = true;
    shapes[snum]->starttranslation[2] = shapes[snum]->translation[2];
  }

  if(k == 'v'){
    shapes[snum]->islrotating[0] = true;
    shapes[snum]->startlrotation[0] = shapes[snum]->lrotation[0];
  } 
  if(k == 'b'){
    shapes[snum]->islrotating[1] = true;
    shapes[snum]->startlrotation[1] = shapes[snum]->lrotation[1];
  } 
  if(k == 'c'){
    shapes[snum]->islrotating[2] = true;
    shapes[snum]->startlrotation[2] = shapes[snum]->lrotation[2];
  }
}

void keyUp(unsigned char k, int x, int y){

  if(k == 'w' || k == 's' || k == 'a' || k == 'd' || k == 'i' || k =='k' || k == 'j' || k=='l') theCam->reset();
  if(k == 'x') shapes[snum]->istranslating[0] = false;
  if(k == 'y') shapes[snum]->istranslating[1] = false;
  if(k == 'z') shapes[snum]->istranslating[2] = false;


  if(k == 'v') shapes[snum]->islrotating[0] = false;
  if(k == 'b') shapes[snum]->islrotating[1] = false;
  if(k == 'c') shapes[snum]->islrotating[2] = false;
  
  if(k == '+'){
    spreadvar-=20;
    if(spreadvar<=0)
      spreadvar=10;
    timevar*=2;
   }
  if(k == '-'){
    spreadvar+=10;
    if(spreadvar<=0)
      spreadvar=10;
    timevar/=2;
   }
}

void update(){
  nowtime += timevar;
  //truce->update();
  theCam->update();
  if(theCam->angle!=0) theCam->yaw();
  shapes[snum]->update();
  glutPostRedisplay();
}

int main(int argc, char** argv)
{
  //_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
  if(((argc - 1) % 2) != 0){
    cout<<"Usage: "<<argv[0]
	<<"[ [modelname coordinate file]"
	<<" [modelname polygon list file] ]"
	<<endl;
    exit(1);
  }
  srand ( time(NULL) );
  glutInit(&argc,argv); 
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_RGBA);  
  glutInitWindowSize(500,500);
  glutInitWindowPosition(0,0); 
  glutCreateWindow("3Dmodel"); 
  glutIdleFunc(update);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse); 
  glutMotionFunc(motion); 
  glutKeyboardFunc(key);
  glutKeyboardUpFunc(keyUp);
  glutDisplayFunc(display);
  init(argc, argv);
  glutMainLoop();
}
