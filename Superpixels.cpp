#include "Superpixels.hpp"

const Mat Superpixels::sobel = (Mat_<float>(3,3) << -1/16., -2/16., -1/16., 0, 0, 0, 1/16., 2/16., 1/16.);

Superpixels::Superpixels(Mat& img, float m, float S){
    this->img = img.clone();
    this->m = m;
    if(S == USE_DEFAULT_S){
        this->nx = 15; // cols
        this->ny = 15; // rows
        this->dx = img.cols / float(nx); //steps
        this->dy = img.rows / float(ny);
        this->S = (dx + dy + 1)/2; // default window size
    }
    else
        this->S = S;
        
    calculateSuperpixels();
}

Mat Superpixels::viewSuperpixels(){    

	// Draw boundaries on original image
	vector<Mat> rgb(3);
	split(this->img_f, rgb);
	for (int i = 0; i < 3; i++){
        rgb[i] = rgb[i].mul(this->show);
    }
    
    Mat output = this->img_f.clone();
	merge(rgb, output);

    output = 255 * output;
    output.convertTo(output, CV_8UC3);
    
    return output;
}

Mat Superpixels::colorSuperpixels(){
    
    int n = nx * ny;
    vector<Vec3b> avg_colors(n);
    vector<int> num_pixels(n);
    
    vector<long> b(n), g(n), r(n);
    
    for(int y = 0; y < (int) labels.rows; ++y){
        for(int x = 0; x < (int) labels.cols; ++x){

            Vec3b pix = img.at<Vec3b>(y, x);
            int lbl = labels.at<int>(y, x);
            
            b[lbl] += (int) pix[0];
            g[lbl] += (int) pix[1];
            r[lbl] += (int) pix[2];
            
            ++num_pixels[lbl];
        }
    }

    for(int i = 0; i < n; ++i){
        int num = num_pixels[i];
        avg_colors[i] = Vec3b(b[i] / num, g[i] / num, r[i] / num);
    }
    
    Mat output = this->img.clone();
    for(int y = 0; y < (int) output.rows; ++y){
        for(int x = 0; x < (int) output.cols; ++x){
            int lbl = labels.at<int>(y, x);
            if(num_pixels[lbl])
                output.at<Vec3b>(y, x) = avg_colors[lbl];
        }
    }
    
    return output;
}

vector<Point> Superpixels::getCenters(){
    return centers;
}

Mat Superpixels::getLabels(){
    return labels;
}

// map<int, Point> Superpixels::getCentersMap(){

//     map<int, Point> out;
//     for(int i = 0; i < (int) centers.size(); ++i){
//         Point p = centers[i];
//         int lbl = labels.at<int>(p);
//         out[lbl] = p;
//     }
//     return out;
// }

void Superpixels::calculateSuperpixels(){

    // Scale img to [0,1]
    this->img.convertTo(this->img_f, CV_32F, 1/255.);

    // Convert to l-a-b colorspace
	cvtColor(this->img_f, this->img_lab, CV_BGR2Lab);

    int n = nx * ny;
    int w = img.cols;
    int h = img.rows;
    
	for (int i = 0; i < ny; i++) {
		for (int j = 0; j < nx; j++) {
			this->centers.push_back( Point2f(j*dx+dx/2, i*dy+dy/2));
		}
	}

	// Initialize labels and distance maps
	vector<int> label_vec(n);
	for (int i = 0; i < n; i++)
        label_vec[i] = i*255*255/n;

	Mat labels = -1*Mat::ones(this->img_lab.size(), CV_32S);
	Mat dists = -1*Mat::ones(this->img_lab.size(), CV_32F);
	Mat window;
	Point2i p1, p2;
	Vec3f p1_lab, p2_lab;

	// Iterate 10 times. In practice more than enough to converge
	for (int i = 0; i < 10; i++) {
		// For each center...
		for (int c = 0; c < n; c++)
            {
                int label = label_vec[c];
                p1 = centers[c];
                int xmin = max<int>(p1.x - S, 0);
                int ymin = max<int>(p1.y - S, 0);
                int xmax = min<int>(p1.x + S, w - 1);
                int ymax = min<int>(p1.y + S, h - 1);

                // Search in a window around the center
                window = this->img_f(Range(ymin, ymax), Range(xmin, xmax));
			
                // Reassign pixels to nearest center
                for (int i = 0; i < window.rows; i++) {
                    for (int j = 0; j < window.cols; j++) {
                        p2 = Point2i(xmin + j, ymin + i);
                        float d = dist(p1, p2);
                        float last_d = dists.at<float>(p2);
                        if (d < last_d || last_d == -1) {
                            dists.at<float>(p2) = d;
                            labels.at<int>(p2) = label;
                        }
                    }
                }
            }
	}

    // Store the labels for each pixel
    this->labels = labels.clone();
    this->labels =  n * this->labels / (255 * 255);

    // Calculate superpixel boundaries
	labels.convertTo(labels, CV_32F);

	Mat gx, gy, grad;
	filter2D(labels, gx, -1, sobel);
	filter2D(labels, gy, -1, sobel.t());
	magnitude(gx, gy, grad);
	grad = (grad > 1e-4)/255;
    Mat show = 1 - grad;
    show.convertTo(show, CV_32F);
        
    // Store the result
    this->show = show.clone();
}

float Superpixels::dist(Point p1, Point p2){
    Vec3f p1_lab = this->img_lab.at<Vec3f>(p1);
    Vec3f p2_lab = this->img_lab.at<Vec3f>(p2);
    
    float dl = p1_lab[0] - p2_lab[0];
	float da = p1_lab[1] - p2_lab[1];
	float db = p1_lab[2] - p2_lab[2];

	float d_lab = sqrtf(dl*dl + da*da + db*db);

	float dx = p1.x - p2.x;
	float dy = p1.y - p2.y;

	float d_xy = sqrtf(dx*dx + dy*dy);

	return d_lab + m/S * d_xy;
}
