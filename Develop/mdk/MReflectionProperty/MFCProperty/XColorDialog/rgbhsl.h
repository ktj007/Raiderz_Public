#ifndef RGBHSL_H
#define RGBHSL_H

void RGBtoHSL(COLORREF cr, double *H, double *S, double *L);
void RGBtoHSL(COLORREF cr, BYTE *h, BYTE *s, BYTE *l);
COLORREF HSLtoRGB(double H, double S, double L);

#endif //RGBHSL_H
