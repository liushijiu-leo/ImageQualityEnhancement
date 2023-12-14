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

#ifndef FASTHESSIAN_H
#define FASTHESSIAN_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "ipoint.h"

#include <vector>

class ResponseLayer;
static const int OCTAVES = 5;//����Ĭ������Ϊ5
static const int INTERVALS = 4;//����Ĭ������Ϊ4
static const float THRES = 0.0004f;//�ߵ���Ӧ��ֵĬ������Ϊ0.0004
static const int INIT_SAMPLE = 2;//�������Ϊ2


class FastHessian {
  
  public:
   
	//���캯��������ͼ��
    FastHessian(std::vector<Ipoint> &ipts, 
                const int octaves = OCTAVES, 
                const int intervals = INTERVALS, 
                const int init_sample = INIT_SAMPLE, 
                const float thres = THRES);

	//���캯������ͼ��
    FastHessian(IplImage *img, 
                std::vector<Ipoint> &ipts, 
                const int octaves = OCTAVES, 
                const int intervals = INTERVALS, 
                const int init_sample = INIT_SAMPLE, 
                const float thres = THRES);

    //��������
    ~FastHessian();

	//�������
    void saveParameters(const int octaves, 
                        const int intervals,
                        const int init_sample, 
                        const float thres);

	//���û������趨����ͼ����Դ
    void setIntImage(IplImage *img);

	//��ȡͼ������Բ�д�뵽����������
    void getIpoints();
    
  private:

    //---------------- ˽�к��� -----------------//

    //����DoH��Ӧ��
    void buildResponseMap();

	//����r���DoH��Ӧ
    void buildResponseLayer(ResponseLayer *r);

    //��3x3x3�ռ���Ѱ�Ҽ�ֵ��
    int isExtremum(int r, int c, ResponseLayer *t, ResponseLayer *m, ResponseLayer *b);    
    
	//��ֵ���� - �ı���Lowe��SIFT�㷨
    void interpolateExtremum(int r, int c, ResponseLayer *t, ResponseLayer *m, ResponseLayer *b);
    void interpolateStep(int r, int c, ResponseLayer *t, ResponseLayer *m, ResponseLayer *b,
                          double* xi, double* xr, double* xc );
    CvMat* deriv3D(int r, int c, ResponseLayer *t, ResponseLayer *m, ResponseLayer *b);
    CvMat* hessian3D(int r, int c, ResponseLayer *t, ResponseLayer *m, ResponseLayer *b);

    //---------------- ˽�б��� -----------------//

	//����ͼ��ָ�뼰������
    IplImage *img;
    int i_width, i_height;

	//�ⲿ�ο�������������Reference to vector of features passed from outside��
    std::vector<Ipoint> &ipts;

	//hessian����ʽ����Ӧֵ
    std::vector<ResponseLayer *> responseMap;

    //����
    int octaves;

    //ÿ�����
    int intervals;

    //! Initial sampling step for Ipoint detection
	//��ʼ�������ĳ������
    int init_sample;

	//�ߵ���Ӧ��ֵ
    float thresh;
};


#endif
