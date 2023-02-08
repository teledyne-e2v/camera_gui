
#include "imgui.h"
ImVec2 operator+(ImVec2 vect1, const ImVec2 &vect2);

ImVec2 operator-(ImVec2 vect1, const ImVec2 &vect2);

ImVec2 operator/(ImVec2 vect1, const ImVec2 &vect2);

ImVec2 operator*(ImVec2 vect1, const ImVec2 &vect2);

bool operator<(ImVec2 vect1, ImVec2 &vect2);
ImVec2 operator/(ImVec2 vect1, const float &nb);
ImVec2 operator/(ImVec2 vect1, const int &nb);
bool operator>(ImVec2 vect1, ImVec2 &vect2);
bool operator!=(ImVec2 vect1, const ImVec2 &vect2);
/**
 * @brief if value < min set value to min
 * if value > max set value to max
 * 
 * @param value 
 * @param min 
 * @param max 
 */
void limit(int &value, int min, int max);
/**
 * @brief if value.x < min.x set value to min.x
 * if value.x > max.x set value to max.x
 * if value.y < min.y set value to min.y
 * if value.y > max.y set value to max.y
 * 
 * @param value 
 * @param min 
 * @param max 
 */
void limit(ImVec2 &value, ImVec2 min, ImVec2 max);
/**
 * @brief if value < min set value to min
 * if value > max set value to max
 * 
 * @param value 
 * @param min 
 * @param max 
 */
void limit(float &value, float min, float max);