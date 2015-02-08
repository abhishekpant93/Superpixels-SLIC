#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

#include "Superpixels.hpp"

using namespace std;
using namespace cv;

int main(int argc, char* argv[]){

    if(argc != 2){
        cout << "Usage: ./superpixels <input_img>" << endl;
        return 0;
    }

    Mat img = imread(argv[1]);

    if(!img.data){
        cout << "Bad image ..." << endl;
        return 0;
    }
    
    imshow("Original", img);

    Superpixels sp(img);
    
    Mat boundaries = sp.viewSuperpixels();
    imshow("Superpixel boundaries", boundaries);

    Mat recolored = sp.colorSuperpixels();
    imshow("Average superpixel colors", recolored);
    
    waitKey(0);

    return 0;
}
