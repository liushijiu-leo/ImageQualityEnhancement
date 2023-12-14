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

#ifndef SURF_H
#define SURF_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "ipoint.h"
#include "integral.h"

#include <vector>

class Surf {
  
  public:
    
	//��׼���캯����img�ǻ���ͼ��
    Surf(IplImage *img, std::vector<Ipoint> &ipts);

	//���ṩ��������������������
    void getDescriptors(bool bUpright = false);
  
  private:
    
    //---------------- ˽�к��� -----------------//

	//Ϊ��ǰIpoint���䷽��
    void getOrientation();
    
	//��ȡ�������������ģ�Agrawal ECCV 08��
    void getDescriptor(bool bUpright = false);

	//����(x,y)������Ϊsig��2ά��˹ֵ
    inline float gaussian(int x, int y, float sig);
    inline float gaussian(float x, float y, float sig);

	//����x��y�����HaarС����Ӧֵ
    inline float haarX(int row, int column, int size);
    inline float haarY(int row, int column, int size);

	//��ȡ��(x,y)�������x�᷽��ĽǶ�
    float getAngle(float X, float Y);


    //---------------- ˽�б��� -----------------//

	//�Ѽ�⵽Ipoints�Ļ���ͼ��
    IplImage *img;

    //Ipoints����
    IpVec &ipts;

	//��ǰIpoint����������ֵ
    int index;
};


#endif
