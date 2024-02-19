#include <iostream>

#include<opencv2/opencv.hpp>
#include<opencv2/opencv_modules.hpp>

using namespace std;
using namespace cv;

int main(){

    //Path of image folder
    string PathToFolder = "../PATH_TO_IMAGES/";

    //Image Name
    string PathToImage = PathToFolder+"PATH_TO_IMG";

    // Read image.
    Mat image = imread(PathToImage);

    // If image is not there, flag an error.
    if (image.empty()) {
        cerr << "Error: Invalid Image URL" << endl;
        return -1;
    }

    // If face classifier file is not present, flag an error.
    CascadeClassifier face_cascade;
    if (!face_cascade.load("../Face_Blur/haarcascade_frontalface_default.xml")) { // This is apart of OpenCV, and is open source.
        cerr << "Error: Invalid cascade classifier" << endl;
    }

    // Detecting faces in the image.
    std::vector<Rect> faces;
    face_cascade.detectMultiScale(image, faces, 1.1, 3, 0, Size(200, 200));

    // Blur the faces in the image
    for (const Rect& face : faces) {
        Mat faceROI = image(face);
        GaussianBlur(faceROI, faceROI, Size(99, 99), 198);
    }

    // Show blurred faces in image.
    namedWindow("Blurred Faces Image", WINDOW_NORMAL);
    imshow("Blurred Faces Image", image);

    waitKey(0);

    return 0;
}
