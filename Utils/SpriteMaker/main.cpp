#include <QImage>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

#define COLOR_R(r) ((uint16_t)(r & 0xF8))
#define COLOR_G_LOW(g) ((uint16_t)(g) << 13)
#define COLOR_G_HIGH(g) ((uint16_t)(g) >> 5)
#define COLOR_B(b) ((uint16_t)(b & 0xF8) << 5)

#define COLOR_565(r, g, b) (uint16_t)(COLOR_R(r) | COLOR_G_LOW(g) | COLOR_G_HIGH(g) | COLOR_B(b))

#define SPRITE_NAME "DOT"

int main(int, char)
{
    QImage img("C:\\Users\\Famille\\Documents\\Arduino\\Gamebuino\\Roads\\Assets\\dot.tif");
    if(img.isNull())
    {
        std::cout << "Failed opening" << std::endl;
        return 1;
    }

    FILE* out = fopen("C:\\temp\\dot.mph", "w");

    fprintf(out, "#define %s_WIDTH (%d)\n", SPRITE_NAME, img.width());
    fprintf(out, "#define %s_HEIGHT (%d)\n", SPRITE_NAME, img.height());

    fprintf(out, "const uint16_t %s[] = {", SPRITE_NAME);

    for(unsigned int y = 0; y < img.height(); ++y)
    {
        for(unsigned int x = 0; x < img.width(); ++x)
        {
            QColor color = img.pixelColor(x, y);
            fprintf(out, "0x%0.4x", COLOR_565(color.red(), color.green(), color.blue()));
            if(x == img.width() - 1 && y == img.height()-1)
            {
                fprintf(out, "};\n");
            }
            else
            {
                fprintf(out, ",\t");
                if(x == img.width() - 1)
                {
                    fprintf(out, "\n");
                }
            }
            //buf[offset] = COLOR_565(color.red(), color.green(), color.blue());
        }
    }

    fclose(out);

    return 0;
}
