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
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include "ipoint.h"

//����ƥ��
void getMatches(IpVec &ipts1, IpVec &ipts2, IpPairVec &matches)
{
  float dist, d1, d2;
  Ipoint *match;

  matches.clear();

  for(unsigned int i = 0; i < ipts1.size(); i++) 
  {
    d1 = d2 = FLT_MAX;

    for(unsigned int j = 0; j < ipts2.size(); j++) 
    {
      dist = ipts1[i] - ipts2[j];  

      if(dist<d1) //���������ƥ��ȵ�ǰ���ƥ�仹��
      {
        d2 = d1;
        d1 = dist;
        match = &ipts2[j];
      }
      else if(dist<d2) //���������ƥ��ȵ�ǰ�μ�ƥ�仹��
      {
        d2 = dist;
      }
    }

	//���d1:d2<0.65����Ϊ��һƥ���
    if(d1/d2 < 0.65) 
    { 
	  //�洢λ�õı仯
      ipts1[i].dx = match->x - ipts1[i].x; 
      ipts1[i].dy = match->y - ipts1[i].y;
      matches.push_back(std::make_pair(ipts1[i], *match));
    }
  }
}

//
//��Ӧ�ԣ������Ϊ��͸�ӱ任��ʹ�õ���CV_RANSAC������OpenCV 1.1�����ڴ����linux���а��в������
//
//-------------------------------------------------------

//���ƥ���֮��ĵ�Ӧ�Ծ��󣬲���src_corners�任��dst_corners
int translateCorners(IpPairVec &matches, const cv::Point src_corners[4], cv::Point dst_corners[4])
{
#ifndef LINUX
  double h[9];//���浥Ӧ�Ծ��������h
  CvMat _h = cvMat(3, 3, CV_64F, h);//��hת���ɾ���
  std::vector<CvPoint2D32f> pt1, pt2;
  CvMat _pt1, _pt2;
  
  int n = (int)matches.size();
  if( n < 4 ) return 0;

  //���������Ĵ�С
  pt1.resize(n);
  pt2.resize(n);

  //�����ĸ���
  for(int i = 0; i < n; i++ )
  {
    pt1[i] = cvPoint2D32f(matches[i].second.x, matches[i].second.y);
    pt2[i] = cvPoint2D32f(matches[i].first.x, matches[i].first.y);
  }
  _pt1 = cvMat(1, n, CV_32FC2, &pt1[0] );
  _pt2 = cvMat(1, n, CV_32FC2, &pt2[0] );

  //��������㼯֮��ĵ�Ӧ�Ա任����h
  if(!cvFindHomography(&_pt1, &_pt2, &_h, cv::RANSAC, 5))  //����Ҫ��opencv 1.1
    return 0;

  //����������õĵ�Ӧ�Ա任����h����src_corners�任��dst_corners
  for(int i = 0; i < 4; i++ )
  {
    double x = src_corners[i].x, y = src_corners[i].y;
    double Z = 1./(h[6]*x + h[7]*y + h[8]);
    double X = (h[0]*x + h[1]*y + h[2])*Z;
    double Y = (h[3]*x + h[4]*y + h[5])*Z;
    dst_corners[i] = cvPoint(cvRound(X), cvRound(Y));
  }
#endif
  return 1;
}


