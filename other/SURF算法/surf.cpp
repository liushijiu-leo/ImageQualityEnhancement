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

/***********************************************************
*  translated by Mr.Hu(Graduate Student of SWJTU)		   *
*  His E-mail is eleftheria@163.com						   *
************************************************************/

#include "utils.h"

#include "surf.h"

//-------------------------------------------------------
//! SURF priors (these need not be done at runtime)
const float pi = 3.14159f;

//7x7��С��2ά��˹�˲��ұ�sigma = 2.5����(0,0)�����Ͻǣ�(6,6)�����½�
const double gauss25 [7][7] = {
  0.02546481,	0.02350698,	0.01849125,	0.01239505,	0.00708017,	0.00344629,	0.00142946,
  0.02350698,	0.02169968,	0.01706957,	0.01144208,	0.00653582,	0.00318132,	0.00131956,
  0.01849125,	0.01706957,	0.01342740,	0.00900066,	0.00514126,	0.00250252,	0.00103800,
  0.01239505,	0.01144208,	0.00900066,	0.00603332,	0.00344629,	0.00167749,	0.00069579,
  0.00708017,	0.00653582,	0.00514126,	0.00344629,	0.00196855,	0.00095820,	0.00039744,
  0.00344629,	0.00318132,	0.00250252,	0.00167749,	0.00095820,	0.00046640,	0.00019346,
  0.00142946,	0.00131956,	0.00103800,	0.00069579,	0.00039744,	0.00019346,	0.00008024
};

//-------------------------------------------------------

//���캯��
Surf::Surf(IplImage *img, IpVec &ipts)
: ipts(ipts)
{
  this->img = img;
}

//-------------------------------------------------------

//��������������������
void Surf::getDescriptors(bool upright)
{
  //����Ƿ���������
  if (!ipts.size()) return;

  //�õ������Ĵ�С
  int ipts_size = (int)ipts.size();

  if (upright)
  {
	//U-SURFѭ��������ȡ������
    for (int i = 0; i < ipts_size; ++i)
    {
	  //���ñ�������Ipoint
      index = i;

	  //��ȡ��ֱ(������ת����)������
      getDescriptor(true);
    }
  }
  else
  {
	//ΪSURF-64ѭ�����䷽�򲢻�ȡ������
    for (int i = 0; i < ipts_size; ++i)
    {
      //���ñ�������Ipoint
      index = i;

      //���䷽�����ȡ��ת�����Ե�������
      getOrientation();
      getDescriptor(false);
    }
  }
}

//-------------------------------------------------------

//ΪIpoint���䷽��
void Surf::getOrientation()
{
  Ipoint *ipt = &ipts[index];
  float gauss = 0.f, scale = ipt->scale;
  const int s = fRound(scale), r = fRound(ipt->y), c = fRound(ipt->x);
  std::vector<float> resX(109), resY(109), Ang(109);
  const int id[] = {6,5,4,3,2,1,0,1,2,3,4,5,6};//���ұ�

  int idx = 0;
  //�ڰ뾶Ϊ6���߶�Բ�������ڼ���Ipoints��haar��Ӧ
  for(int i = -6; i <= 6; ++i) 
  {
    for(int j = -6; j <= 6; ++j) 
    {
      if(i*i + j*j < 36) 
      {
        gauss = static_cast<float>(gauss25[id[i+6]][id[j+6]]);  //����ʹ��abs()������id�����,��������
        resX[idx] = gauss * haarX(r+j*s, c+i*s, 4*s);
        resY[idx] = gauss * haarY(r+j*s, c+i*s, 4*s);
        Ang[idx] = getAngle(resX[idx], resY[idx]);
        ++idx;
      }
    }
  }

  //���������� 
  float sumX=0.f, sumY=0.f;
  float max=0.f, orientation = 0.f;
  float ang1=0.f, ang2=0.f;

  //��������Ϊ���ģ��Ž�Ϊpi/3�����λ�������
  for(ang1 = 0; ang1 < 2*pi;  ang1+=0.15f) {
    ang2 = ( ang1+pi/3.0f > 2*pi ? ang1-5.0f*pi/3.0f : ang1+pi/3.0f);
    sumX = sumY = 0.f; 
    for(unsigned int k = 0; k < Ang.size(); ++k) 
    {
	  //�����������x��Ƕ�
      const float & ang = Ang[k];

	  //ȷ���ؼ����Ƿ��ڴ�����
      if (ang1 < ang2 && ang1 < ang && ang < ang2) 
      {
        sumX+=resX[k];  
        sumY+=resY[k];
      } 
      else if (ang2 < ang1 && 
        ((ang > 0 && ang < ang2) || (ang > ang1 && ang < 2*pi) )) 
      {
        sumX+=resX[k];  
        sumY+=resY[k];
      }
    }

	//���������ʸ�������е�ʸ������,���䷽����Ϊ�µ�������
    if (sumX*sumX + sumY*sumY > max) 
    {
	  //�����ʸ���ķ���
      max = sumX*sumX + sumY*sumY;
      orientation = getAngle(sumX, sumY);
    }
  }

  //����������Ϊipt��һ������
  ipt->orientation = orientation;
}

