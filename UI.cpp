#include "UI.h"

// --- NavBar ---
void NavBar::render(Adafruit_SSD1306 &disp, int width, int height)
{
    disp.drawLine(0, NAVBAR_HEIGHT, width, NAVBAR_HEIGHT, WHITE);
    disp.setTextSize(1);
    disp.setTextColor(WHITE);
    disp.setCursor(3, 3);
    disp.print(title);

    int x = width;
    for (auto icon : icons)
    {
        x -= (icon->w + 2);
        disp.drawBitmap(x, 2, icon->bitmap, icon->w, icon->h, WHITE);
    }
}

void NavBar::setIcon(Icon *icon)
{
    for (auto &ic : icons)
    {
        if (ic->label == icon->label)
        {
            ic->bitmap = icon->bitmap;
            ic->w = icon->w;
            ic->h = icon->h;
            return;
        }
    }
    icons.push_back(icon);
}

void NavBar::removeIcon(const String &id)
{
    for (auto it = icons.begin(); it != icons.end(); ++it)
    {
        if ((*it)->label == id)
        {
            icons.erase(it);
            break;
        }
    }
}

// --- ListBody ---
void ListBody::render(Adafruit_SSD1306 &disp, int x, int y, int w, int h)
{
    int itemHeight = 11;

    // Ajustar topIndex según selección
    if (currentIndex < topIndex)
        topIndex = currentIndex;
    else if (currentIndex >= topIndex + maxVisible)
        topIndex = currentIndex - maxVisible + 1;

    // Calcular desplazamiento deseado
    targetOffset = -topIndex * itemHeight;

    // Animar hacia desplazamiento deseado
    if (fabs(targetOffset - scrollOffset) > 0.5f)
    {
        scrollOffset += (targetOffset - scrollOffset) * velScroll;
    }
    else
    {
        scrollOffset = targetOffset;
    }

    // Limitar scrollOffset para no salirse del área visible
    int minOffset = h - items.size() * itemHeight;
    if (minOffset > 0)
        minOffset = 0; // si hay pocos items, no scroll
    if (scrollOffset < minOffset)
        scrollOffset = minOffset;
    if (scrollOffset > 0)
        scrollOffset = 0;

    int targetPosY = 0;

    for (int i = 0; i < items.size(); i++)
    {
        int yy = y + i * itemHeight + scrollOffset;

        // No dibujar si está fuera del área visible
        if (yy + itemHeight < y || yy > y + h)
            continue;

        int cursorX = x + 2;
        if (items[i].icon)
        {
            disp.drawBitmap(cursorX, yy + 1, items[i].icon->bitmap, items[i].icon->w, items[i].icon->h, WHITE);
            cursorX += items[i].icon->w + 2;
        }

        String label = items[i].label;
        int maxChars = (w - (cursorX + 2)) / 6; // 6 px por char en textSize(1)
        if (label.length() > maxChars)
        {
            label = label.substring(0, maxChars - 1) + "…"; // agrega puntos suspensivos
        }

        disp.setCursor(cursorX, yy + 2);
        disp.setTextSize(1);
        disp.setTextColor(WHITE);
        disp.print(label);

        if (i == currentIndex)
            targetPosY = yy;
    }

    // Highlight animado
    if (fabs(targetPosY - currentPosY) > 3)
        currentPosY += (targetPosY - currentPosY) * velScroll;
    else
        currentPosY = targetPosY;

    // Limitar currentPosY antes de invertir
    if (currentPosY < y)
        currentPosY = y;
    if (currentPosY + itemHeight > y + h)
        currentPosY = y + h - itemHeight;

    invertRect(disp, x + 1, currentPosY, w - 5, itemHeight);

    if (items.size() > maxVisible)
    {                                                    // solo si hay más ítems que los visibles
        int barHeight = (maxVisible * h) / items.size(); // tamaño proporcional
        int barY = y + (topIndex * h) / items.size();    // posición proporcional
        int barX = x + w - 3;                            // a la derecha del área de la lista
        disp.fillRect(barX, barY, 2, barHeight, WHITE);
    }
}

void ListBody::handleInput(UIAction a)
{
    switch (a)
    {
    case ACTION_UP:
        up();
        break;
    case ACTION_DOWN:
        down();
        break;
    case ACTION_ENTER:
        if (currentIndex >= 0 && currentIndex < items.size())
        {
            if (items[currentIndex].callback)
            {
                items[currentIndex].callback();
            }
        }
        break;
    case ACTION_RIGHT:
        if (currentIndex >= 0 && currentIndex < items.size())
        {
            if (items[currentIndex].callback)
            {
                items[currentIndex].callback();
            }
        }
        break;
    default:
        break;
    }
}

void ListBody::down()
{
    currentIndex++;
    if (currentIndex >= items.size())
    {
        currentIndex = 0;
    }
}

void ListBody::up()
{
    currentIndex--;
    if (currentIndex < 0)
    {
        currentIndex = items.size() - 1;
    }
}

// --- WindowManager ---
void WindowManager::push(Body *b)
{
    stack.push_back(b);
}

void WindowManager::pop()
{
    if (!stack.empty())
    {
        stack.pop_back();
    }
}

Body *WindowManager::current()
{
    if (stack.empty())
        return nullptr;
    return stack.back();
}

bool WindowManager::canGoBack()
{
    // Solo hay "back" si hay al menos 2 elementos:
    // root + uno más
    return stack.size() > 1;
}

void WindowManager::render(Adafruit_SSD1306 &disp)
{
    disp.clearDisplay();

    Body *b = current();
    if (b)
    {
        b->render(disp, 0, NAVBAR_HEIGHT, disp.width(), disp.height() - NAVBAR_HEIGHT);
    }

    // Fondo del navbar
    disp.fillRect(0, 0, disp.width(), NAVBAR_HEIGHT, BLACK);

    // Borde de la pantalla
    disp.drawRoundRect(0, 0, disp.width(), disp.height(), 4, WHITE);

    // Renderizar la navbar
    navbar.render(disp, disp.width(), disp.height());

    disp.display();
}

// --- invertRectFast ---
void invertRect(Adafruit_SSD1306 &disp, int16_t x, int16_t y, int16_t w, int16_t h)
{
    uint8_t *buf = disp.getBuffer();
    int16_t width = disp.width();

    for (int16_t j = y; j < y + h; j++)
    {
        for (int16_t i = x; i < x + w; i++)
        {
            int16_t byteIndex = i + (j / 8) * width;
            uint8_t bitMask = 1 << (j & 7);
            buf[byteIndex] ^= bitMask; // XOR → invierte el bit
        }
    }
}
