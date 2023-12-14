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

#include "ipoint.h"

#include <vector>
#include <time.h>
#include <stdlib.h>

//-----------------------------------------------------------
//K-means���ࣨunder development��
//�ɸ��ݵ��λ�������о���
//����K-means������IpVec���о���
//Planned improvements include clustering based on motion and descriptor components.
//-----------------------------------------------------------

class Kmeans {

public:

  //��������
  ~Kmeans() {};

  //���캯��
  Kmeans() {};

  //����
  void Run(IpVec *ipts, int clusters, bool init = false);

  //����Ҫ���������
  void SetIpoints(IpVec *ipts);

  //�����Ϊn��
  void InitRandomClusters(int n);

  //�������������
  bool AssignToClusters();

  //�����µľ�������
  void RepositionClusters();

  //����������֮��ľ���
  float Distance(Ipoint &ip1, Ipoint &ip2);

  //����������
  IpVec *ipts;

  //��������
  IpVec clusters;

};

//-------------------------------------------------------

void Kmeans::Run(IpVec *ipts, int clusters, bool init)
{
  if (!ipts->size()) return;

  SetIpoints(ipts);

  if (init) InitRandomClusters(clusters);
  
  while (AssignToClusters());
  {
    RepositionClusters();
  }
}

//-------------------------------------------------------

void Kmeans::SetIpoints(IpVec *ipts)
{
  this->ipts = ipts;
}

//-------------------------------------------------------

void Kmeans::InitRandomClusters(int n)
{
  //�����������
  clusters.clear();

  //�������������������
  srand((int)time(NULL));

  //��n�����������������Ϊn����ĳ�ʼ����
  for (int i = 0; i < n; ++i)
  {
    clusters.push_back(ipts->at(rand() % ipts->size()));
  }
}

//-------------------------------------------------------

bool Kmeans::AssignToClusters()
{
  bool Updated = false;

  // �������������㣬��������������������Ϊһ��
  for (unsigned int i = 0; i < ipts->size(); ++i)
  {
    float bestDist = FLT_MAX;
    int oldIndex = ipts->at(i).clusterIndex;

    for (unsigned int j = 0; j < clusters.size(); ++j)
    {
      float currentDist = Distance(ipts->at(i), clusters[j]);
      if (currentDist < bestDist)
      {
        bestDist = currentDist;
        ipts->at(i).clusterIndex = j;
      }
    }

    //�����Ƿ����˸ı�
    if (ipts->at(i).clusterIndex != oldIndex) Updated = true;
  }

  return Updated;
}

//-------------------------------------------------------

void Kmeans::RepositionClusters()
{
  float x, y, dx, dy, count;

  for (unsigned int i = 0; i < clusters.size(); ++i)
  {
    x = y = dx = dy = 0;
    count = 1;

    for (unsigned int j = 0; j < ipts->size(); ++j)
    {
      if (ipts->at(j).clusterIndex == i)
      {
        Ipoint ip = ipts->at(j);
        x += ip.x;
        y += ip.y;
        dx += ip.dx;
        dy += ip.dy;
        ++count;
      }
    }

    clusters[i].x = x/count;
    clusters[i].y = y/count;
    clusters[i].dx = dx/count;
    clusters[i].dy = dy/count;
  }
}

//-------------------------------------------------------

float Kmeans::Distance(Ipoint &ip1, Ipoint &ip2)
{
  return sqrt(pow(ip1.x - ip2.x, 2) 
            + pow(ip1.y - ip2.y, 2)
            /*+ pow(ip1.dx - ip2.dx, 2) 
            + pow(ip1.dy - ip2.dy, 2)*/);
}

//-------------------------------------------------------
