#include <Arduino.h>
#include "BlueDisplay.hpp"

class ArrayEditor {
private:
    static const uint8_t ARRAY_SIZE = 20;
    static const uint8_t ARRAY_CLAMP_VALUE = 255;
    static const uint16_t BAR_COLOR = COLOR16_BLACK;
    static const uint16_t SELECTED_BAR_COLOR = COLOR16_RED;
    static const uint16_t BAR_BOUNDARY = COLOR16_RED;
    static const uint16_t BACKGROUND_COLOR = COLOR16_WHITE;
    static const uint8_t LABEL_FONT_SIZE = 24;
    static constexpr float SENSITIVITY_MULTIPLIER = 0.1;

    uint8_t dataArray[ARRAY_SIZE];
    uint16_t plotX, plotY, plotWidth, plotHeight;
    uint16_t barWidth;
    uint8_t touchedIndex;
    int touchEventIndex;
    int lastTouchedIndex;
    float sensitivity;
    float touchedDelta;
    struct XYPosition lastPos;

public:
    ArrayEditor(float xPercent, float yPercent, float widthPercent, float heightPercent) {
        plotXPercent = xPercent;
        plotYPercent = yPercent;
        plotWidthPercent = widthPercent;
        plotHeightPercent = heightPercent;
        touchEventIndex = -1;
        lastTouchedIndex = -1;
        touchedDelta = 0;
        
        for (int i = 0; i < ARRAY_SIZE; i++) {
            dataArray[i] = 0;
        }
    }

    void initPlot(uint16_t displayWidth, uint16_t displayHeight) {
        plotX = (displayWidth * plotXPercent) / 100;
        plotY = (displayHeight * plotYPercent) / 100;
        plotWidth = (displayWidth * plotWidthPercent) / 100;
        plotHeight = (displayHeight * plotHeightPercent) / 100;
        barWidth = plotWidth / (ARRAY_SIZE) + 1;
        sensitivity = ((float)ARRAY_CLAMP_VALUE / displayHeight) * SENSITIVITY_MULTIPLIER;
    }

    void drawBars() {
        for (uint8_t i = 0; i < ARRAY_SIZE; i++) {
            updateBar(i, BAR_COLOR);
        }
    }

    void updateBar(uint8_t index, uint16_t barColor) {
        uint16_t xPos = plotX + index * barWidth;
        uint16_t barHeight = map(dataArray[index], 0, ARRAY_CLAMP_VALUE, 0, plotHeight);

        BlueDisplay1.fillRect(xPos, plotY + plotHeight - barHeight, xPos + barWidth - 1, plotY, BACKGROUND_COLOR);
        BlueDisplay1.fillRect(xPos, plotY + plotHeight, xPos + barWidth - 1, plotY + plotHeight - barHeight, barColor);

        char sStringBuffer[8];
        sprintf(sStringBuffer, "%d", dataArray[index]);
        BlueDisplay1.drawText((xPos + (barWidth / 2)) - LABEL_FONT_SIZE * 1, plotY + plotHeight, sStringBuffer, LABEL_FONT_SIZE, barColor, BACKGROUND_COLOR);
    }

    void handleTouchMove(struct TouchEvent *const aCurrentPositionPtr) {
        if (touchedIndex < ARRAY_SIZE) {
            touchedDelta += (lastPos.PositionY - aCurrentPositionPtr->TouchPosition.PositionY) 
                * (sensitivity * abs(lastPos.PositionY - aCurrentPositionPtr->TouchPosition.PositionY));
            
            int delta_Y = dataArray[touchedIndex] + touchedDelta;
            dataArray[touchedIndex] = constrain(delta_Y, 0, ARRAY_CLAMP_VALUE);
        }
        lastPos.PositionX = aCurrentPositionPtr->TouchPosition.PositionX;
        lastPos.PositionY = aCurrentPositionPtr->TouchPosition.PositionY;
        touchEventIndex = touchedIndex;
    }

    void handleTouchDown(struct TouchEvent *const aCurrentPositionPtr) {
        if (aCurrentPositionPtr->TouchPosition.PositionX >= plotX && 
            aCurrentPositionPtr->TouchPosition.PositionX <= plotX + plotWidth && 
            aCurrentPositionPtr->TouchPosition.PositionY >= plotY && 
            aCurrentPositionPtr->TouchPosition.PositionY <= plotY + plotHeight) {
            
            if ((aCurrentPositionPtr->TouchPosition.PositionX - plotX) / barWidth < ARRAY_SIZE) {
                touchedIndex = (aCurrentPositionPtr->TouchPosition.PositionX - plotX) / barWidth;
                touchEventIndex = touchedIndex;
                touchedDelta = 0;
            }
        }
        lastPos.PositionX = aCurrentPositionPtr->TouchPosition.PositionX;
        lastPos.PositionY = aCurrentPositionPtr->TouchPosition.PositionY;
    }

    void update() {
        if (lastTouchedIndex != touchedIndex && lastTouchedIndex > -1) {
            updateBar(lastTouchedIndex, BAR_COLOR);
            lastTouchedIndex = -1;
        }
        if (touchEventIndex > -1) {
            updateBar(touchEventIndex, SELECTED_BAR_COLOR);
            lastTouchedIndex = touchEventIndex;
            touchEventIndex = -1;
        }
    }

private:
    float plotXPercent, plotYPercent, plotWidthPercent, plotHeightPercent;
};

// Global variables
ArrayEditor editor1(10.0, 10.0, 80.0, 35.0);
ArrayEditor editor2(10.0, 55.0, 80.0, 35.0);

void initDisplay() {
    uint16_t displayWidth = BlueDisplay1.getMaxDisplayWidth();
    uint16_t displayHeight = BlueDisplay1.getMaxDisplayHeight();
    BlueDisplay1.setFlagsAndSize(BD_FLAG_FIRST_RESET_ALL | BD_FLAG_USE_MAX_SIZE, displayWidth, displayHeight);
}

void drawGui() {
    struct XYSize* maxSize = BlueDisplay1.getMaxDisplaySize();
    uint16_t maxDisplayWidth = maxSize->XWidth;
    uint16_t maxDisplayHeight = maxSize->YHeight;

    editor1.initPlot(maxDisplayWidth, maxDisplayHeight);
    editor2.initPlot(maxDisplayWidth, maxDisplayHeight);

    BlueDisplay1.clearDisplay(COLOR16_WHITE);
    editor1.drawBars();
    editor2.drawBars();
}

void handleTouchMove(struct TouchEvent *const aCurrentPositionPtr) {
    editor1.handleTouchMove(aCurrentPositionPtr);
    editor2.handleTouchMove(aCurrentPositionPtr);
}

void handleTouchDown(struct TouchEvent *const aCurrentPositionPtr) {
    editor1.handleTouchDown(aCurrentPositionPtr);
    editor2.handleTouchDown(aCurrentPositionPtr);
}

void setup() {
    Serial0.begin(115200);
    initSerial("array_example");
    BlueDisplay1.initCommunication(&initDisplay, &drawGui);
    registerTouchMoveCallback(&handleTouchMove);
    registerTouchDownCallback(&handleTouchDown);
}

void loop() {
    for (uint8_t i = 0; i < 32; i++) {
        checkAndHandleEvents();
    }
    editor1.update();
    editor2.update();
    delay(20);
}
