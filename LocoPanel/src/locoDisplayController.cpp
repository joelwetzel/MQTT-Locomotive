#include "locoDisplayController.h"

LocoDisplayController::LocoDisplayController(LocoList &locoList, Adafruit_SSD1306 &display)
    : _locoList(locoList), _display(display)
{
    currentIndex = 0;

    currentState = LocoDisplayState::Booting;
}

void LocoDisplayController::Setup()
{
    if (!_display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println("SSD1306 allocation failed");
        for (;;)
          ;
    }

    _display.clearDisplay();
    _display.display();


    _display.setTextSize(2);
    _display.setTextColor(WHITE);
    _display.setCursor(0,0);
    _display.println("Loco v1.0");
    _display.display();
}


void LocoDisplayController::HandleDisconnected()
{
    currentState = LocoDisplayState::ConnectingToWifi;
    _locoList.Clear();
    ProcessStep();
}

void LocoDisplayController::HandleConnectedToMqtt()
{
    currentState = LocoDisplayState::SearchingAndDisplayingLocos;
}


void LocoDisplayController::ProcessStep()
{
    _display.clearDisplay();

    switch (currentState)
    {
    case LocoDisplayState::Booting:
        _display.setTextSize(2);
        _display.setTextColor(WHITE);
        _display.setCursor(0,0);
        _display.println("LocoPanel");
        break;
    case LocoDisplayState::ConnectingToWifi:
        _display.setTextSize(2);
        _display.setTextColor(WHITE);
        _display.setCursor(0,0);
        _display.println("LocoPanel");

        _display.setTextSize(1);
        _display.setCursor(0, 16);
        _display.println("Connecting...");
        break;
    case LocoDisplayState::SearchingAndDisplayingLocos:
        if (_locoList.GetLocoCount() == 0)
        {
            // No locos discovered yet.
            _display.setTextSize(1);
            _display.setCursor(0, 16);
            _display.println("Searching...");
        }

        _display.setTextSize(2);

        std::vector<String> nextLocos = _locoList.GetListStartingAtIndex(currentIndex);

        for (int i = 0; i < 4 && i < nextLocos.size(); i++)
        {
            _display.setCursor(0, 16 * i);
            _display.println(nextLocos[i]);
        }
        break;
    }

    _display.display();
}


String LocoDisplayController::GetSelectedRoadname()
{
    if (_locoList.GetLocoCount() == 0)
    {
        // No locos discovered yet.  Return empty roadName.
        return String();
    }

    return _locoList.GetRoadnameByIndex(currentIndex);
}


void LocoDisplayController::MoveToNextLoco()
{
    if (_locoList.GetLocoCount() == 0)
    {
        // No locos discovered yet.
        return;
    }

    currentIndex++;

    if (currentIndex >= _locoList.GetLocoCount())
    {
        currentIndex = 0;
    }
}