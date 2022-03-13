#pragma once


namespace mathcustom
{
    float reMap(float x, float fromMin, float fromMax, float toMin, float toMax)
    {
        return (toMin + (x - fromMin) * (toMax - toMin) / (fromMax - fromMin));
    }
}
