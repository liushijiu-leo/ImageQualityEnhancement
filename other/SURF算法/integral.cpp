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

#include "utils.h"

#include "integral.h"

//��������ͼ��img�Ļ���ͼ�񡣼���Դͼ����һ��32λ���㡣����32λ��������IplImage��ʽͼ��
IplImage *Integral(IplImage *source)
{
  //��Դͼ��ת��Ϊ32λ�����ͻҶ�ͼ��
  IplImage *img = getGray(source);
  IplImage *int_img = cvCreateImage(cvGetSize(img), IPL_DEPTH_32F, 1);

  //����������
  int height = img->height;
  int width = img->width;
  int step = img->widthStep/sizeof(float);
  float *data   = (float *) img->imageData;  
  float *i_data = (float *) int_img->imageData;  

  //�����ͼ��ͨ����������Ҫѭ��height*width-1�Ρ�
  //��һ����Ԫ�Ļ���
  float rs = 0.0f;
  for(int j=0; j<width; j++) 
  {
    rs += data[j]; 
    i_data[j] = rs;
  }

  //ʣ����Ԫ�Ļ���
  for(int i=1; i<height; ++i) 
  {
    rs = 0.0f;
    for(int j=0; j<width; ++j) 
    {
      rs += data[i*step+j]; 
      i_data[i*step+j] = rs + i_data[(i-1)*step+j];
    }
  }

  //�ͷŻҶ�ͼ��
  cvReleaseImage(&img);

  //���ػ���ͼ��
  return int_img;
}

