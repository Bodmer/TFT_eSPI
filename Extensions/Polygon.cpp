//--------------------------------------------------------------------------------
//constexpr float PixelAlphaGain = 255.0;
constexpr float LoAlphaThreshold = 1.0 / 32.0;
constexpr float HiAlphaThreshold = 1.0 - LoAlphaThreshold;

//--------------------------------------------------------------------------------
// Calculates closest distance of point to a vector (with origin at 0,0)
float TFT_eSPI::calcDistance(tFPoint *aPoint, tFPoint *aVector) {
    float lH = fmaxf(fminf((aPoint->X * aVector->X + aPoint->Y * aVector->Y) / (aVector->X * aVector->X + aVector->Y * aVector->Y), 1.0f), 0.0f);
    float lDx = aPoint->X - (aVector->X * lH);
    float lDy = aPoint->Y - (aVector->Y * lH);
    return sqrtf(lDx * lDx + lDy * lDy);
}

//--------------------------------------------------------------------------------
// Calculates closest distance of point to a vector (with origin at 0,0)
// Result
// 0: No intersection
// 1: Intersection with ascending vector 
// -1: Intersection with descending vector 
int TFT_eSPI::checkIntersection(tFPoint *aPoint, tFPoint *aVector) {
    // Detect intersection with vector to -inf
    bool lInter = false;
    int lResult = 0;
    if ((aVector->Y != 0) && (((aPoint->Y >= 0) && (aPoint->Y <= aVector->Y)) || ((aPoint->Y <= 0) && (aPoint->Y >= aVector->Y)))) {
        if ((aVector->X != 0) && (aPoint->X != 0)) { 
            float lV = aVector->Y / aVector->X;
            float lP = aPoint->Y / aPoint->X;
            if (aPoint->X > 0)
                lInter = ((aVector->X < 0) || (fabsf(lP) <= fabsf(lV)));
            else
                lInter = ((aVector->X < 0) && (fabsf(lP) >= fabsf(lV)));
        } else 
            lInter = (aPoint->X >= aVector->X);
    }
    if (lInter) {
        lResult = (aVector->Y >= 0) ? 1 : -1;
    }
    // Serial.printf("%f, %f, %f, %f  lV= %f  lP= %f  res= %d\r\n", aPoint.X, aPoint.Y, aVector.X, aVector.Y, lV, lP,  lResult);
    return lResult;
}

//----------------------------------------------------
// in degrees (0..360)
void TFT_eSPI::rotatePoint(tFPoint *aPoint, float aAngle) {
    float lAngle = aAngle * PI / 180;
    float lX = (aPoint->X * cos(lAngle)) - (aPoint->Y * sin(lAngle));
    float lY = (aPoint->X * sin(lAngle)) + (aPoint->Y * cos(lAngle));
    aPoint->X = lX;
    aPoint->Y = lY;
}

