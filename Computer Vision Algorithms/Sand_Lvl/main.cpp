#include <iostream>
#include <dirent.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

const string PathToFolder = "../YOUR_IMAGES/Images/";

Mat Test_Mod_Coords(Mat main_img, vector<Point2f> DistortedCroppedPoints, Mat cropped_image) { // Testing if Homography points are correct.
    while (waitKey(1) != 'z') {
        namedWindow("Image_Cropped", WINDOW_NORMAL);
        imshow("Image_Cropped", cropped_image);
    }

    GaussianBlur(cropped_image, cropped_image, Size(15, 15), 0);

    Mat matched_scene;
    matchTemplate(main_img, cropped_image, matched_scene, TM_SQDIFF_NORMED);

    Point min_Loc;
    minMaxLoc(matched_scene, 0, 0, &min_Loc, 0);

    Size s_cropped_img = cropped_image.size();

    Mat Matched_Scene_img = main_img(Rect(min_Loc.x, min_Loc.y, s_cropped_img.width, s_cropped_img.height));
    rectangle(main_img, Rect(min_Loc.x, min_Loc.y, s_cropped_img.width, s_cropped_img.height), Scalar(0, 255, 0), 2);

    vector<Point> distortedPoints;
    distortedPoints.push_back(Point(min_Loc.x + DistortedCroppedPoints[0].x, min_Loc.y + DistortedCroppedPoints[0].y));
    distortedPoints.push_back(Point(min_Loc.x + DistortedCroppedPoints[1].x, min_Loc.y + DistortedCroppedPoints[1].y));
    distortedPoints.push_back(Point(min_Loc.x + DistortedCroppedPoints[2].x, min_Loc.y + DistortedCroppedPoints[2].y));
    distortedPoints.push_back(Point(min_Loc.x + DistortedCroppedPoints[3].x, min_Loc.y + DistortedCroppedPoints[3].y));

    vector<Point2f> undistortedPoints;
    undistortedPoints.push_back(Point2f(distortedPoints[0].x, distortedPoints[0].y));
    undistortedPoints.push_back(Point2f(distortedPoints[0].x, distortedPoints[1].y));
    undistortedPoints.push_back(Point2f(distortedPoints[2].x, distortedPoints[1].y));
    undistortedPoints.push_back(Point2f(distortedPoints[2].x, distortedPoints[0].y));

    vector<vector<Point>> contours;
    contours.push_back(distortedPoints);

    polylines(main_img, contours, true, Scalar(0, 0, 255), 2);
    rectangle(main_img, Rect(undistortedPoints[1].x, undistortedPoints[1].y,
            (distortedPoints[2].x - distortedPoints[0].x),
            (undistortedPoints[0].y - undistortedPoints[1].y)), Scalar(255, 0, 0), 2);

    return main_img;
}

