#include "MouseSelect.h"

MouseSelectParams *g_mousep;
MouseSelect::MouseSelect() {}

MouseSelect::~MouseSelect() {}

void MouseSelect::select_rect(Mat &frame)
{
	obj_selected = 0;
	while (0 == obj_selected)
	{
		obj_selected = get_rects(frame);
		if (obj_selected == 0)
		{
			std::cout << "You haven't selected any rectangles. " << std::endl;
		}
	}
}

void MouseSelect::select_point(Mat &frame)
{
	obj_selected = 0;
	while (0 == obj_selected)
	{
		obj_selected = get_points(frame);
		if (obj_selected == 0)
		{
			std::cout << "You haven't selected any points." << std::endl;
		}
	}
}

int MouseSelect::get_rects(Mat &frame)
{
	char *win_name = "Init Frame";
	MouseSelectParams params;
	params.win_name = win_name;
	params.image = &frame;
	params.n = 0;
	params.mode = 2;
	namedWindow(win_name, WINDOW_AUTOSIZE);
	imshow(win_name, frame);

	g_mousep = &params;
	setMouseCallback(win_name, on_mouse, 0);
	waitKey(0);
	destroyWindow(win_name);

	int x1, x2, y1, y2, w, h;
	Rect rt;
	Point pt1, pt2;
	std::cout << "Point Num: " << params.pts.size() << " Object Num: " << params.n << " Object Type: Rect." << std::endl;
	for (int i = 0; i < params.pts.size(); i += 2)
	{
		pt1 = params.pts[i];
		pt2 = params.pts[i + 1];
		x1 = min(pt1.x, pt2.x);
		x2 = max(pt1.x, pt2.x);
		y1 = min(pt1.y, pt2.y);
		y2 = max(pt1.y, pt2.y);
		w = x2 - x1;
		h = y2 - y1;
		w = (w % 2) ? w : w + 1;
		h = (h % 2) ? h : h + 1;
		rt.x = x1;
		rt.y = y1;
		rt.width = w;
		rt.height = h;
		vRects.push_back(rt);
	}
	return params.n;
}

int MouseSelect::get_points(Mat &frame)
{
	char    *win_name = "Init Frame";
	MouseSelectParams params;
	params.win_name = win_name;
	params.image = &frame;
	params.n = 0;
	params.mode = 1;

	namedWindow(win_name, 1);
	imshow(win_name, frame);

	g_mousep = &params;
	setMouseCallback(win_name, on_mouse, 0);
	waitKey(0);

	destroyWindow(win_name);

	std::cout << "Point Num: " << params.pts.size() << " Object Num: " << params.n << " Object Type: Point." << std::endl;
	for (int i = 0; i < params.n; i++)
	{
		vPoints.push_back(params.pts[i]);
	}
	return params.n;
}

void on_mouse(int event, int x, int y, int flags, void *param)
{
	int        n;
	static bool pressed = false;
	static Point cur_pt;
	static Point prev_pt;
	/* on left button press, remember first corner of rectangle around object */
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		n = g_mousep->n;
		if (n == MAX_OBJECTS)
		{
			return;
		}
		prev_pt.x = x;
		prev_pt.y = y;
		//cout<<prev_pt.x <<","<<prev_pt.y<<" "<<cur_pt.x<<","<<cur_pt.y<<endl;
		pressed = true;
	}
	/* on left button up, finalise the rectangle and draw it in black */
	else if (event == CV_EVENT_LBUTTONUP && pressed)
	{
		/* 1 == mode for selecting points */
		if (1 == g_mousep->mode)
		{
			n = g_mousep->n;
			if (n == MAX_OBJECTS)
			{
				return;
			}
			cur_pt.x = x;
			cur_pt.y = y;
			g_mousep->pts.push_back(cur_pt);
			int radius = 3;
			circle(*g_mousep->image, cur_pt, radius, CV_RGB(255, 0, 0));
			imshow(g_mousep->win_name, *g_mousep->image);
			pressed = false;
			g_mousep->n++;
		}
		/* 2 == mode for selecting rectangles */
		else if (2 == g_mousep->mode)
		{
			n = g_mousep->n;
			if (n == MAX_OBJECTS)
			{
				return;
			}
			cur_pt.x = x;
			cur_pt.y = y;
			g_mousep->pts.push_back(prev_pt);
			g_mousep->pts.push_back(cur_pt);

//			g_mousep->rt = CvRect(prev_pt.x, prev_pt.y, cur_pt.x - prev_pt.x, cur_pt.y - prev_pt.y);
			//    cout<<prev_pt.x <<","<<prev_pt.y<<" "<<cur_pt.x<<","<<cur_pt.y<<endl;
			rectangle(*g_mousep->image, prev_pt, cur_pt, CV_RGB(0, 0, 255));
			imshow(g_mousep->win_name, *g_mousep->image);
			pressed = false;
			g_mousep->n++;
		}
		else if (3 == g_mousep->mode)
		{
			n = g_mousep->n;

		}
	}
	else if (event == CV_EVENT_MOUSEMOVE && pressed && CV_EVENT_FLAG_LBUTTON)
	{
		if (2 == g_mousep->mode)
		{
			n = g_mousep->n;
			if (n == MAX_OBJECTS)
			{
				return;
			}
			cur_pt.x = x;
			cur_pt.y = y;
			Mat tmp = (*g_mousep->image).clone();
			//cout<<prev_pt.x <<","<<prev_pt.y<<" "<<cur_pt.x<<","<<cur_pt.y<<endl;
			rectangle(tmp, prev_pt, cur_pt, CV_RGB(0, 255, 0));
			imshow(g_mousep->win_name, tmp);
		}
	}
}