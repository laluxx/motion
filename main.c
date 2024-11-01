#include <lume.h>
#include <math.h>
#include "theme.h"

typedef struct {
    float x, y, width, height;
    bool selected;
} Rectangle;

#define MAX_RECTANGLES 100
#define SNAP_THRESHOLD 10.0f  // pixels

Rectangle rectangles[MAX_RECTANGLES];
int rectangleCount = 0;
bool isDragging = false, isSelecting = false;
float startX, startY, offsetX, offsetY;
int selectedIndex = -1;
int layerIndices[MAX_RECTANGLES];  // To track the drawing order

void keyHandler(int key, int action, int mods);
void updateDragging(double mouseX, double mouseY);

int main(void) {
    int sw = 1920;
    int sh = 1080;

    initWindow(sw, sh, "Lume - Adit");
    registerKeyCallback(keyHandler);
    initThemes();

    while (!windowShouldClose()) {
        sw = getScreenWidth();
        sh = getScreenHeight();

        beginDrawing();
        clearBackground(CT.bg);

        double mouseX, mouseY;
        getCursorPosition(&mouseX, &mouseY);

        if (isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            bool found = false;
            for (int i = rectangleCount - 1; i >= 0; i--) {
                int idx = layerIndices[i];
                Rectangle *r = &rectangles[idx];
                if (mouseX >= r->x && mouseX <= r->x + r->width && mouseY >= r->y && mouseY <= r->y + r->height) {
                    if (selectedIndex != idx) {
                        if (selectedIndex != -1) {
                            rectangles[selectedIndex].selected = false;
                        }
                        selectedIndex = idx;
                        r->selected = true;
                        for (int j = i; j < rectangleCount - 1; j++) {
                            layerIndices[j] = layerIndices[j + 1];
                        }
                        layerIndices[rectangleCount - 1] = idx;
                    }
                    offsetX = mouseX - r->x;
                    offsetY = mouseY - r->y;
                    isDragging = true;
                    found = true;
                    break;
                }
            }
            if (!found) {
                if (selectedIndex != -1) {
                    rectangles[selectedIndex].selected = false;
                    selectedIndex = -1;
                }
                startX = (float)mouseX;
                startY = (float)mouseY;
                isSelecting = true;
            }
        }

        if (isMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            if (isSelecting && rectangleCount < MAX_RECTANGLES && selectedIndex == -1) {
                float width = (float)mouseX - startX;
                float height = (float)mouseY - startY;
                if (width != 0 && height != 0) {
                    if (width < 0) {
                        startX += width;
                        width = -width;
                    }
                    if (height < 0) {
                        startY += height;
                        height = -height;
                    }
                    rectangles[rectangleCount] = (Rectangle){startX, startY, width, height, false};
                    layerIndices[rectangleCount] = rectangleCount;
                    rectangleCount++;
                }
            }
            isSelecting = false;
            isDragging = false;
        }

        if (isDragging) {
            updateDragging(mouseX, mouseY);
        }

        useShader("simple");
        for (int i = 0; i < rectangleCount; i++) {
            int idx = layerIndices[i];
            Rectangle r = rectangles[idx];
            drawRectangle((Vec2f){r.x, r.y}, (Vec2f){r.width, r.height}, CT.text);
            if (r.selected) {
                drawRectangleLines((Vec2f){r.x, r.y}, (Vec2f){r.width, r.height}, CT.cursor, 2.0);
            }
        }

        if (isSelecting) {
            drawRectangleLines((Vec2f){startX, startY}, (Vec2f){(float)mouseX - startX, (float)mouseY - startY}, (Color){0.5, 0.5, 0.5, 1.0}, 1.0);
        }

        flush();
        endDrawing();
    }

    closeWindow();
    return 0;
}

void keyHandler(int key, int action, int mods) {

    bool shiftPressed = mods & GLFW_MOD_SHIFT;
    bool ctrlPressed = mods & GLFW_MOD_CONTROL;
    bool altPressed = mods & GLFW_MOD_ALT;

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
        case KEY_D:
            if (selectedIndex != 1) {
                rectangles[selectedIndex].selected = false;
                for (int i = selectedIndex; i < rectangleCount - 1; i++) {
                    rectangles[i] = rectangles[i + 1];
                    layerIndices[i] = layerIndices[i + 1];
                }
                rectangleCount--;
                selectedIndex = -1;
            }
            break;
        case KEY_EQUAL:
            if (altPressed) nextTheme();
            break;
        case KEY_MINUS:
            if (altPressed) previousTheme();
            break;
        }
    }
}


void updateDragging(double mouseX, double mouseY) {
    Rectangle *current = &rectangles[selectedIndex];
    float newX = mouseX - offsetX;
    float newY = mouseY - offsetY;
    
    // Check if there is a significant position change
    if (current->x == newX && current->y == newY) {
        return; // No movement, don't proceed with snapping logic
    }

    float snapX = newX, snapY = newY;
    float screenW = getScreenWidth();
    float screenH = getScreenHeight();

    // Check for potential snapping with other rectangles
    for (int i = 0; i < rectangleCount; i++) {
        if (i == selectedIndex) continue;  // Skip the rectangle itself

        Rectangle *other = &rectangles[i];
        // Check for snapping on the Y-axis (horizontal alignment)
        float edgesCurrent[] = {newY, newY + current->height};
        float edgesOther[] = {other->y, other->y + other->height};

        for (int ci = 0; ci < 2; ci++) {
            for (int oi = 0; oi < 2; oi++) {
                if (fabsf(edgesCurrent[ci] - edgesOther[oi]) < SNAP_THRESHOLD) {
                    snapY = edgesOther[oi] - (edgesCurrent[ci] - newY);
                    float drawY = snapY + (ci == 0 ? 0 : current->height);  // Adjust drawY based on which edge is snapping
                    drawLine((Vec2f){0, drawY}, (Vec2f){screenW, drawY}, CT.cursor, 1.0);
                }
            }
        }

        // Check for snapping on the X-axis (vertical alignment)
        float edgesCurrentX[] = {newX, newX + current->width};
        float edgesOtherX[] = {other->x, other->x + other->width};

        for (int ci = 0; ci < 2; ci++) {
            for (int oi = 0; oi < 2; oi++) {
                if (fabsf(edgesCurrentX[ci] - edgesOtherX[oi]) < SNAP_THRESHOLD) {
                    snapX = edgesOtherX[oi] - (edgesCurrentX[ci] - newX);
                    float drawX = snapX + (ci == 0 ? 0 : current->width);  // Adjust drawX based on which edge is snapping
                    drawLine((Vec2f){drawX, 0}, (Vec2f){drawX, screenH}, CT.cursor, 1.0);
                }
            }
        }
    }

    // Update rectangle position to the snapped position
    current->x = snapX;
    current->y = snapY;
}

