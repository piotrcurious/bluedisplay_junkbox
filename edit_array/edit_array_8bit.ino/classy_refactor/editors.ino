#include <Arduino.h>
#include "BlueDisplay.hpp"

#define AMOUNT_OF_REQUESTS_PROCESSED 32
#define ARRAY_SIZE 20
#define ARRAY_CLAMP_VALUE 255
#define BAR_COLOR COLOR16_BLACK
#define SELECTED_BAR_COLOR COLOR16_RED
#define BAR_BOUNDARY COLOR16_RED
#define BACKGROUND_COLOR COLOR16_WHITE
#define LABEL_FONT_SIZE 24

class ArrayEditor {
public:
    ArrayEditor(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t* dataArray, uint8_t arraySize, uint8_t clampValue, color16_t barColor, color16_t selectedBarColor, color16_t backgroundColor, uint8_t labelFontSize);
    void draw();
    void updateBars(uint8_t index, color16_t barColor);
    void handleTouchMove(struct TouchEvent* const aCurrentPositionPtr);
    void handleTouchDown(struct TouchEvent* const aCurrentPositionPtr);
    void enable();
    void disable();

private:
    uint16_t _x;
    uint16_t _y;
    uint16_t _width;
    uint16_t _height;
    uint8_t* _dataArray;
    uint8_t _arraySize;
    uint8_t _clampValue;
    color16_t _barColor;
    color16_t _selectedBarColor;
    color16_t _backgroundColor;
    uint8_t _labelFontSize;
    uint16_t _barWidth;
    uint8_t _touchedIndex;
    int _touchEventIndex;
    int _lastTouchedIndex;
    float _sensitivity;
    float _touchedDelta;
    struct XYPosition _lastPos;
};

ArrayEditor::ArrayEditor(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t* dataArray, uint8_t arraySize, uint8_t clampValue, color16_t barColor, color16_t selectedBarColor, color16_t backgroundColor, uint8_t labelFontSize) {
    _x = x;
    _y = y;
    _width = width;
    _height = height;
    _dataArray = dataArray;
    _arraySize = arraySize;
    _clampValue = clampValue;
    _barColor = barColor;
    _selectedBarColor = selectedBarColor;
    _backgroundColor = backgroundColor;
    _labelFontSize = labelFontSize;
    _barWidth = _width / _arraySize;
    _touchedIndex = 0;
    _touchEventIndex = -1;
    _lastTouchedIndex = -1;
    _sensitivity = ((float)_clampValue / _height) * 0.1;
    _touchedDelta = 0;
}

void ArrayEditor::draw() {
    for (uint8_t i = 0; i < _arraySize; i++) {
        updateBars(i, _barColor);
    }
}

void ArrayEditor::updateBars(uint8_t index, color16_t barColor) {
    uint16_t xPos = _x + index * _barWidth;
    uint16_t barHeight = map(_dataArray[index], 0, _clampValue, 0, _height);
    BlueDisplay1.fillRect(xPos, _y + _height - barHeight, _barWidth, barHeight, barColor);
    sprintf(sStringBuffer, "%d", _dataArray[index]);
    BlueDisplay1.drawText((xPos + (_barWidth / 2)) - _labelFontSize * 1, _y + _height, sStringBuffer, _labelFontSize, barColor, _backgroundColor);
}

void ArrayEditor::handleTouchMove(struct TouchEvent* const aCurrentPositionPtr) {
    if (aCurrentPositionPtr->TouchPosition.PositionX >= _x && aCurrentPositionPtr->TouchPosition.PositionX <= _x + _width && aCurrentPositionPtr->TouchPosition.PositionY >= _y && aCurrentPositionPtr->TouchPosition.PositionY <= _y + _height) {
        _touchedDelta += (aCurrentPositionPtr->TouchPosition.PositionY - _lastPos.PositionY) * (_sensitivity * abs(aCurrentPositionPtr->TouchPosition.PositionY - _lastPos.PositionY));
        int delta_Y = _dataArray[_touchedIndex] + _touchedDelta;
        _dataArray[_touchedIndex] = constrain(delta_Y, 0, _clampValue);
    }
    _lastPos.PositionX = aCurrentPositionPtr->TouchPosition.PositionX;
    _lastPos.PositionY = aCurrentPositionPtr->TouchPosition.PositionY;
    _touchEventIndex = _touchedIndex;
}

void ArrayEditor::handleTouchDown(struct TouchEvent* const aCurrentPositionPtr) {
    if (aCurrentPositionPtr->TouchPosition.PositionX >= _x && aCurrentPositionPtr->TouchPosition.PositionX <= _x + _width && aCurrentPositionPtr->TouchPosition.PositionY >= _y && aCurrentPositionPtr->TouchPosition.PositionY <= _y + _height) {
        _touchedIndex = (aCurrentPositionPtr->TouchPosition.PositionX - _x) / _barWidth;
        _touchEventIndex = _touchedIndex;
        _touchedDelta = 0;
    }
    _lastPos.PositionX = aCurrentPositionPtr->TouchPosition.PositionX;
    _lastPos.PositionY = aCurrentPositionPtr->TouchPosition.PositionY;
}

void ArrayEditor::enable() {
    // Add the editor to the active view
    // ...
}

void ArrayEditor::disable() {
    // Remove the editor from the active view
    // ...
}

// Example usage:
ArrayEditor editor1(10, 10, 200, 200, dataArray1, ARRAY_SIZE, ARRAY_CLAMP_VALUE, BAR_COLOR, SELECTED_BAR_COLOR, BACKGROUND_COLOR, LABEL_FONT_SIZE);
ArrayEditor editor2(220, 10, 200, 200, dataArray2, ARRAY_SIZE, ARRAY_CLAMP_VALUE, BAR_COLOR, SELECTED_BAR_COLOR, BACKGROUND_COLOR, LABEL_FONT_SIZE);

void setup() {
    // ...
    editor1.enable();
    editor2.enable();
}

void loop() {
    // ...
    editor1.draw();
    editor2.draw();
    // ...
}

void handleTouchMove(struct TouchEvent* const aCurrentPositionPtr) {
    editor1.handleTouchMove(aCurrentPositionPtr);
    editor2.handleTouchMove(aCurrentPositionPtr);
}

void handleTouchDown(struct TouchEvent* const aCurrentPositionPtr) {
    editor1.handleTouchDown(aCurrentPositionPtr);
    editor2.handleTouchDown(aCurrentPositionPtr);
}
