#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

#include "Superpixels.hpp"

using namespace std;
using namespace cv;

int main(int argc, char* argv[]){

    if(argc != 2){
        cout << "Usage: ./superpixels input.png" << endl;
        return 0;
    }

    Mat img = imread(argv[1]);
    imshow("Original", img);

    Superpixels sp(img);
    Mat superpixels = sp.getSuperpixelsImg();
    imshow("Superpixel boundaries", superpixels);
    
    waitKey(0);

    return 0;
}
