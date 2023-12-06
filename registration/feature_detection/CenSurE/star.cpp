#include "star.hpp"

MyStarTest::MyStarTest() {
}

MyStarTest::~MyStarTest() {
}

void MyStarTest::computeIntegralImages(Mat& matI, Mat& matS, Mat& matT, Mat& _FT) {
    int x, y, rows = matI.rows, cols = matI.cols;

    matS.create(rows + 1, cols + 1, CV_32S);
    matT.create(rows + 1, cols + 1, CV_32S);
    _FT.create(rows + 1, cols + 1, CV_32S);

    uchar* I = matI.ptr<uchar>();

    int *S = matS.ptr<int>();
    int *T = matT.ptr<int>();
    int *FT = _FT.ptr<int>();

    int istep = (int)(matI.step/matI.elemSize());
    int step = (int)(matS.step/matS.elemSize());

    for( x = 0; x <= cols; x++ )
        S[x] = T[x] = FT[x] = 0;

    S += step; T += step; FT += step;
    S[0] = T[0] = 0;
    FT[0] = I[0];
    for(x = 1; x < cols; x++) {
        S[x] = S[x-1] + I[x-1];
        T[x] = I[x-1];
        FT[x] = I[x] + I[x-1];
    }
    S[cols] = S[cols-1] + I[cols-1];
    T[cols] = FT[cols] = I[cols-1];

    for(y = 2; y <= rows; y++) {
        I += istep, S += step, T += step, FT += step;

        S[0] = S[-step]; S[1] = S[-step+1] + I[0];
        T[0] = T[-step + 1];
        T[1] = FT[0] = T[-step + 2] + I[-istep] + I[0];
        FT[1] = FT[-step + 2] + I[-istep] + I[1] + I[0];

        for(x = 2; x < cols; x++) {
            S[x] = S[x - 1] + S[-step + x] - S[-step + x - 1] + I[x - 1];
            T[x] = T[-step + x - 1] + T[-step + x + 1] - T[-step*2 + x] + I[-istep + x - 1] + I[x - 1];
            FT[x] = FT[-step + x - 1] + FT[-step + x + 1] - FT[-step*2 + x] + I[x] + I[x-1];
        }

        S[cols] = S[cols - 1] + S[-step + cols] - S[-step + cols - 1] + I[cols - 1];
        T[cols] = FT[cols] = T[-step + cols - 1] + I[-istep + cols - 1] + I[cols - 1];
    }
}

