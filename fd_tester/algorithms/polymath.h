#ifndef POLYMATH_H
#define POLYMATH_H

#include <opencv2/core/core.hpp>

#include <QPointF>
#include <QPolygonF>

class PolyMath
{
public:
    static PolyMath* instance();
    static void destroy();

    double area(const QPolygonF &poly);
    bool isSelfIntersecting(const QPolygonF &poly);
    double angle(const QPointF &vec1, const QPointF &vec2);

    double cvArea(const std::vector<cv::Point2f> &poly);
    bool cvIsSelfIntersecting(const std::vector<cv::Point2f> &poly);
    double cvAngle(const cv::Point2f &vec1, const cv::Point2f &vec2);

    QPolygonF cvToQPoly(const std::vector<cv::Point2f> &vec);

private:
    PolyMath();
    ~PolyMath();
};

#endif // POLYMATH_H
