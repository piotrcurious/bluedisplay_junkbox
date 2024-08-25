void ArrayEditor::draw() {
    if (_lastTouchedIndex != _touchedIndex && _lastTouchedIndex > -1) {
        updateBars(_lastTouchedIndex, _barColor);
        _lastTouchedIndex = -1;
    }
    if (_touchEventIndex > -1) {
        updateBars(_touchEventIndex, _selectedBarColor);
        _lastTouchedIndex = _touchEventIndex;
        _touchEventIndex = -1;
    }
    for (uint8_t i = 0; i < _arraySize; i++) {
        updateBars(i, _barColor);
    }
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
