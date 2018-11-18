#include <QImage>
#include <QMap>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

#define COLOR_R(r) ((uint16_t)(r & 0xF8))
#define COLOR_G_LOW(g) ((uint16_t)(g) << 13)
#define COLOR_G_HIGH(g) ((uint16_t)(g) >> 5)
#define COLOR_B(b) ((uint16_t)(b & 0xF8) << 5)

#define COLOR_565(r, g, b) (uint16_t)(COLOR_R(r) | COLOR_G_LOW(g) | COLOR_G_HIGH(g) | COLOR_B(b))

#define BG_NAME "BACKGROUND_ARIZONA"

int colorIndex(const QVector<QColor> palette, const QColor& color)
{
    for(unsigned int ii = 0; ii < palette.size(); ++ii)
    {
        if(palette[ii] == color)
        {
            return ii;
        }
    }
    return -1;
}

int main(int, char)
{
    QImage img("C:\\temp\\arizona.tif");
    if(img.isNull())
    {
        std::cout << "Failed opening" << std::endl;
        return 1;
    }

    FILE* out = fopen("C:\\temp\\arizona.mph", "w");

    // Compute palette, check that it is 16 colors max
    QVector<QColor> palette;

    for(unsigned int y = 0; y < img.height(); ++y)
    {
        for(unsigned int x = 0; x < img.width(); ++x)
        {
            QColor color = img.pixelColor(x, y);
            if(colorIndex(palette, color) < 0)
            {
                palette.push_back(color);
            }
        }
    }
    std::cout << "Palette size " << palette.size() << std::endl;

    fprintf(out, "const uint16_t %s_PALETTE[] = {", BG_NAME);
    for(unsigned int ii = 0; ii < palette.size(); ++ii)
    {
        const QColor& color = palette[ii];
        if(ii != 0)
        {
            fprintf(out, ",\t");
        }
        fprintf(out, "0x%0.4x", COLOR_565(color.red(), color.green(), color.blue()));
    }
    fprintf(out, "};\n\n");

    fprintf(out, "#define %s_WIDTH (%d)\n", BG_NAME, img.width());
    fprintf(out, "#define %s_HEIGHT (%d)\n", BG_NAME, img.height());

    fprintf(out, "const uint8_t %s[] = {", BG_NAME);

    for(unsigned int y = 0; y < img.height(); ++y)
    {
        for(unsigned int x = 0; x < img.width(); x+=2)
        {
            QColor color1 = img.pixelColor(x, y);
            QColor color2 = img.pixelColor(x+1, y);
            uint8_t index1 = colorIndex(palette, color1);
            uint8_t index2 = colorIndex(palette, color2);
            fprintf(out, "%d", (uint8_t)((index1 << 4) | (index2 & 0x0F)));
            if(x == img.width() - 1 && y == img.height()-1)
            {
                fprintf(out, "};\n");
            }
            else
            {
                fprintf(out, ",\t");
                if(x == img.width() - 2)
                {
                    fprintf(out, "\n");
                }
            }
        }
    }

    fclose(out);

    return 0;
}
