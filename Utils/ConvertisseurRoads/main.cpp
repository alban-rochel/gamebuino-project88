#include <QImage>
#include <fstream>
#include <iostream>

#define COLOR_R(r) ((uint16_t)(r & 0xF8))
#define COLOR_G_LOW(g) ((uint16_t)(g) << 13)
#define COLOR_G_HIGH(g) ((uint16_t)(g) >> 5)
#define COLOR_B(b) ((uint16_t)(b & 0xF8) << 5)

#define COLOR_565(r, g, b) (COLOR_R(r) | COLOR_G_LOW(g) | COLOR_G_HIGH(g) | COLOR_B(b))

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 128
#define SCREEN_SIZE_PIX (SCREEN_WIDTH*SCREEN_HEIGHT)

int main(int, char)
{
    QImage img("C:\\temp\\brown.jpg");
    if(img.isNull())
    {
        std::cout << "Failed opening" << std::endl;
        return 1;
    }

    uint16_t buf[SCREEN_SIZE_PIX];

    uint32_t offset = 0;

    for(unsigned int y = 0; y < SCREEN_HEIGHT; ++y)
    {
        for(unsigned int x = 0; x < SCREEN_WIDTH; ++x, ++offset)
        {
            QColor color = img.pixelColor(x, y);
            buf[offset] = COLOR_565(/*color.red(), color.green(), color.blue()*/255, 0, 0);
            //buf[offset] = COLOR_565(color.red(), color.green(), color.blue());
        }
    }

    std::ofstream outfile ("C:\\temp\\brown2.mph", std::ofstream::binary);

    outfile.write((const char*) buf, SCREEN_SIZE_PIX*2);

    outfile.close();

    return 0;
}
