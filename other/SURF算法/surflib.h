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

#ifndef SURFLIB_H
#define SURFLIB_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "integral.h"
#include "fasthessian.h"
#include "surf.h"
#include "ipoint.h"
#include "utils.h"


//�������ܣ��������������������
inline void surfDetDes(IplImage *img,  /* �ڴ�ͼ�ϼ�������� */
                       std::vector<Ipoint> &ipts, /* ������������������ */
                       bool upright = false, /* ����ת�����Ե�ģʽ������ */
                       int octaves = OCTAVES, /* ������������ */
                       int intervals = INTERVALS, /* ��������ÿ��Ĳ��� */
                       int init_sample = INIT_SAMPLE, /* ��ʼ���� */
                       float thres = THRES /* �ߵ���Ӧ��ֵ */)
{
  //��������ͼ��
  IplImage *int_img = Integral(img);
  
  //��������Hessian����
  FastHessian fh(int_img, ipts, octaves, intervals, init_sample, thres);
 
  //��ȡ�����㣬�������ipts������
  fh.getIpoints();
  
  //����surf����������
  Surf des(int_img, ipts);

  //��ȡipts�е�������
  des.getDescriptors(upright);

  //�ͷŻ���ͼ��
  cvReleaseImage(&int_img);
}


//�������ܣ��������������������
inline void surfDet(IplImage *img,  /* �ڴ�ͼ�ϼ�������� */
                    std::vector<Ipoint> &ipts, /* ������������������ */
                    int octaves = OCTAVES, /* ������������ */
                    int intervals = INTERVALS, /* ��������ÿ��Ĳ��� */
                    int init_sample = INIT_SAMPLE, /* ��ʼ���� */
                    float thres = THRES /* �ߵ���Ӧ��ֵ */)
{
  //��������ͼ��
  IplImage *int_img = Integral(img);

  //��������Hessian����
  FastHessian fh(int_img, ipts, octaves, intervals, init_sample, thres);

  //��ȡ�����㣬�������ipts������
  fh.getIpoints();

  //�ͷŻ���ͼ��
  cvReleaseImage(&int_img);
}




//�������ܣ����������������
inline void surfDes(IplImage *img,  /* �ڴ�ͼ�ϼ�������� */
                    std::vector<Ipoint> &ipts, /* ������������������ */
                    bool upright = false) /* ����ת�����Ե�ģʽ�����У� */
{ 
  //��������ͼ��
  IplImage *int_img = Integral(img);

  //����surf����������
  Surf des(int_img, ipts);

  //��ȡipts�е�������
  des.getDescriptors(upright);
  
  //�ͷŻ���ͼ��
  cvReleaseImage(&int_img);
}


#endif
