#include <Arduino.h>
#include "BlueDisplay.hpp"

#define ARRAY_SIZE 10
#define MAX_BAR_HEIGHT 100
#define BAR_COLOR COLOR16_BLUE
#define BACKGROUND_COLOR COLOR16_WHITE

// Bar plot positioning and size as percentages
float plotXPercent = 10.0;   // X position as a percentage of screen width
float plotYPercent = 10.0;   // Y position as a percentage of screen height
float plotWidthPercent = 80.0;  // Width as a percentage of screen width
float plotHeightPercent = 80.0; // Height as a percentage of screen height

uint8_t dataArray[ARRAY_SIZE] = {20, 40, 60, 80, 100, 80, 60, 40, 20, 10};
uint8_t lastArray[ARRAY_SIZE];

void initDisplay(void);
void drawBars();
void updateBars(uint8_t index, uint8_t newValue);
void handleTouchMove(BDTouchEvent *event);

uint16_t plotX, plotY, plotWidth, plotHeight;
uint16_t barWidth;

void setup() {
    Serial.begin(115200);
    initSerial();
    BlueDisplay1.initCommunication(&initDisplay, NULL);
    BlueDisplay1.registerTouchMoveCallback(&handleTouchMove);
}

void loop() {
    checkAndHandleEvents();
    // Periodically update the GUI
    for (uint8_t i = 0; i < ARRAY_SIZE; i++) {
        if (dataArray[i] != lastArray[i]) {
            updateBars(i, dataArray[i]);
            lastArray[i] = dataArray[i];
        }
    }
}

void initDisplay() {
    // Get the maximum display size
    struct XYSize* maxSize = BlueDisplay1.getMaxDisplaySize();
    uint16_t maxDisplayWidth = maxSize->XWidth;
    uint16_t maxDisplayHeight = maxSize->YHeight;

    // Set up plot dimensions based on percentages
    plotX = (maxDisplayWidth * plotXPercent) / 100;
    plotY = (maxDisplayHeight * plotYPercent) / 100;
    plotWidth = (maxDisplayWidth * plotWidthPercent) / 100;
    plotHeight = (maxDisplayHeight * plotHeightPercent) / 100;

    // Calculate the width of each bar
    barWidth = plotWidth / ARRAY_SIZE;

    // Clear the display with background color
    BlueDisplay1.clearDisplay(BACKGROUND_COLOR);
    drawBars();
}

void drawBars() {
    for (uint8_t i = 0; i < ARRAY_SIZE; i++) {
        uint16_t xPos = plotX + i * barWidth;
        uint16_t barHeight = map(dataArray[i], 0, MAX_BAR_HEIGHT, 0, plotHeight);
        BlueDisplay1.fillRect(xPos, plotY + plotHeight - barHeight, barWidth, barHeight, BAR_COLOR);
    }
}

void updateBars(uint8_t index, uint8_t newValue) {
    uint16_t xPos = plotX + index * barWidth;
    BlueDisplay1.fillRect(xPos, plotY, barWidth, plotHeight, BACKGROUND_COLOR);  // Clear the old bar
    uint16_t barHeight = map(newValue, 0, MAX_BAR_HEIGHT, 0, plotHeight);
    BlueDisplay1.fillRect(xPos, plotY + plotHeight - barHeight, barWidth, barHeight, BAR_COLOR);
}

void handleTouchMove(BDTouchEvent *event) {
    if (event->x >= plotX && event->x <= plotX + plotWidth && event->y >= plotY && event->y <= plotY + plotHeight) {
        uint8_t touchedIndex = (event->x - plotX) / barWidth;
        if (touchedIndex < ARRAY_SIZE) {
            uint8_t newBarValue = map(plotHeight + plotY - event->y, 0, plotHeight, 0, MAX_BAR_HEIGHT);
            dataArray[touchedIndex] = constrain(newBarValue, 0, MAX_BAR_HEIGHT);
        }
    }
}
