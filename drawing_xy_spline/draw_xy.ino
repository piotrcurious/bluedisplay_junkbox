#include <BlueDisplay.h>
#include <CurveFitting.h>

#define NUM_POINTS 6

// Define the points along the x-axis
float xPoints[NUM_POINTS] = {0, 50, 100, 150, 200, 250};
float yPoints[NUM_POINTS] = {10, 60, 110, 60, 10, 30};

// For interpolated spline
float xSpline[NUM_POINTS * 10];
float ySpline[NUM_POINTS * 10];

// For BlueDisplay touch coordinates
int touchX, touchY;
int selectedPoint = -1;

BlueDisplay bd;

void setup() {
  bd.begin();
  bd.setCustomIconTitle(BlueDisplay::ICON_HOME, "Spline Drawing UI");
  bd.draw2dGraph("Spline Graph", 320, 240, 0, 0, 0, 300, 0, 150, false);
  
  // Draw initial points and spline
  updateSpline();
}

void loop() {
  bd.loop();
  
  if (bd.isTouched()) {
    touchX = bd.getTouchX();
    touchY = bd.getTouchY();
    
    // Check if any point is selected
    for (int i = 0; i < NUM_POINTS; i++) {
      if (abs(touchX - xPoints[i]) < 10 && abs(touchY - yPoints[i]) < 10) {
        selectedPoint = i;
        break;
      }
    }

    // Update point's Y position based on touch
    if (selectedPoint != -1) {
      yPoints[selectedPoint] = touchY;
      updateSpline();
    }
  }
  
  // Release touch
  if (bd.isTouchReleased()) {
    selectedPoint = -1;
  }
}

// Function to update and draw the spline
void updateSpline() {
  bd.clearGraphArea();
  
  // Draw control points
  for (int i = 0; i < NUM_POINTS; i++) {
    bd.drawCircle(xPoints[i], yPoints[i], 5, BlueDisplay::COLOR_RED);
  }
  
  // Perform cubic spline interpolation
  cubicSplineInterpolation(xPoints, yPoints, NUM_POINTS, xSpline, ySpline, NUM_POINTS * 10);
  
  // Draw the spline
  for (int i = 0; i < NUM_POINTS * 10 - 1; i++) {
    bd.drawLine(xSpline[i], ySpline[i], xSpline[i+1], ySpline[i+1], BlueDisplay::COLOR_BLUE);
  }
}

#include <BlueDisplay.h>
#include <CurveFitting.h>

#define NUM_POINTS 6

// Define the points along the x-axis
float xPoints[NUM_POINTS] = {0, 50, 100, 150, 200, 250};
float yPoints[NUM_POINTS] = {10, 60, 110, 60, 10, 30};

// For interpolated spline
float xSpline[NUM_POINTS * 10];
float ySpline[NUM_POINTS * 10];

// For BlueDisplay touch coordinates
int touchX, touchY;
int selectedPoint = -1;

BlueDisplay bd;

void setup() {
  bd.begin();
  bd.setCustomIconTitle(BlueDisplay::ICON_HOME, "Spline Drawing UI");
  bd.draw2dGraph("Spline Graph", 320, 240, 0, 0, 0, 300, 0, 150, false);
  
  // Draw initial points and spline
  updateSpline();
}

void loop() {
  bd.loop();
  
  if (bd.isTouched()) {
    touchX = bd.getTouchX();
    touchY = bd.getTouchY();
    
    // Check if any point is selected
    for (int i = 0; i < NUM_POINTS; i++) {
      if (abs(touchX - xPoints[i]) < 10 && abs(touchY - yPoints[i]) < 10) {
        selectedPoint = i;
        break;
      }
    }

    // Update point's Y position based on touch
    if (selectedPoint != -1) {
      yPoints[selectedPoint] = touchY;
      updateSpline();
    }
  }
  
  // Release touch
  if (bd.isTouchReleased()) {
    selectedPoint = -1;
  }
}

// Function to update and draw the spline
void updateSpline() {
  bd.clearGraphArea();
  
  // Draw control points
  for (int i = 0; i < NUM_POINTS; i++) {
    bd.drawCircle(xPoints[i], yPoints[i], 5, BlueDisplay::COLOR_RED);
  }
  
  // Perform cubic spline interpolation
  cubicSplineInterpolation(xPoints, yPoints, NUM_POINTS, xSpline, ySpline, NUM_POINTS * 10);
  
  // Draw the spline
  for (int i = 0; i < NUM_POINTS * 10 - 1; i++) {
    bd.drawLine(xSpline[i], ySpline[i], xSpline[i+1], ySpline[i+1], BlueDisplay::COLOR_BLUE);
  }
}

// Function to perform cubic spline interpolation
void cubicSplineInterpolation(float* x, float* y, int n, float* xi, float* yi, int m) {
  CurveFitting::Spline spline;
  spline.calcNaturalSpline(x, y, n);

  float step = (x[n-1] - x[0]) / (m - 1);
  for (int i = 0; i < m; i++) {
    xi[i] = x[0] + i * step;
    yi[i] = spline.getValueAt(xi[i]);
  }
}
