//--------------------------------------------------------------------------------
#define TFT_NO_COLOR 0x00FFFFFF

//--------------------------------------------------------------------------------
public:
    typedef struct {
        float X;
        float Y;
    } tFPoint;
    typedef std::vector<tFPoint> tFPoints;

    void rotatePoint(tFPoint *aPoint, float aAngle);
    void drawSmoothPolygon(tFPoints aPoints, float aLineWidth, uint32_t aLineColor, 
        tFPoint aOffset = {0, 0}, float aRotationAngle = 0.0, tFPoint aRotationPoint = {0, 0}, 
        uint32_t aBackgroundColor = TFT_NO_COLOR);
    void fillSmoothPolygon(tFPoints aPoints, float aLineWidth, uint32_t aLineColor, 
        uint32_t aFillColor = TFT_NO_COLOR,
        tFPoint aOffset = {0, 0}, float aRotationAngle = 0.0, tFPoint aRotationPoint = {0, 0}, 
        uint32_t aBackgroundColor = TFT_NO_COLOR);

private:
    float calcDistance(tFPoint *aPoint, tFPoint *aVector);
    int checkIntersection(tFPoint *aPoint, tFPoint *aVector);

//--------------------------------------------------------------------------------
