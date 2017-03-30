#include "polymath.h"

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
    if (this->isSelfIntersecting(poly))
        return -1.0;

    double area = 0.0;
    for (int i = 0; i < poly.size(); i++)
    {
        area += poly.at(i).x()*poly.at(i+1).y() -
                poly.at(i).y()*poly.at(i+1).x();
    }
    area += poly.at(poly.size()).x()*poly.at(0).y() -
            poly.at(poly.size()).y()*poly.at(0).x();

    area = std::abs(area / 2);
    return area;
}

bool PolyMath::isSelfIntersecting(const QPolygonF &poly)
{
    int n = poly.size() - 1;
    QPolygonF vecs;
    for (int i = 0; i < n; i++)
        vecs.append(poly.at(i) - poly.at(i+1));

    vecs.append(poly.at(n) - poly.at(0));

    double sumAngle = 0.0;
    for (int i = 0; i < n; i++)
        sumAngle += this->angle(-vecs.at(i), vecs.at(i+1));

    sumAngle += this->angle(-vecs.at(n), vecs.at(0));

    return std::abs(sumAngle - M_PI*(n-1)) > 0.001;
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
    return this->area(this->cvToQPoly(poly));
}

bool PolyMath::cvIsSelfIntersecting(const std::vector<cv::Point2f> &poly)
{
    return this->isSelfIntersecting(this->cvToQPoly(poly));
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
