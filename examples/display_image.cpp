#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;

int main() {
    Mat image;
    image = imread( "../data/im.jpeg", 1 );

    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }

    imshow("Display Image", image);

    waitKey(0);

    return 0;
}