//--------------------------------------------------------------------------------
// Rotation in degrees (0..360)
void TFT_eSPI::fillSmoothPolygon(const tFPoints aPoints, float aLineWidth, uint32_t aLineColor, 
        uint32_t aFillColor,
        const tFPoint aOffset, float aRotationAngle, const tFPoint aRotationPoint, 
        uint32_t aBackgroundColor) {

    tFPoints lPoints = aPoints;
    tFPoint *lP;

    // Rotation and movement
    for (int li=0; li < lPoints.size(); li++) {
        lP = &lPoints[li];
        // Rotate
        lP->X -= aRotationPoint.X;
        lP->Y -= aRotationPoint.Y;
        rotatePoint(lP, aRotationAngle);
        // Move
        lP->X += aOffset.X + aRotationPoint.X;
        lP->Y += aOffset.Y + aRotationPoint.Y;
    }

    // Find line bounding box
    tFPoint lMin, lMax;
    lMin = lMax = lPoints[0]; 
    for (int li=0; li < lPoints.size(); li++) {
        lP = &lPoints[li];
        lMin.X = fminf(lMin.X, lP->X);
        lMin.Y = fminf(lMin.Y, lP->Y);
        lMax.X = fmaxf(lMax.X, lP->X);
        lMax.Y = fmaxf(lMax.Y, lP->Y);
    }

    if (aLineWidth == 0) 
        aLineWidth = 1;
    
    int32_t lBb0X = (int32_t)floorf(lMin.X - aLineWidth);
    int32_t lBb0Y = (int32_t)floorf(lMin.Y - aLineWidth);
    int32_t lBb1X = (int32_t)ceilf(lMax.X + aLineWidth);
    int32_t lBb1Y = (int32_t)ceilf(lMax.Y + aLineWidth);

    if (!clipWindow(&lBb0X, &lBb0Y, &lBb1X, &lBb1Y))
        return;

    // Append first point to the end to close polygon
    lPoints.push_back(lPoints.at(0));

    uint32_t lBackCol;

    begin_nin_write();
    inTransaction = true;

    // Scan bounding box from lY0 down, calculate pixel intensity from distance to line
    std::vector<int> lIntersections;
    lIntersections.resize(lPoints.size()-1);

    for (int32_t lYp = lBb0Y; lYp <= lBb1Y; lYp++) {
        bool lNewWindow = true;  // Flag to start new window area

        for (int32_t lXp = lBb0X; lXp <= lBb1X; lXp++) {
            float lDistance = aLineWidth * 2; // Just to make sure it's max
            uint32_t lBackgroundTemp = aBackgroundColor;
            tFPoint *lPa = &lPoints[0];
            // save intersections in array
            int lIntersectionCount = 0;

            // loop all edges
            for (int li=0; li < lPoints.size()-1; li++) {
                lP = &lPoints[li + 1];
                tFPoint lPtemp = {lXp - lPa->X, lYp - lPa->Y};
                tFPoint lVtemp = {lP->X - lPa->X, lP->Y - lPa->Y};
                lDistance = fminf(lDistance, calcDistance(&lPtemp, &lVtemp));
                if (aFillColor != TFT_NO_COLOR) {
                    // Check intersections only for non-horizontal vectors
                    if (lVtemp.Y != 0) 
                        lIntersections[lIntersectionCount++] = checkIntersection(&lPtemp, &lVtemp);
                }
                lPa = lP;
            }
            if (aFillColor != TFT_NO_COLOR) {
                // check for odd intersections to determine "inside" points
                int lCount = 0;
                for (int li=0; li < lIntersectionCount; li++) {
                    if (lIntersections[li] != 0) {
                        if (lIntersections[li] != lIntersections[(li + lIntersectionCount - 1) % lIntersectionCount])
                            lCount++;
                    }
                }
                if ((lCount % 2) == 1)
                    lBackgroundTemp = aFillColor;
            }

            float lAlpha = aLineWidth - lDistance;
            uint32_t lColor;
            if (lAlpha > LoAlphaThreshold) {
                if (lAlpha <= HiAlphaThreshold) {
                    //Blend color with background and plot
                    if (lBackgroundTemp == TFT_NO_COLOR) {
                        lBackCol = readPixel(lXp, lYp);
                        lNewWindow = true;
                    } else
                        lBackCol = lBackgroundTemp;
                    lColor = alphaBlend((uint8_t)(lAlpha * PixelAlphaGain), aLineColor, lBackCol);
                } else
                    lColor = aLineColor;
            } else
                lColor = lBackgroundTemp;

            if (lColor != TFT_NO_COLOR) {
#ifdef GC9A01_DRIVER
                drawPixel(lXp, lYp, lColor);
#else
                if (lNewWindow) 
                    setWindow(lXp, lYp, lBb1X, lYp);
                lNewWindow = false;
                pushColor(lColor);
            } else
                lNewWindow = true;
#endif
        }
    }

    inTransaction = lockTransaction;
    end_nin_write();
}

//--------------------------------------------------------------------------------
// Rotation in degrees (0..360)
void TFT_eSPI::drawSmoothPolygon(const tFPoints aPoints, float aLineWidth, uint32_t aLineColor, 
        const tFPoint aOffset, float aRotationAngle, const tFPoint aRotationPoint, 
        uint32_t aBackgroundColor) {
    
    fillSmoothPolygon(aPoints, aLineWidth, aLineColor, TFT_NO_COLOR, aOffset, aRotationAngle, aRotationPoint, aBackgroundColor);
}
//--------------------------------------------------------------------------------
