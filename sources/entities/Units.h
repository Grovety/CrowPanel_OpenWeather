#pragma once

class Units
{
public:
    enum Temperature
    {
        Celsium = 0,
        Fahrenheit

    };
    enum Pressure
    {
        hPa = 0,
        mmHg,
    };

    static constexpr char TemperatureStr[2][10] = { "\u00B0C", "\u00B0F" };
    static constexpr char PressureStr[2][10]    = { " hPa", " mmHg" };

    static Units& instance()
    {
        static Units instance;
        return instance;
    }
    bool        setTemperature(Temperature);
    bool        setPressure(Pressure);
    float       convertTemp(float celsium);
    float       convertPressure(float hPa);
    const char* getTemperatureString();
    const char* getPressureString();
    Temperature getTemperature()
    {
        return temperature;
    };
    Pressure getPressure()
    {
        return pressure;
    };

    static const char* getTemperatureString4Lvgl()
    {
        return "\u00B0C\n\u00B0F";
    }
    static const char* getPressureString4Lvgl()
    {
        return "hPa\nmmHg";
    }
    void init();

private:
    static constexpr Temperature DefaultTemperature = Temperature::Celsium;
    static constexpr Pressure    DefaultPressure    = Pressure::hPa;
    Temperature                  temperature        = DefaultTemperature;
    Pressure                     pressure           = DefaultPressure;
};