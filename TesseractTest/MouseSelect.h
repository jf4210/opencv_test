#pragma once
#ifndef __MOUSESELECT_H__
#define __MOUSESELECT_H__

#include <opencv2/opencv.hpp>
#include <iostream>

#define MAX_OBJECTS    10
using namespace cv;
//using namespace std;
typedef struct MouseSelectParams
{
	std::vector<Point> pts;        //Points of selected
	char        *win_name;
	Mat            *image;
	unsigned int mode;        //1 Point,2 Rect
	int             n;            // selected object number
}MouseSelectParams;

void on_mouse(int event, int x, int y, int flags, void *param);
class MouseSelect
{
public:
	MouseSelect();
	~MouseSelect();

	void select_rect(Mat &frame);
	void select_point(Mat &frame);

	std::vector<Point> vPoints;
	std::vector<Rect> vRects;
	unsigned int obj_selected;
private:
	int get_rects(Mat &frame);
	int get_points(Mat &frame);

};
#endif