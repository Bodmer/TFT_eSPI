// Anti-aliased functions - for test purposes only
// TODO: optimise and incorporate in TFT_eSPI

// Support
#define iround(X) ((uint16_t)(((float)(X))+0.5))

// Alphablend specified color with sprite background pixel and plot in sprite at x,y
void plotPixel(int16_t x, int16_t y, uint16_t color, float alpha) {
  if (alpha > 0.996) { face.drawPixel(x, y, color); return; }
  alpha = alpha * ALPHA_GAIN;
  // Clip alpha
  if (alpha > 255) alpha = 255;
  //Blend color with background and plot
  face.drawPixel(x, y, face.alphaBlend((uint8_t)alpha, color, face.readPixel(x, y)));
}

// AA circle outline
void drawCircleAA( int16_t x, int16_t y, int16_t radius, uint16_t color )  {
  drawRoundRectangleAntialiased(x - radius, y - radius, radius << 1, radius << 1, radius, radius, color);
}

// AA filled circle
void fillCircleAA( int16_t x, int16_t y, int16_t radius, uint16_t color )  {
  drawRoundRectangleAntialiased(x - radius, y - radius, radius << 1, radius << 1, radius, radius, color);
  face.fillCircle(x, y, radius, color);
}

// AA circle and rounded rectangle support
void drawRoundRectangleAntialiased(int16_t x, int16_t y, int16_t width, int16_t height, int16_t rx, int16_t ry, uint16_t color) {

  int16_t i;
  int32_t a2, b2, ds, dt, dxt, t, s, d;
  int16_t xp, yp, xs, ys, dyt, od, xx, yy, xc2, yc2;
  float cp;
  float sab;
  float weight, iweight;

  if ((rx < 0) || (ry < 0)) {
    return;
  }

  // Temporary - should really draw a box if corner radius is zero...
  if (rx == 0) {
    face.drawFastVLine(x, y - ry, y + ry, color);
    return;
  }

  if (ry == 0) {
    face.drawFastHLine(x - rx, y, x + rx, color);
    return;
  }

  a2 = rx * rx;
  b2 = ry * ry;

  ds = a2 << 1;
  dt = b2 << 1;

  xc2 = x << 1;
  yc2 = y << 1;

  sab = sqrt((float)(a2 + b2));
  od = (int)iround(sab * 0.01) + 1;
  dxt = (int)iround((float)a2 / sab) + od;

  t = 0;
  s = -2 * a2 * ry;
  d = 0;

  xp = x + rx;
  yp = y;

  // Sides - may need to reduce color brightness to visually match anti-aliased corners
  face.drawFastHLine(x + rx, y + height, 2 * rx - width + 1, color);
  face.drawFastHLine(x + rx, y, 2 * rx - width + 1, color);
  face.drawFastVLine(x + width, y + ry, 2 * ry - height + 1, color);
  face.drawFastVLine(x, y + ry, 2 * ry - height + 1, color);

  for (i = 1; i <= dxt; i++) {
    xp--;
    d += t - b2;

    if (d >= 0) {
      ys = yp - 1;
    } else if ((d - s - a2) > 0) {
      if (((d << 1) - s - a2) >= 0) {
        ys = yp + 1;
      } else {
        ys = yp;
        yp++;
        d -= s + a2;
        s += ds;
      }
    } else {
      yp++;
      ys = yp + 1;
      d -= s + a2;
      s += ds;
    }

    t -= dt;

    if (s != 0) {
      cp = (float) abs(d) / (float) abs(s);
      if (cp > 1.0) {
        cp = 1.0f;
      }
    } else {
      cp = 1.0f;
    }

    weight = cp;
    iweight = 1 - weight;


    xx = xc2 - xp;
    plotPixel(xp, yp, color, iweight);
    plotPixel(xx + width, yp, color, iweight);

    plotPixel(xp, ys, color, weight);
    plotPixel(xx + width, ys, color, weight);

    yy = yc2 - yp;
    plotPixel(xp, yy + height, color, iweight);
    plotPixel(xx + width, yy + height, color, iweight);

    yy = yc2 - ys;
    plotPixel(xp, yy + height, color, weight);
    plotPixel(xx + width, yy + height, color, weight);
  }

  dyt = (int)iround((float)b2 / sab ) + od;

  for (i = 1; i <= dyt; i++) {
    yp++;
    d -= s + a2;

    if (d <= 0) {
      xs = xp + 1;
    } else if ((d + t - b2) < 0) {
      if (((d << 1) + t - b2) <= 0) {
        xs = xp - 1;
      } else {
        xs = xp;
        xp--;
        d += t - b2;
        t -= dt;
      }
    } else {
      xp--;
      xs = xp - 1;
      d += t - b2;
      t -= dt;
    }

    s += ds;

    if (t != 0) {
      cp = (float) abs(d) / (float) abs(t);
      if (cp > 1.0) {
        cp = 1.0f;
      }
    } else {
      cp = 1.0f;
    }

    weight = cp;
    iweight = 1 - weight;

    xx = xc2 - xp;
    yy = yc2 - yp;
    plotPixel(xp, yp, color, iweight);
    plotPixel(xx + width, yp, color, iweight);

    plotPixel(xp, yy + height, color, iweight);
    plotPixel(xx + width, yy + height, color, iweight);

    xx = xc2 - xs;
    plotPixel(xs, yp, color, weight);
    plotPixel(xx + width, yp, color, weight);

    plotPixel(xs, yy + height, color, weight);
    plotPixel(xx + width, yy + height, color, weight);
  }
}
