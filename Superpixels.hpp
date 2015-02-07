#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

#include <math.h>
#include <iostream>
#include <string>
#include <algorithm>

#define DEFAULT_M 20
#define USE_DEFAULT_S -1

using namespace std;
using namespace cv;

class Superpixels{

public:
    Superpixels(Mat& img, float m = DEFAULT_M, float S = USE_DEFAULT_S); // calculates the superpixel boundaries on construction
    
    Mat getSuperpixelsImg(); // returns image displaying superpixel boundaries
    vector<vector<Point> > getSuperpixelContours(); // returns contours
    Mat recolor(); // recolors image with average color in each cluster
    
protected:
    Mat img; // original image
    Mat img_f; // scaled to [0,1]
    Mat img_lab; // converted to LAB colorspace
    Mat show;
    float m; // compactness parameter
    float S; // window size

    vector<Point> centers; // superpixel centers
    
    void calculateSuperpixels();
    float dist(Point p1, Point p2);

    const static Mat sobel;
};
