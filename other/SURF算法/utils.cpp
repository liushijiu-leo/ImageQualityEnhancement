/*********************************************************** 
*  --- OpenSURF ---                                       *
*  This library is distributed under the GNU GPL. Please   *
*  use the contact form at http://www.chrisevansdev.com    *
*  for more information.                                   *
*                                                          *
*  C. Evans, Research Into Robust Visual Features,         *
*  MSc University of Bristol, 2008.                        *
*                                                          *
************************************************************/

#include <highgui.h>
#include <iostream>
#include <fstream>
#include <time.h>

#include "utils.h"

using namespace std;

//-------------------------------------------------------

static const int NCOLOURS = 8;
static const CvScalar COLOURS [] = {cvScalar(255,0,0), cvScalar(0,255,0), 
                                    cvScalar(0,0,255), cvScalar(255,255,0),
                                    cvScalar(0,255,255), cvScalar(255,0,255),
                                    cvScalar(255,255,255), cvScalar(0,0,0)};

//-------------------------------------------------------

//��ʾ������Ϣ����ֹ����
void error(const char *msg) 
{
  cout << "\nError: " << msg;
  getchar();
  exit(0);
}

//-------------------------------------------------------

//��ʾ���ṩ��ͼ�񲢵ȴ�������Ӧ
void showImage(const IplImage *img)
{
  cvNamedWindow("Surf", CV_WINDOW_AUTOSIZE); 
  cvShowImage("Surf", img);  
  cvWaitKey(0);
}

//-------------------------------------------------------

//��ָ��������ʾ���ṩ��ͼ�񲢵ȴ�������Ӧ
void showImage(char *title,const IplImage *img)
{
  cvNamedWindow(title, CV_WINDOW_AUTOSIZE); 
  cvShowImage(title, img);  
  cvWaitKey(0);
}

//-------------------------------------------------------

//ת���ɵ�ͨ��32λ�����͵ĻҶ�ͼ��
IplImage *getGray(const IplImage *img)
{
  //����ṩ��ͼ���Ƿ�Ϊ��
  if (!img) error("Unable to create grayscale image.  No image supplied");

  IplImage* gray8, * gray32;

  gray32 = cvCreateImage( cvGetSize(img), IPL_DEPTH_32F, 1 );

  if( img->nChannels == 1 )
    gray8 = (IplImage *) cvClone( img );
  else {
    gray8 = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 1 );
    cvCvtColor( img, gray8, CV_BGR2GRAY );
  }

  cvConvertScale( gray8, gray32, 1.0 / 255.0, 0 );

  cvReleaseImage( &gray8 );
  return gray32;
}

//-------------------------------------------------------

//������������������
void drawIpoints(IplImage *img, vector<Ipoint> &ipts, int tailSize)
{
  Ipoint *ipt;
  float s, o;
  int r1, c1, r2, c2, lap;

  for(unsigned int i = 0; i < ipts.size(); i++) 
  {
    ipt = &ipts.at(i);
    s = (2.5f * ipt->scale);
    o = ipt->orientation;
    lap = ipt->laplacian;
    r1 = fRound(ipt->y);
    c1 = fRound(ipt->x);
    c2 = fRound(s * cos(o)) + c1;
    r2 = fRound(s * sin(o)) + r1;

    if (o) //���߱�ʾ����
      cvLine(img, cvPoint(c1, r1), cvPoint(c2, r2), cvScalar(0, 255, 0));
    else  //�̵㣨if using upright version��
      cvCircle(img, cvPoint(c1,r1), 1, cvScalar(0, 255, 0),-1);

    if (lap == 1)
    { //��ɫԲȦ��ʾ�������ϵİ���
      cvCircle(img, cvPoint(c1,r1), fRound(s), cvScalar(255, 0, 0),1);
    }
    else if (lap == 0)
    { //��ɫԲȦ��ʾ�������ϵ�����
      cvCircle(img, cvPoint(c1,r1), fRound(s), cvScalar(0, 0, 255),1);
    }
    else if (lap == 9)
    { //��ɫԲȦ��ʾ�������ϵ����ߣ�ԭӢ��ע������Red circles indicate light blobs on dark backgrounds��
      cvCircle(img, cvPoint(c1,r1), fRound(s), cvScalar(0, 255, 0),1);
    }

    //����dx��dy��ָ��
    if (tailSize)
    {
      cvLine(img, cvPoint(c1,r1),
        cvPoint(int(c1+ipt->dx*tailSize), int(r1+ipt->dy*tailSize)),
        cvScalar(255,255,255), 1);
    }
  }
}