int MyStarTest::StarDetectorComputeResponses(Mat img, Mat &responses, Mat &sizes, int maxSize) {
    int MAX_PATTERN = 17;
    int sizes0[] = {1, 2, 3, 4, 6, 8, 11, 12, 16, 22, 23, 32, 45, 46, 64, 90, 128, -1};
    int pairs[12][2] = {{1, 0}, {3, 1}, {4, 2}, {5, 3}, {7, 4}, {8, 5}, {9, 6},
                        {11, 8}, {13, 10}, {14, 11}, {15, 12}, {16, 14}};
    int MAX_PAIR = sizeof(pairs)/sizeof(pairs[0]);
    float invSizes[MAX_PATTERN][2];
    int sizes1[MAX_PATTERN];

    struct StarFeature {
        int area;
        int* p[8];
    };

    StarFeature f[MAX_PATTERN];

    int y, rows = img.rows, cols = img.cols;
    responses = Mat::zeros(img.size(), CV_32F);
    sizes = Mat::zeros(img.size(), CV_16S);

    int npatterns=0;
    while(npatterns < MAX_PAIR && !
          (sizes0[pairs[npatterns][0]] >= maxSize
           || sizes0[pairs[npatterns+1][0]] + sizes0[pairs[npatterns+1][0]]/2 >= min(rows, cols))) {
        ++npatterns;
    }

    if (npatterns-1 < MAX_PAIR) {
        ++npatterns;
    }
    int maxIdx = pairs[npatterns-1][0];

    Mat sum, tilted, flatTilted;
    computeIntegralImages(img, sum, tilted, flatTilted);

    int step = (int)(sum.step/sum.elemSize());

    for(int i = 0; i <= maxIdx; i++ ) {
        int ur_size = sizes0[i], t_size = sizes0[i] + sizes0[i]/2;
        int ur_area = (2*ur_size + 1)*(2*ur_size + 1);
        int t_area = t_size*t_size + (t_size + 1)*(t_size + 1);

        f[i].p[0] = sum.ptr<int>() + (ur_size + 1)*step + ur_size + 1;
        f[i].p[1] = sum.ptr<int>() - ur_size*step + ur_size + 1;
        f[i].p[2] = sum.ptr<int>() + (ur_size + 1)*step - ur_size;
        f[i].p[3] = sum.ptr<int>() - ur_size*step - ur_size;

        f[i].p[4] = tilted.ptr<int>() + (t_size + 1)*step + 1;
        f[i].p[5] = flatTilted.ptr<int>() - t_size;
        f[i].p[6] = flatTilted.ptr<int>() + t_size + 1;
        f[i].p[7] = tilted.ptr<int>() - t_size*step + 1;

        f[i].area = ur_area + t_area;
        sizes1[i] = sizes0[i];
    }

    // negate end points of the size range
    // for a faster rejection of very small or very large features in non-maxima suppression.
    sizes1[0] = -sizes1[0];
    sizes1[1] = -sizes1[1];
    sizes1[maxIdx] = -sizes1[maxIdx];
    int border = sizes0[maxIdx] + sizes0[maxIdx]/2;

    for(int i = 0; i < npatterns; i++ )
    {
        int innerArea = f[pairs[i][1]].area;
        int outerArea = f[pairs[i][0]].area;
        outerArea -= innerArea;

        invSizes[i][0] = 1.f/outerArea;
        invSizes[i][1] = 1.f/innerArea;
    }

    for( y = 0; y < border; y++ )
    {
        float* r_ptr = responses.ptr<float>(y);
        float* r_ptr2 = responses.ptr<float>(rows - 1 - y);
        short* s_ptr = sizes.ptr<short>(y);
        short* s_ptr2 = sizes.ptr<short>(rows - 1 - y);

        memset( r_ptr, 0, cols*sizeof(r_ptr[0]));
        memset( r_ptr2, 0, cols*sizeof(r_ptr2[0]));
        memset( s_ptr, 0, cols*sizeof(s_ptr[0]));
        memset( s_ptr2, 0, cols*sizeof(s_ptr2[0]));
    }

    for( y = border; y < rows - border; y++ )
    {
        int x = border;
        float* r_ptr = responses.ptr<float>(y);
        short* s_ptr = sizes.ptr<short>(y);

        memset( r_ptr, 0, border*sizeof(r_ptr[0]));
        memset( s_ptr, 0, border*sizeof(s_ptr[0]));
        memset( r_ptr + cols - border, 0, border*sizeof(r_ptr[0]));
        memset( s_ptr + cols - border, 0, border*sizeof(s_ptr[0]));

        for( ; x < cols - border; x++ )
        {
            int ofs = y*step + x;
            int vals[MAX_PATTERN];
            float bestResponse = 0;
            int bestSize = 0;

            for(int i = 0; i <= maxIdx; i++ )
            {
                const int** p = (const int**)f[i].p;
                vals[i] = (int)(p[0][ofs] - p[1][ofs] - p[2][ofs] + p[3][ofs] +
                    p[4][ofs] - p[5][ofs] - p[6][ofs] + p[7][ofs]);
            }
            for(int i = 0; i < npatterns; i++ )
            {
                int inner_sum = vals[pairs[i][1]];
                int outer_sum = vals[pairs[i][0]] - inner_sum;
                float response = inner_sum*invSizes[i][1] - outer_sum*invSizes[i][0];
                if( fabs(response) > fabs(bestResponse) )
                {
                    bestResponse = response;
                    bestSize = sizes1[pairs[i][0]];
                }
            }

            r_ptr[x] = bestResponse;
            s_ptr[x] = (short)bestSize;
        }
    }

    return border;
}

bool MyStarTest::StarDetectorSuppressLines(const Mat& responses, const Mat& sizes, Point pt,
                                       int lineThresholdProjected, int lineThresholdBinarized) {
    const float* r_ptr = responses.ptr<float>();
    int rstep = (int)(responses.step/sizeof(r_ptr[0]));
    const short* s_ptr = sizes.ptr<short>();
    int sstep = (int)(sizes.step/sizeof(s_ptr[0]));
    int sz = s_ptr[pt.y*sstep + pt.x];
    int x, y, delta = sz/4, radius = delta*4;
    float Lxx = 0, Lyy = 0, Lxy = 0;
    int Lxxb = 0, Lyyb = 0, Lxyb = 0;

    for( y = pt.y - radius; y <= pt.y + radius; y += delta )
        for( x = pt.x - radius; x <= pt.x + radius; x += delta )
        {
            float Lx = r_ptr[y*rstep + x + 1] - r_ptr[y*rstep + x - 1];
            float Ly = r_ptr[(y+1)*rstep + x] - r_ptr[(y-1)*rstep + x];
            Lxx += Lx*Lx; Lyy += Ly*Ly; Lxy += Lx*Ly;
        }

    if( (Lxx + Lyy)*(Lxx + Lyy) >= lineThresholdProjected*(Lxx*Lyy - Lxy*Lxy) )
        return true;

    for( y = pt.y - radius; y <= pt.y + radius; y += delta )
        for( x = pt.x - radius; x <= pt.x + radius; x += delta )
        {
            int Lxb = (s_ptr[y*sstep + x + 1] == sz) - (s_ptr[y*sstep + x - 1] == sz);
            int Lyb = (s_ptr[(y+1)*sstep + x] == sz) - (s_ptr[(y-1)*sstep + x] == sz);
            Lxxb += Lxb * Lxb; Lyyb += Lyb * Lyb; Lxyb += Lxb * Lyb;
        }

    if( (Lxxb + Lyyb)*(Lxxb + Lyyb) >= lineThresholdBinarized*(Lxxb*Lyyb - Lxyb*Lxyb) )
        return true;

    return false;
}

