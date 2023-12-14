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

#ifndef IPOINT_H
#define IPOINT_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <vector>
#include <math.h>


//-------------------------------------------------------

class Ipoint; //Ipoint����������Ҫ����ؼ������Ӧ����
typedef std::vector<Ipoint> IpVec;
typedef std::vector<std::pair<Ipoint, Ipoint> > IpPairVec;

//-------------------------------------------------------

//Ipoint����
void getMatches(IpVec &ipts1, IpVec &ipts2, IpPairVec &matches);
int translateCorners(IpPairVec &matches, const cv::Point src_corners[4], cv::Point dst_corners[4]);

//-------------------------------------------------------

class Ipoint {

public:

  //��������
  ~Ipoint() {};

  //���캯��
  Ipoint() : orientation(0) {};

  //�õ�����������֮��Ŀռ����
  float operator-(const Ipoint &rhs)
  {
    float sum=0.f;
    for(int i=0; i < 64; ++i)//surf��64ά�������˴��������ŷ�Ͼ���
      sum += (this->descriptor[i] - rhs.descriptor[i])*(this->descriptor[i] - rhs.descriptor[i]);
    return sqrt(sum);
  };

  //����������
  float x, y;

  //�����߶�
  float scale;

  //��x������������ʱ����ת�ĽǶ�
  float orientation;

  //������˹����
  int laplacian;

  //64ά��������
  float descriptor[64];

  //! Placeholds for point motion (��������֡��֮֡����˶�����)
  float dx, dy;

  //����
  int clusterIndex;
};

//-------------------------------------------------------


#endif
