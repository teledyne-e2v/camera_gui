#include "roi.hpp"

#include <stdio.h>

ROI::ROI(GstElement *autofocus)
    : autofocus(autofocus)
{
    normPos1 = {0, 0};
    normPos2 = {1, 1};
}

ROI::~ROI()
{
}

ImVec4 ROI::getROI()
{
    int x = static_cast<int>(normPos1.x * videoWidth);
    int y = static_cast<int>(normPos1.y * videoHeight);

    int width = static_cast<int>((normPos2.x - normPos1.x) * videoWidth);
    int height = static_cast<int>((normPos2.y - normPos1.y) * videoHeight);
    int x1 = x;
    int x2 = x + width;
    int y1;
    int y2;
    if (y > y + height)
    {
        y1 = y + height;
        y2 = y;
    }
    else
    {
        y2 = y + height;
        y1 = y;
    }

    if (x > x + width)
    {
        x1 = x + width;
        x2 = x;
    }
    else
    {
        x2 = x + width;
        x1 = x;
    }

    return ImVec4((float)x1, (float)y1, (float)x2, (float)y2);
}

void ROI::setVideoSize(int width, int height)
{
    this->videoWidth = static_cast<float>(width);
    this->videoHeight = static_cast<float>(height);
}

void ROI::resetROI()
{
    normPos1 = {0, 0};
    normPos2 = {1, 1};
    changed = true;
    updatePluginROI();
}

void ROI::selectROI(ImVec2 streamPosition, ImVec2 streamSize, ImVec2 windowSize, ImVec2 windowPosition)
{
    if (!dragging && ImGui::IsMouseClicked(0))
    {
        // slightly reduce dragging area to prevent changing the ROI while resizing a window
        windowPosition = windowPosition + ImVec2(5, 5);
        windowSize = windowSize - ImVec2(10, 10);

        if (ImGui::IsMouseHoveringRect(windowPosition, windowPosition + windowSize, false))
        {
            normDraggingPos = (ImGui::GetMousePos() - streamPosition) / streamSize;

            limit(normDraggingPos.x, 0, 1);
            limit(normDraggingPos.y, 0, 1);

            dragging = true;
        }
    }
    else if (dragging && ImGui::IsMouseDragging(0))
    {
        normPos1 = normDraggingPos;
        normPos2 = (ImGui::GetMousePos() - streamPosition) / streamSize;

        limit(normPos1.x, 0, 1);
        limit(normPos2.x, 0, 1);
        limit(normPos1.x, 0, 1);
        limit(normPos2.y, 0, 1);

        sort(normPos1.x, normPos2.x);
        sort(normPos1.y, normPos2.y);
    }
    else if (dragging && ImGui::IsMouseReleased(0))
    {
        dragging = false;
        changed = true;
        updatePluginROI();
    }
}

void ROI::moveROI(ImVec2 streamPosition, ImVec2 streamSize)
{
    ImVec2 normCurrPos = (ImGui::GetMousePos() - streamPosition) / streamSize;

    ImVec2 absPos1 = (normPos1 * streamSize) + streamPosition;
    ImVec2 absPos2 = (normPos2 * streamSize) + streamPosition;

    if (!dragging && ImGui::IsMouseClicked(0) && ImGui::IsMouseHoveringRect(absPos1, absPos2, false))
    {
        normDraggingPos = normCurrPos;
        dragging = true;
    }
    else if (dragging && ImGui::IsMouseDragging(0))
    {
        // delta between the current mouse position and the its position during the previous frame
        ImVec2 delta = normCurrPos - normDraggingPos;

        float centerX = (normPos2.x - normPos1.x) / 2.0f;
        float centerY = (normPos2.y - normPos1.y) / 2.0f;

        // Don't move the roi on the X axis while the mouse is outside the video stream on the left or right
        if ((delta.x > 0 && normCurrPos.x < (0 + centerX)) ||
            (delta.x < 0 && normCurrPos.x > (1 - centerX)))
            delta.x = 0;

        // Don't move the roi on the Y axis while the mouse is outside the video stream on the top or bottom
        if ((delta.y > 0 && normCurrPos.y < (0 + centerY)) ||
            (delta.y < 0 && normCurrPos.y > (1 - centerY)))
            delta.y = 0;

        float newX1 = normPos1.x + delta.x;
        float newY1 = normPos1.y + delta.y;
        float newX2 = normPos2.x + delta.x;
        float newY2 = normPos2.y + delta.y;

        if (newX1 >= 0 && newX2 <= 1) // directly update the x coordinate when the ROI would still be inside the video
        {
            normPos1.x = newX1;
            normPos2.x = newX2;
        }
        else
        {
            // Snap the ROI to the edge of the video
            if (newX1 < 0)
            {
                normPos2.x -= normPos1.x;
                normPos1.x = 0;
            }
            else if (newX2 > 1)
            {
                normPos1.x += (1 - normPos2.x);
                normPos2.x = 1;
            }
        }

        if (newY1 >= 0 && newY2 <= 1) // directly update the y coordinate when the ROI would still be inside the video
        {
            normPos1.y = newY1;
            normPos2.y = newY2;
        }
        else
        {
            // Snap the ROI to the edge of the video
            if (newY1 < 0)
            {
                normPos2.y -= normPos1.y;
                normPos1.y = 0;
            }
            else if (newY2 > 1)
            {
                normPos1.y += (1 - normPos2.y);
                normPos2.y = 1;
            }
        }

        normDraggingPos = normCurrPos;
    }
    else if (dragging && ImGui::IsMouseReleased(0))
    {
        dragging = false;
        changed = true;
        updatePluginROI();
    }
}

