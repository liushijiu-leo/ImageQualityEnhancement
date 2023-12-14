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

#ifndef INTEGRAL_H
#define INTEGRAL_H

#include <algorithm>  // req'd for std::min/max

//δ��ֵ�ĺ�
#ifdef min
  #undef min
#endif

#ifdef max
  #undef max
#endif

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

//��������ͼ��img�Ļ���ͼ�񡣼���Դͼ����һ��32λ���㡣����32λ��������IplImage��ʽͼ��
IplImage *Integral(IplImage *img);



//���ӻ���
inline float BoxIntegral(IplImage *img, int row, int col, int rows, int cols) 
{
  float *data = (float *) img->imageData;
  int step = img->widthStep/sizeof(float);

  int r1 = std::min(row,          img->height) - 1;
  int c1 = std::min(col,          img->width)  - 1;
  int r2 = std::min(row + rows,   img->height) - 1;
  int c2 = std::min(col + cols,   img->width)  - 1;

  float A(0.0f), B(0.0f), C(0.0f), D(0.0f);
  if (r1 >= 0 && c1 >= 0) A = data[r1 * step + c1];
  if (r1 >= 0 && c2 >= 0) B = data[r1 * step + c2];
  if (r2 >= 0 && c1 >= 0) C = data[r2 * step + c1];
  if (r2 >= 0 && c2 >= 0) D = data[r2 * step + c2];

  return std::max(0.f, A - B - C + D);
}

#endif