//-------------------------------------------------------

//��ͼ���ϻ�����������
void drawIpoint(IplImage *img, Ipoint &ipt, int tailSize)
{
  float s, o;
  int r1, c1, r2, c2, lap;

  s = (2.5f * ipt.scale);
  o = ipt.orientation;
  lap = ipt.laplacian;
  r1 = fRound(ipt.y);
  c1 = fRound(ipt.x);

  //���߱�ʾ����
  if (o)
  {
    c2 = fRound(s * cos(o)) + c1;
    r2 = fRound(s * sin(o)) + r1;
    cvLine(img, cvPoint(c1, r1), cvPoint(c2, r2), cvScalar(0, 255, 0));
  }
  else  //�̵㣨if using upright version��
    cvCircle(img, cvPoint(c1,r1), 1, cvScalar(0, 255, 0),-1);

  if (lap >= 0)
  { //��ȦȦ��ʾ�������ϵ�����
    cvCircle(img, cvPoint(c1,r1), fRound(s), cvScalar(255, 0, 0),1);
  }
  else
  { //��ȦȦ��ʾ�������ϵ�����
    cvCircle(img, cvPoint(c1,r1), fRound(s), cvScalar(0, 0, 255),1);
  }

  //����dx��dy��ָ��
  if (tailSize)
  {
    cvLine(img, cvPoint(c1,r1),
      cvPoint(int(c1+ipt.dx*tailSize), int(r1+ipt.dy*tailSize)),
      cvScalar(255,255,255), 1);
  }
}

//-------------------------------------------------------

//������λ���ϻ�һ����
void drawPoint(IplImage *img, Ipoint &ipt)
{
  float s, o;
  int r1, c1;

  s = 3;
  o = ipt.orientation;
  r1 = fRound(ipt.y);
  c1 = fRound(ipt.x);

  cvCircle(img, cvPoint(c1,r1), fRound(s), COLOURS[ipt.clusterIndex%NCOLOURS], -1);
  cvCircle(img, cvPoint(c1,r1), fRound(s+1), COLOURS[(ipt.clusterIndex+1)%NCOLOURS], 2);

}

//-------------------------------------------------------

//����������λ���ϻ���
void drawPoints(IplImage *img, vector<Ipoint> &ipts)
{
  float s, o;
  int r1, c1;

  for(unsigned int i = 0; i < ipts.size(); i++) 
  {
    s = 3;
    o = ipts[i].orientation;
    r1 = fRound(ipts[i].y);
    c1 = fRound(ipts[i].x);

    cvCircle(img, cvPoint(c1,r1), fRound(s), COLOURS[ipts[i].clusterIndex%NCOLOURS], -1);
    cvCircle(img, cvPoint(c1,r1), fRound(s+1), COLOURS[(ipts[i].clusterIndex+1)%NCOLOURS], 2);
  }
}

//-------------------------------------------------------