void ROI::centerROI(ImVec2 center)
{
    ImVec2 pos1 = normPos1;
    normPos1 = center - (normPos2 - normPos1) / 2.0f;
    normPos2 = center + (normPos2 - pos1) / 2.0f;

    changed = true;

    updatePluginROI();
}

void ROI::drawROI(ImDrawList *drawList, ImVec2 windowPos, ImVec2 windowSize, bool lostFocus)
{
    ImVec2 pos1 = windowPos + (normPos1 * windowSize);
    ImVec2 pos2 = windowPos + (normPos2 * windowSize);

    ImColor color = (lostFocus) ? ImColor(1.0f, 0.1f, 0.1f, 1.0f) : ImColor(0.1f, 1.0f, 0.1f, 1.0f);

    drawList->AddRect(pos1, pos2, color, (0.0F), 0, (4.0F));
}

void ROI::sort(float &p1, float &p2)
{
    if (p1 > p2)
    {
        float tmp = p2;
        p2 = p1;
        p1 = tmp;
    }
}

void ROI::updatePluginROI()
{
#ifndef DEBUG_MODE
    if (changed)
    {
        // convert the roi normalize positions into absolute coordinates, width and height

        int x = static_cast<int>(normPos1.x * videoWidth);
        int y = static_cast<int>(normPos1.y * videoHeight);

        int width = static_cast<int>((normPos2.x - normPos1.x) * videoWidth);
        int height = static_cast<int>((normPos2.y - normPos1.y) * videoHeight);

        g_object_set(G_OBJECT(autofocus),
                     "x", x,
                     "y", y,
                     "width", width,
                     "height", height,
                     NULL);
    }
#endif

    changed = false;
}

void ROI::render()
{
    bool changePos = false;
    lastSizeX = (normPos2.x - normPos1.x) * videoWidth;
    lastSizeY = (normPos2.y - normPos1.y) * videoHeight;
    centerX = videoWidth * (normPos1.x + normPos2.x) / 2.0;
    centerY = videoHeight * (normPos1.y + normPos2.y) / 2.0;
    ImGui::PushItemWidth(-1);

    if (ImGui::CollapsingHeader("Edit ROI"))
    {
        ImGui::Spacing();
        ImGui::Text("Size:");
        ImGui::Spacing();
        ImGui::Text("Width:");
        ImGui::SameLine(70);
        if (ImGui::InputInt("###width", &lastSizeX, 10, 100))
        {
            limit(lastSizeX, 0, videoWidth);
            if (centerX + lastSizeX / 2 > videoWidth)
            {
                lastSizeX = 2 * (videoWidth - centerX);
            }
            if (centerX - lastSizeX / 2 < 0)
            {
                lastSizeX = 2 * centerX;
            }

            float delta = (lastSizeX / videoWidth) - (normPos2.x - normPos1.x);

            if (delta != 0.0f)
            {
                normPos1.x -= delta / 2.0f;
                normPos2.x += delta / 2.0f;

                changed = true;

                updatePluginROI();
            }
        }

        ImGui::Text("Heigth:");
        ImGui::SameLine(70);

        if (ImGui::InputInt("###heigth", &lastSizeY, 10, 100))
        {
            limit(lastSizeY, 0, videoHeight);
            if (centerY + lastSizeY / 2 > videoHeight)
            {
                lastSizeY = 2 * (videoHeight - centerY);
            }
            if (centerY - lastSizeY / 2 < 0)
            {
                lastSizeY = 2 * centerY;
            }

            float delta = (lastSizeY / videoHeight) - (normPos2.y - normPos1.y);

            if (delta != 0.0f)
            {
                normPos1.y -= delta / 2.0f;
                normPos2.y += delta / 2.0f;

                changed = true;

                updatePluginROI();
            }
        }

        ImGui::Spacing();
        ImGui::Text("Center coordinates:");
        ImGui::Spacing();
        ImGui::Text("X:");
        ImGui::SameLine(70);
        changePos |= ImGui::InputInt("### centre X", &centerX, 10, 100);

        ImGui::Text("Y:");
        ImGui::SameLine(70);
        changePos |= ImGui::InputInt("### centre Y", &centerY, 10, 100);
        if (changePos)
        {
            if (centerX + lastSizeX / 2 > videoWidth)
            {
                centerX = videoWidth - lastSizeX / 2;
            }
            if (centerX - lastSizeX / 2 < 0)
            {
                centerX = lastSizeX / 2;
            }
            if (centerY + lastSizeY / 2 > videoHeight)
            {
                centerY = videoHeight - lastSizeY / 2;
            }
            if (centerY - lastSizeY / 2 < 0)
            {
                centerY = lastSizeY / 2;
            }

            float deltaX = (centerX / videoWidth) - (normPos2.x + normPos1.x) / 2.0f;
            float deltaY = (centerY / videoHeight) - (normPos2.y + normPos1.y) / 2.0f;

            if (deltaX != 0.0f || deltaY != 0.0f)
            {
                normPos1.y += deltaY;
                normPos2.y += deltaY;
                normPos1.x += deltaX;
                normPos2.x += deltaX;

                changed = true;

                updatePluginROI();
            }
        }
    }

    ImGui::PopItemWidth();
}
