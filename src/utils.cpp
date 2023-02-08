#include "utils.hpp"

ImVec2 operator+(ImVec2 vect1, const ImVec2 &vect2)
{
    return ImVec2(vect1.x + vect2.x, vect1.y + vect2.y);
}

ImVec2 operator-(ImVec2 vect1, const ImVec2 &vect2)
{
    return ImVec2(vect1.x - vect2.x, vect1.y - vect2.y);
}

ImVec2 operator/(ImVec2 vect1, const ImVec2 &vect2)
{
    return ImVec2(vect1.x / vect2.x, vect1.y / vect2.y);
}

ImVec2 operator/(ImVec2 vect1, const int &nb)
{
    return ImVec2(vect1.x / nb, vect1.y / nb);
}

ImVec2 operator/(ImVec2 vect1, const float &nb)
{
    return ImVec2(vect1.x / nb, vect1.y / nb);
}

ImVec2 operator*(ImVec2 vect1, const ImVec2 &vect2)
{
    return ImVec2(vect1.x * vect2.x, vect1.y * vect2.y);
}

bool operator!=(ImVec2 vect1, const ImVec2 &vect2)
{
    return vect1.x != vect2.x || vect1.y != vect2.y;
}

bool operator<(ImVec2 vect1, ImVec2 &vect2)
{
    return vect1.x < vect2.x && vect1.y < vect2.y;
}
bool operator>(ImVec2 vect1, ImVec2 &vect2)
{
    return vect1.x > vect2.x && vect1.y > vect2.y;
}

void limit(int &value, int min, int max)
{
    if (value < min)
    {
        value = min;
    }
    else if (value > max)
    {
        value = max;
    }
}

void limit(ImVec2 &value, ImVec2 min, ImVec2 max)
{
    if (value.x < min.x)
    {
        value.x = min.x;
    }
    else if (value.x > max.x)
    {
        value.x = max.x;
    }

    if (value.y < min.y)
    {
        value.y = min.y;
    }
    else if (value.y > max.y)
    {
        value.y = max.y;
    }
}

void limit(float &value, float min, float max)
{
    if (value < min)
    {
        value = min;
    }
    else if (value > max)
    {
        value = max;
    }
}