void MyStarTest::StarDetectorSuppressNonmax(Mat& responses, Mat& sizes,
        vector<KeyPoint>& keypoints, int border,
        int responseThreshold, int lineThresholdProjected,
        int lineThresholdBinarized, int suppressNonmaxSize) {

    int x, y, x1, y1, delta = suppressNonmaxSize/2;
    int rows = responses.rows, cols = responses.cols;
    const float* r_ptr = responses.ptr<float>();
    int rstep = (int)(responses.step/sizeof(r_ptr[0]));
    const short* s_ptr = sizes.ptr<short>();
    int sstep = (int)(sizes.step/sizeof(s_ptr[0]));
    short featureSize = 0;

    for( y = border; y < rows - border; y += delta+1 )
        for( x = border; x < cols - border; x += delta+1) {
            float maxResponse = (float)responseThreshold;
            float minResponse = (float)-responseThreshold;
            Point maxPt(-1, -1), minPt(-1, -1);
            int tileEndY = MIN(y + delta, rows - border - 1);
            int tileEndX = MIN(x + delta, cols - border - 1);

            for( y1 = y; y1 <= tileEndY; y1++ )
                for( x1 = x; x1 <= tileEndX; x1++ )
                {
                    float val = r_ptr[y1*rstep + x1];
                    if( maxResponse < val )
                    {
                        maxResponse = val;
                        maxPt = Point(x1, y1);
                    }
                    else if( minResponse > val )
                    {
                        minResponse = val;
                        minPt = Point(x1, y1);
                    }
                }

            if( maxPt.x >= 0 )
            {
                for( y1 = maxPt.y - delta; y1 <= maxPt.y + delta; y1++ )
                    for( x1 = maxPt.x - delta; x1 <= maxPt.x + delta; x1++ )
                    {
                        float val = r_ptr[y1*rstep + x1];
                        if( val >= maxResponse && (y1 != maxPt.y || x1 != maxPt.x))
                            goto skip_max;
                    }

                if( (featureSize = s_ptr[maxPt.y*sstep + maxPt.x]) >= 4 &&
                        !StarDetectorSuppressLines( responses, sizes, maxPt, lineThresholdProjected,
                            lineThresholdBinarized ))
                {
                    KeyPoint kpt((float)maxPt.x, (float)maxPt.y, featureSize, -1, maxResponse);
                    keypoints.push_back(kpt);
                }
            }
skip_max:
            if( minPt.x >= 0 )
            {
                for( y1 = minPt.y - delta; y1 <= minPt.y + delta; y1++ )
                    for( x1 = minPt.x - delta; x1 <= minPt.x + delta; x1++ )
                    {
                        float val = r_ptr[y1*rstep + x1];
                        if( val <= minResponse && (y1 != minPt.y || x1 != minPt.x))
                            goto skip_min;
                    }

                if( (featureSize = s_ptr[minPt.y*sstep + minPt.x]) >= 4 &&
                        !StarDetectorSuppressLines( responses, sizes, minPt,
                            lineThresholdProjected, lineThresholdBinarized))
                {
                    KeyPoint kpt((float)minPt.x, (float)minPt.y, featureSize, -1, maxResponse);
                    keypoints.push_back(kpt);
                }
            }
skip_min:
            ;
        }
}

Mat MyStarTest::CornersShow(Mat src, vector<KeyPoint> corners) {
    Mat dst = src.clone();

    for(int i=0; i<corners.size(); i++) {
        circle(dst, corners[i].pt, 1, Scalar(255, 0, 0), -1);  // 画半径为1的圆(画点）
    }

    return dst;
}

Mat MyStarTest::run(Mat src) {
    int maxSize = 45;
    int responseThreshold = 30;
    int lineThresholdProjected = 10;
    int lineThresholdBinarized = 8;
    int suppressNonmaxSize = 5;

    Mat responses;
    Mat sizes;

    int border = StarDetectorComputeResponses(src, responses, sizes, maxSize);
    cout << "border:" << border << endl;

    vector<KeyPoint> keypoints;
    if(border >= 0)
        StarDetectorSuppressNonmax(responses, sizes, keypoints, border,
                responseThreshold, lineThresholdProjected,
                lineThresholdBinarized, suppressNonmaxSize);

    cout << "keypoints:" << keypoints.size() << endl;

    Mat dst = CornersShow(src, keypoints);

    return dst;
}
