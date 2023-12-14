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

#ifndef UTILS_H
#define UTILS_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "ipoint.h"

#include <vector>


//��ʾ������Ϣ����ֹ����
void error(const char *msg);

//��ʾ���ṩ��ͼ�񲢵ȴ�������Ӧ
void showImage(const IplImage *img);

//��ָ����������ʾ���ṩ��ͼ�񲢵ȴ�������Ӧ
void showImage(char *title,const IplImage *img);

//��ͼ��ת��Ϊ32λ�����ͻҶ�ͼ��
IplImage* getGray(const IplImage *img);

//��ͼ���ϻ�����������
void drawIpoint(IplImage *img, Ipoint &ipt, int tailSize = 0);

//������������������
void drawIpoints(IplImage *img, std::vector<Ipoint> &ipts, int tailSize = 0);

//��������������������������
void drawWindows(IplImage *img, std::vector<Ipoint> &ipts);

//��ͼ���ϻ�FPSͼ(������Ҫ2�ε���)
void drawFPS(IplImage *img);

//������λ���ϻ�һ����
void drawPoint(IplImage *img, Ipoint &ipt);

//����������λ���ϻ���
void drawPoints(IplImage *img, std::vector<Ipoint> &ipts);

//����SURF�������ļ�
void saveSurf(char *filename, std::vector<Ipoint> &ipts);

//���ļ�������SURF����
void loadSurf(char *filename, std::vector<Ipoint> &ipts);

//��������ת������ӽ�������
inline int fRound(float flt)
{
  return (int) floor(flt+0.5f);
}

#endif
