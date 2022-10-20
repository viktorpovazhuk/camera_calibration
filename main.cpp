#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::Mat image;
    image = cv::imread( "../im.jpeg", 1 );

    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }

    cv::imshow("Display Image", image);

    cv::waitKey(0);

    return 0;
}