//��������������������������
void drawWindows(IplImage *img, vector<Ipoint> &ipts)
{
  Ipoint *ipt;
  float s, o, cd, sd;
  int x, y;
  CvPoint2D32f src[4];

  for(unsigned int i = 0; i < ipts.size(); i++) 
  {
    ipt = &ipts.at(i);
    s = (10 * ipt->scale);
    o = ipt->orientation;
    y = fRound(ipt->y);
    x = fRound(ipt->x);
    cd = cos(o);
    sd = sin(o);

    src[0].x=sd*s+cd*s+x;   src[0].y=-cd*s+sd*s+y;
    src[1].x=sd*s+cd*-s+x;  src[1].y=-cd*s+sd*-s+y;
    src[2].x=sd*-s+cd*-s+x; src[2].y=-cd*-s+sd*-s+y;
    src[3].x=sd*-s+cd*s+x;  src[3].y=-cd*-s+sd*s+y;

    if (o) //��������
      cvLine(img, cvPoint(x, y), 
      cvPoint(fRound(s*cd + x), fRound(s*sd + y)), cvScalar(0, 255, 0),1);
    else  //�̵㣨if using upright version��
      cvCircle(img, cvPoint(x,y), 1, cvScalar(0, 255, 0),-1);


    //�������㻭�����
    cvLine(img, cvPoint(fRound(src[0].x), fRound(src[0].y)), 
      cvPoint(fRound(src[1].x), fRound(src[1].y)), cvScalar(255, 0, 0),2);
    cvLine(img, cvPoint(fRound(src[1].x), fRound(src[1].y)), 
      cvPoint(fRound(src[2].x), fRound(src[2].y)), cvScalar(255, 0, 0),2);
    cvLine(img, cvPoint(fRound(src[2].x), fRound(src[2].y)), 
      cvPoint(fRound(src[3].x), fRound(src[3].y)), cvScalar(255, 0, 0),2);
    cvLine(img, cvPoint(fRound(src[3].x), fRound(src[3].y)), 
      cvPoint(fRound(src[0].x), fRound(src[0].y)), cvScalar(255, 0, 0),2);

  }
}

//-------------------------------------------------------

//��ͼ���ϻ�FPSͼ(������Ҫ2�ε���)
void drawFPS(IplImage *img)
{
  static int counter = 0;
  static clock_t t;
  static float fps;
  char fps_text[20];
  CvFont font;
  cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, 1.0,1.0,0,2);

  //���fpsͼ��ÿ10֡��
  if (counter > 10)
  {
    fps = (10.0f/(clock()-t) * CLOCKS_PER_SEC);
    t=clock(); 
    counter = 0;
  }

  //�������Լ�
  ++counter;

  //ת�����ַ���
  sprintf(fps_text,"FPS: %.2f",fps);

  //��ͼ������ʾ֡���ַ���
  cvPutText (img,fps_text,cvPoint(10,25), &font, cvScalar(255,255,0));
}

//-------------------------------------------------------

//����SURF�������ļ�
void saveSurf(char *filename, vector<Ipoint> &ipts)
{
  ofstream outfile(filename);

  //����������ĳ���
  outfile << "64\n";
  outfile << ipts.size() << "\n";

  //ÿ�е������ʽ��scale  x  y  des
  for(unsigned int i=0; i < ipts.size(); i++) 
  {
    outfile << ipts.at(i).scale << "  ";
    outfile << ipts.at(i).x << " ";
    outfile << ipts.at(i).y << " ";
    outfile << ipts.at(i).orientation << " ";
    outfile << ipts.at(i).laplacian << " ";
    outfile << ipts.at(i).scale << " ";
    for(int j=0; j<64; j++)
      outfile << ipts.at(i).descriptor[j] << " ";

    outfile << "\n";
  }

  outfile.close();
}

//-------------------------------------------------------

//���ļ�������SURF����
void loadSurf(char *filename, vector<Ipoint> &ipts)
{
  int descriptorLength, count;
  ifstream infile(filename);

  //�������ipts����
  ipts.clear();

  //��ȡ��������/��������Ŀ
  infile >> descriptorLength;
  infile >> count;

  //����ÿ��������
  for (int i = 0; i < count; i++) 
  {
    Ipoint ipt;

    //��ȡ
    infile >> ipt.scale; 
    infile >> ipt.x;
    infile >> ipt.y;
    infile >> ipt.orientation;
    infile >> ipt.laplacian;
    infile >> ipt.scale;

    //��ȡ���������
    for (int j = 0; j < 64; j++)
      infile >> ipt.descriptor[j];

    ipts.push_back(ipt);

  }
}

//-------------------------------------------------------

//-------------------------------------------------------