//-------------------------------------------------------

//�õ���������������������£�Agrawal ECCV 08��
void Surf::getDescriptor(bool bUpright)
{
  int y, x, sample_x, sample_y, count=0;
  int i = 0, ix = 0, j = 0, jx = 0, xs = 0, ys = 0;
  float scale, *desc, dx, dy, mdx, mdy, co, si;
  float gauss_s1 = 0.f, gauss_s2 = 0.f;
  float rx = 0.f, ry = 0.f, rrx = 0.f, rry = 0.f, len = 0.f;
  float cx = -0.5f, cy = 0.f; //���Ӵ���Ϊ����ȡ4x4��˹Ȩ��

  Ipoint *ipt = &ipts[index];
  scale = ipt->scale;
  x = fRound(ipt->x);
  y = fRound(ipt->y);  
  desc = ipt->descriptor;

  if (bUpright)
  {
    co = 1;
    si = 0;
  }
  else
  {
    co = cos(ipt->orientation);
    si = sin(ipt->orientation);
  }

  i = -8;

  //�����������������
  while(i < 12)
  {
    j = -8;
    i = i-4;

    cx += 1.f;
    cy = -0.5f;

    while(j < 12) 
    {
      dx=dy=mdx=mdy=0.f;
      cy += 1.f;

      j = j - 4;

      ix = i + 5;
      jx = j + 5;

      xs = fRound(x + ( -jx*scale*si + ix*scale*co));
      ys = fRound(y + ( jx*scale*co + ix*scale*si));

      for (int k = i; k < i + 9; ++k) 
      {
        for (int l = j; l < j + 9; ++l) 
        {
		  //�õ���������ת�������
          sample_x = fRound(x + (-l*scale*si + k*scale*co));
          sample_y = fRound(y + ( l*scale*co + k*scale*si));

		  //�õ�x��y�ĸ�˹��Ȩ��Ӧֵ
          gauss_s1 = gaussian(xs-sample_x,ys-sample_y,2.5f*scale);
          rx = haarX(sample_y, sample_x, 2*fRound(scale));
          ry = haarY(sample_y, sample_x, 2*fRound(scale));

		  //�õ���ת��x��y�ĸ�˹��Ȩ��Ӧֵ
          rrx = gauss_s1*(-rx*si + ry*co);
          rry = gauss_s1*(rx*co + ry*si);

          dx += rrx;
          dy += rry;
          mdx += fabs(rrx);
          mdy += fabs(rry);

        }
      }

	  //��ֵ��ӵ�������������
      gauss_s2 = gaussian(cx-2.0f,cy-2.0f,1.5f);

      desc[count++] = dx*gauss_s2;
      desc[count++] = dy*gauss_s2;
      desc[count++] = mdx*gauss_s2;
      desc[count++] = mdy*gauss_s2;

      len += (dx*dx + dy*dy + mdx*mdx + mdy*mdy) * gauss_s2*gauss_s2;

      j += 9;
    }
    i += 9;
  }

  //ת��Ϊ��λ����
  len = sqrt(len);
  for(int i = 0; i < 64; ++i)
    desc[i] /= len;

}


//-------------------------------------------------------

//����(x,y)���ģ�����Ϊsig��2ά��˹ֵ
inline float Surf::gaussian(int x, int y, float sig)
{
  return (1.0f/(2.0f*pi*sig*sig)) * exp( -(x*x+y*y)/(2.0f*sig*sig));
}

//-------------------------------------------------------

//����(x,y)���ģ�����Ϊsig��2ά��˹ֵ
inline float Surf::gaussian(float x, float y, float sig)
{
  return 1.0f/(2.0f*pi*sig*sig) * exp( -(x*x+y*y)/(2.0f*sig*sig));
}

//-------------------------------------------------------

//����x�����HarrС����Ӧֵ
inline float Surf::haarX(int row, int column, int s)
{
  return BoxIntegral(img, row-s/2, column, s, s/2) 
    -1 * BoxIntegral(img, row-s/2, column-s/2, s, s/2);
}

//-------------------------------------------------------

//����y�����HarrС����Ӧֵ
inline float Surf::haarY(int row, int column, int s)
{
  return BoxIntegral(img, row, column-s/2, s/2, s) 
    -1 * BoxIntegral(img, row-s/2, column-s/2, s/2, s);
}

//-------------------------------------------------------

//�õ���(x,y)���������x�᷽��ĽǶ�
float Surf::getAngle(float X, float Y)
{
  if(X > 0 && Y >= 0)
    return atan(Y/X);

  if(X < 0 && Y >= 0)
    return pi - atan(-Y/X);

  if(X < 0 && Y < 0)
    return pi + atan(Y/X);

  if(X > 0 && Y < 0)
    return 2*pi - atan(-Y/X);

  return 0;
}