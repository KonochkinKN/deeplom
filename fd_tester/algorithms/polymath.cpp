#include "polymath.h"
#include <QDebug>

static PolyMath *math = nullptr;

PolyMath* PolyMath::instance()
{
    if (!math)
        math = new PolyMath();

    return math;
}

void PolyMath::destroy()
{
    delete math;
    math = nullptr;
}

PolyMath::PolyMath()
{

}

PolyMath::~PolyMath()
{

}

double PolyMath::area(const QPolygonF &poly)
{
    double area = 0.0;
    for (int i = 0; i < poly.size(); i++)
    {
        area += poly.at(i).x()*poly.at((i+1)%poly.size()).y() -
                poly.at(i).y()*poly.at((i+1)%poly.size()).x();
    }

    area = std::abs(area / 2);
    return area;
}

//! @brief return true if polygon is convex
//! @ref http://dxdy.ru/topic14153.html
bool PolyMath::isConvex(const QPolygonF &poly)
{
    int s = poly.size();
    if (s < 4)
        return true;

    int a = 0;
    for (int i = 0; i < s; i++)
    {
        QPointF side1 = poly.at(i) - poly.at((i-1+s)%s);
        QPointF side2 = poly.at(i) - poly.at((i+1)%s);

        double v = side1.x()*side2.y() - side1.y()*side2.y();
        a += (v >= 0) ? 1 : -1;
    }

    return std::abs(a) == s;
}

bool PolyMath::isCorrect(const QPolygonF &poly)
{
    return this->isConvex(poly) && !this->isSelfIntersecting(poly);
}

/*! @brief Counts center of mass of polygon *
 * @param QPolygonF poly - polygon
 * @return QPointF - center of mass *
 * @details Parse a polygon into triangles.
 * Count their centers of mass. Calculate
 * the center of mass of the original figure
 * according to the formula:
 * Xc = sum (Xci*Si) / S,
 * Yc = sum (Yci*Si) / S;
 *
 * @ref http://algolist.manual.ru/maths/geom/polygon/center_mass.php
 */
QPointF PolyMath::massCenter(const QPolygonF &poly)
{
    if (poly.size() == 0)
        return QPointF();

    if (poly.size() == 1)
        return poly.at(0);

    if (poly.size() == 2)
        return (poly.at(0) + poly.at(1))/2;

    double S = 0.0;
    int n = poly.size();
    QPointF c = {0.0, 0.0};
    QPointF m = this->pointSumAvg(poly);
    for (int i = 0; i < n; i++)
    {
        QPolygonF curTriangle({m, poly.at(i), poly.at((i+1)%n)});
        double Si = this->area(curTriangle);
        c += Si * this->pointSumAvg(curTriangle);
        S += Si;
    }
    c /= S;

    return c;
}

QPointF PolyMath::pointSumAvg(const QPolygonF &poly)
{
    QPointF s = {0.0, 0.0};

    foreach (QPointF pt, poly)
        s += pt;

    s /= poly.size();

    return s;
}

bool PolyMath::isSelfIntersecting(const QPolygonF &poly)
{
    if (poly.size() < 4)
        return false;

    QPolygonF vecs;
    int n = poly.size();
    for (int i = 0; i < n; i++)
        vecs.append(poly.at(i) - poly.at((i+1)%n));

    double sumAngle = 0.0;
    for (int i = 0; i < n; i++)
        sumAngle += this->angle(-vecs.at(i), vecs.at((i+1)%n));

    return std::abs(sumAngle - M_PI*(n-2)) > 0.001;
}

double PolyMath::angle(const QPointF &vec1, const QPointF &vec2)
{
    if (vec1.isNull() || vec2.isNull())
        return 0.0;

    double cos = (QPointF::dotProduct(vec1, vec2)) /
            (std::sqrt(QPointF::dotProduct(vec1, vec1)) *
             std::sqrt(QPointF::dotProduct(vec2, vec2)));

    return std::acos(cos);
}

double PolyMath::cvArea(const std::vector<cv::Point2f> &poly)
{
    double area = 0.0;
    for (uint i = 0; i < poly.size(); i++)
    {
        area += poly.at(i).x*poly.at((i+1)%poly.size()).y -
                poly.at(i).y*poly.at((i+1)%poly.size()).x;
    }

    area = std::abs(area / 2);
    return area;
}

//! @brief return true if polygon is convex
//! @ref http://dxdy.ru/topic14153.html
bool PolyMath::cvIsConvex(const std::vector<cv::Point2f> &poly)
{
    int s = (int)poly.size();
    if (s < 4)
        return true;

    int a = 0;
    for (int i = 0; i < s; i++)
    {
        cv::Point2f side1 = poly.at(i) - poly.at((i-1+s)%s);
        cv::Point2f side2 = poly.at(i) - poly.at((i+1)%s);

        double v = side1.x*side2.y - side1.y*side2.y;
        a += (v >= 0) ? 1 : -1;
    }

    return std::abs(a) == s;
}

bool PolyMath::cvIsCorrect(const std::vector<cv::Point2f> &poly)
{
    return this->cvIsConvex(poly) && !this->cvIsSelfIntersecting(poly);
}

bool PolyMath::cvIsSelfIntersecting(const std::vector<cv::Point2f> &poly)
{
    if (poly.size() < 4)
        return false;

    int n = poly.size();
    std::vector<cv::Point2f> vecs;
    for (int i = 0; i < n; i++)
        vecs.push_back(poly.at(i) - poly.at((i+1)%n));

    double sumAngle = 0.0;
    for (int i = 0; i < n; i++)
        sumAngle += this->cvAngle(-vecs.at(i), vecs.at((i+1)%n));

    return std::abs(sumAngle - M_PI*(n-2)) > 0.001;
}

double PolyMath::cvAngle(const cv::Point2f &vec1, const cv::Point2f &vec2)
{
    double n1 = vec1.ddot(vec1);
    double n2 = vec2.ddot(vec2);
    if (!(n1 * n2))
        return 0.0;

    double cos = vec1.ddot(vec2) / (std::sqrt(n1) * std::sqrt(n2));

    return std::acos(cos);
}

QPolygonF PolyMath::cvToQPoly(const std::vector<cv::Point2f> &vec)
{
    QPolygonF poly;
    foreach (cv::Point2f pt, vec)
        poly.append(QPointF(pt.x, pt.y));

    return poly;
}