vector<String> GetImageNames(string FolderPath) {
    vector<String> TmpStringVec;

    DIR* dir;
    struct dirent* ent;

    const string& ADDPathToFolder = FolderPath;
    if ((dir = opendir(ADDPathToFolder.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            if (isdigit(ent->d_name[0])) {
                TmpStringVec.push_back(FolderPath+ent->d_name);
            } else if (ent->d_name[ent->d_namlen-1] == 'g'){
                cerr << "Image " << ent->d_name << " is not formatted correctly to have date at start of filename." << endl;
            }
        }
        closedir(dir);
    } else {
        cerr << "Error opening directory" << endl;
    }
    return TmpStringVec;
}

void GetCoordsHFromCropped(String Beach, vector<Point2f>& DistortedPoints, Mat& Cropped_Img) {

    // Coords go like this:
    //   2 3
    //   1 4

    if (Beach == "Whitmore") {
        // Distorted Points related to cropped Photo.
        DistortedPoints.push_back(Point2f(99, 383));
        DistortedPoints.push_back(Point2f(93, 15));
        DistortedPoints.push_back(Point2f(237, 179));
        DistortedPoints.push_back(Point2f(238, 354));

        // Getting Cropped Image:
        Cropped_Img = imread(PathToFolder+"PATH_TO_YOUR_IMG");
    } else if (Beach == "East") {
        // Distorted Points related to cropped Photo.
        DistortedPoints.push_back(Point2f(495, 112));
        DistortedPoints.push_back(Point2f(495, 32));
        DistortedPoints.push_back(Point2f(894, 26));
        DistortedPoints.push_back(Point2f(894, 93));

        // Getting Cropped Image:
        Cropped_Img = imread(PathToFolder+"PATH_TO_YOUR_IMG");
    }

    if (Cropped_Img.empty()) {
        cerr << "Error: Cropped Image not accessible" << endl;
    }
}

Mat Calibrating_Homography(Mat main_img, vector<Point2f> DistortedCroppedPoints, Mat cropped_image, Mat& H_M, Point& min_Loc) {
    Mat matched_scene;
    matchTemplate(main_img, cropped_image, matched_scene, TM_SQDIFF_NORMED);

    minMaxLoc(matched_scene, 0, 0, &min_Loc, 0);

    Size s_cropped_img = cropped_image.size();
    Size s_main_img = main_img.size();

    Mat Matched_Scene_img = main_img(Rect(min_Loc.x, min_Loc.y, s_cropped_img.width, s_cropped_img.height));

    vector<Point2f> distortedPoints;
    distortedPoints.push_back(Point2f(min_Loc.x + DistortedCroppedPoints[0].x, min_Loc.y + DistortedCroppedPoints[0].y));
    distortedPoints.push_back(Point2f(min_Loc.x + DistortedCroppedPoints[1].x, min_Loc.y + DistortedCroppedPoints[1].y));
    distortedPoints.push_back(Point2f(min_Loc.x + DistortedCroppedPoints[2].x, min_Loc.y + DistortedCroppedPoints[2].y));
    distortedPoints.push_back(Point2f(min_Loc.x + DistortedCroppedPoints[3].x, min_Loc.y + DistortedCroppedPoints[3].y));

    vector<Point2f> undistortedPoints;
    undistortedPoints.push_back(Point2f(distortedPoints[0].x, distortedPoints[0].y));
    undistortedPoints.push_back(Point2f(distortedPoints[0].x, distortedPoints[1].y));
    undistortedPoints.push_back(Point2f(distortedPoints[2].x, distortedPoints[1].y));
    undistortedPoints.push_back(Point2f(distortedPoints[2].x, distortedPoints[0].y));

    H_M = findHomography(distortedPoints, undistortedPoints);

    Mat warpedImage;
    warpPerspective(main_img, warpedImage, H_M, s_main_img);

    return warpedImage;
}

void Detecting_Contours_H(Mat main_img, Mat H_main_img, Mat H, Mat Cropped_image, Point Location, vector<Point>& F_contour) {
    Mat gray;
    cvtColor(H_main_img, gray, COLOR_BGR2GRAY);

    GaussianBlur(gray, gray, Size(35, 35), 0);

    double thresh = 100, maxValue = 255;
    threshold(gray, gray, thresh, maxValue, THRESH_BINARY);

    Canny(gray, gray, 0, 255);

    vector<vector<Point>> contours;
    findContours(gray, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    Size s_cropped = Cropped_image.size();

    // removing unnecessary vectors from the list of contour vectors.
    size_t min_size = 1000;
    auto rm_unnecessary = [min_size](const vector<Point>& vec) {
        return vec.size() < min_size;
    };
    contours.erase(remove_if(contours.begin(), contours.end(), rm_unnecessary), contours.end());

    vector<Point2f> warpedPoints;
    vector<Point2f> originalPoints = {Location, Point2f((Location.x + s_cropped.width), (Location.y + s_cropped.height))};

    perspectiveTransform(originalPoints, warpedPoints, H);

    vector<Point> intWarpedPoints;
    for (const auto& warpedPoint : warpedPoints) {
        intWarpedPoints.push_back(Point(static_cast<int>(warpedPoint.x), static_cast<int>(warpedPoint.y)));
    }

    // checking if points within the field of the cropped photo.
    auto filter_P = [intWarpedPoints](const Point& point) {
        return point.y > intWarpedPoints[1].y;
    };

    for (auto& Points : contours) {
        Points.erase(remove_if(Points.begin(), Points.end(), filter_P), Points.end());
    }

    contours.erase(remove_if(contours.begin(), contours.end(),
                             [](const vector<Point>& contour) {
        return contour.empty();
    }), contours.end());

    vector<Point2f> Final_Line;
    for (const auto& contour : contours) {
        size_t s_contour = contour.size();

        if (s_contour > 500) {
            drawContours(H_main_img, vector<vector<Point>>{contour}, 0, Scalar(0, 255, 0), 2);

            for (const auto& point : contour) {
                Final_Line.push_back(Point2f(point.x, point.y));
            }
        }
    }

    vector<Point2f> Unwarped_Final_Line;
    perspectiveTransform(Final_Line, Unwarped_Final_Line, H.inv());

    vector<Point> intUnwarpedFinalLine;
    for (const auto& unwarpedPoint : Unwarped_Final_Line) {
        if ((unwarpedPoint.x >= 1000) && (unwarpedPoint.x <= 1250)) {
            if ((unwarpedPoint.y >= 75) && (unwarpedPoint.y <= 150)) {
                intUnwarpedFinalLine.push_back(Point(static_cast<int>(unwarpedPoint.x), static_cast<int>(unwarpedPoint.y)));
            }
        }
    }

    vector<vector<Point>> vector_unwarped;
    vector_unwarped.push_back(intUnwarpedFinalLine);

    drawContours(main_img, vector_unwarped, 0, Scalar(0, 255, 0), 2);
    F_contour = intUnwarpedFinalLine;
}

void drawing_Lines_East(Mat cropped_img, vector<vector<Point>> Contours) {
    Size s_cropped = cropped_img.size();
    int width = (s_cropped.width + 500), height = (s_cropped.height + 100);
    Mat contour_image = Mat::ones(height, width, CV_8UC3);

    line(contour_image, Point(0, 36), Point(width, 36), Scalar(255, 0, 0), 2);
    line(contour_image, Point(0, 135), Point(width, 135), Scalar(255, 0, 0), 2);

    int b = 0, g = 0, r = 0;
    for (size_t i = 0; i < Contours.size(); i++) {
        g += 75;
        if (g == 225) {
            r += 75;
            g = 0;

            if (r == 225) {
                r = 0;
                b += 75;

                if (b == 225) {
                    cerr << "Too many lines!" << endl;
                }
            }
        }

        drawContours(contour_image, Contours, static_cast<int>(i), Scalar(b, g, r), 2);
    }

    while(waitKey(1) != 'x') {
        namedWindow("Contours_Image", WINDOW_NORMAL);
        imshow("Contours_Image", contour_image);
    }
}

int main(){
    vector<String> Images = GetImageNames(PathToFolder+"EastBeach/");
    vector<Point2f> DistortedCroppedPoints;
    Mat Sel_Cropped_Img;
    GetCoordsHFromCropped("East", DistortedCroppedPoints, Sel_Cropped_Img);

    vector<vector<Point>> Collected_contours;
    for (const auto& str : Images) {
        vector<Point> contour;
        Mat image_beach = imread(str), H;
        Point Location;
        Mat main_img = Calibrating_Homography(image_beach, DistortedCroppedPoints, Sel_Cropped_Img, H, Location);
        Detecting_Contours_H(image_beach, main_img, H, Sel_Cropped_Img, Location, contour);
        Collected_contours.push_back(contour);

        while(waitKey(1) != 'x') {
            namedWindow("Image", WINDOW_NORMAL);
            namedWindow("WarpedImage", WINDOW_NORMAL);
            imshow("Image", image_beach);
            imshow("WarpedImage", main_img);
        }
    }
    drawing_Lines_East(Sel_Cropped_Img, Collected_contours);

    return 0;
}
