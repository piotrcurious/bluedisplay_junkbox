#include <BlueDisplay.h>


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
  // Arrays to store spline coefficients
  float h[n-1], alpha[n-1], l[n], mu[n], z[n];
  float c[n], b[n], d[n];
  
  // Calculate h[i] and alpha[i] used in the system of equations
  for (int i = 0; i < n-1; i++) {
    h[i] = x[i+1] - x[i];  // Interval width
    alpha[i] = (3.0 / h[i]) * (y[i+1] - y[i]) - (3.0 / h[i-1]) * (y[i] - y[i-1]);  // Right-hand side of the system
  }

  // Initialize the coefficients for the tridiagonal system
  l[0] = 1.0;  // Boundary condition for the first spline segment
  mu[0] = 0.0;  // Boundary condition for the first spline segment
  z[0] = 0.0;  // Boundary condition for the first spline segment

  // Forward pass of the Thomas algorithm to solve the tridiagonal system
  for (int i = 1; i < n-1; i++) {
    l[i] = 2.0 * (x[i+1] - x[i-1]) - h[i-1] * mu[i-1];  // Compute l[i]
    mu[i] = h[i] / l[i];  // Compute mu[i]
    z[i] = (alpha[i] - h[i-1] * z[i-1]) / l[i];  // Compute z[i]
  }

  // Boundary conditions for the last segment
  l[n-1] = 1.0;  // Boundary condition for the last spline segment
  z[n-1] = 0.0;  // Boundary condition for the last spline segment
  c[n-1] = 0.0;  // Boundary condition for the last spline segment

  // Backward pass of the Thomas algorithm to solve the tridiagonal system
  for (int j = n-2; j >= 0; j--) {
    c[j] = z[j] - mu[j] * c[j+1];  // Compute c[j]
    b[j] = (y[j+1] - y[j]) / h[j] - h[j] * (c[j+1] + 2.0 * c[j]) / 3.0;  // Compute b[j]
    d[j] = (c[j+1] - c[j]) / (3.0 * h[j]);  // Compute d[j]
  }

  // Compute spline values
  float step = (x[n-1] - x[0]) / (m - 1);  // Step size for interpolation
  for (int i = 0; i < m; i++) {
    float t = x[0] + i * step;  // Compute interpolation point
    int j = 0;
    
    // Find the interval that contains the point t
    while (j < n-1 && x[j+1] < t) {
      j++;
    }
    
    // Calculate the relative position within the interval
    float dx = t - x[j];
    
    // Evaluate the spline polynomial at t
    yi[i] = y[j] + b[j] * dx + c[j] * dx * dx + d[j] * dx * dx * dx;
    xi[i] = t;
  }
}
