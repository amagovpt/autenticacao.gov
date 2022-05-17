/* ****************************************************************************

* PTeID Middleware Project.
* Copyright (C) 2022 André Guerreiro - <aguerreiro1985@gmail.com>
*
**************************************************************************** */

#include "pteidControls.h"

#include <Windows.h>
#include <gdiplus.h>
#include <uxtheme.h>

#include "Log.h"

using namespace Gdiplus;

void PteidControls::Paint_Circle_Animation(HWND hWnd, HDC hdc, int circle_x, int circle_y, int outer_circle_diameter, int inner_circle_diameter, int angle) {

	const Gdiplus::Pen white_pen(Color(255, 255, 255, 255));

	const Gdiplus::SolidBrush cc_lightblue_brush(Color(255, 194, 199, 227));
	const Gdiplus::SolidBrush cc_blue_brush(Color(255, 62, 95, 172));
	const Gdiplus::SolidBrush white_brush(Color(255, 255, 255, 255));
	Gdiplus::Graphics graphics(hdc);

	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	//Fill white square that container the outer circle
	//The extra DrawRectangle call is needed to avoid some unawanted black border lines at the start of the animation
	graphics.DrawRectangle(&white_pen, circle_x, circle_y, outer_circle_diameter, outer_circle_diameter);
	graphics.FillRectangle(&white_brush, circle_x, circle_y, outer_circle_diameter, outer_circle_diameter);

	graphics.FillEllipse(&cc_lightblue_brush, circle_x, circle_y, outer_circle_diameter, outer_circle_diameter);

	graphics.FillPie(&cc_blue_brush, circle_x, circle_y, outer_circle_diameter, outer_circle_diameter, angle, 45.0f);
	//Smaller ellipse to paint white over the inner part of the circle slice
	graphics.FillEllipse(&white_brush, circle_x + 15, circle_y + 15, inner_circle_diameter, inner_circle_diameter);

}

void PteidControls::Circle_Animation_OnPaint(HWND hWnd, HDC hdc, const RECT *animation_rect, PAINTSTRUCT *ps, int angle)
{
	if (hdc)
	{
		HDC hdcTo;

		//This implements painting with double buffering which is essential for smooth animation rendering
		HPAINTBUFFER paint_buffer = BeginBufferedPaint(hdc, animation_rect, BPBF_COMPATIBLEBITMAP, NULL, &hdcTo);
		int outer_circle_diam = animation_rect->bottom - animation_rect->top;
		Paint_Circle_Animation(hWnd, hdcTo, animation_rect->left, animation_rect->top, outer_circle_diam, outer_circle_diam-30, angle);

		EndBufferedPaint(paint_buffer, TRUE);
		DeleteDC(hdcTo);
	}
}

void PteidControls::Circle_Animation_Setup(ULONG_PTR gdiplusToken) {
	GdiplusStartupInput gdiplusStartupInput;
	Status st = GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	MWLOG(LEV_DEBUG, MOD_DLG, L"GdiPlusStartup() status=%d", st);

	BufferedPaintInit();
}

void PteidControls::Circle_Animation_Destroy(ULONG_PTR gdiplusToken) {
	BufferedPaintUnInit();

	GdiplusShutdown(gdiplusToken);
}