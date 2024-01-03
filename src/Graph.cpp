#include "vex.h"
#include <utility>
#include <deque>
#include <string>
#include <map>
#include <vector>

#include "Graph.h"

using std::deque;
using std::make_pair;
using std::map;
using std::pair;
using std::string;
using std::tuple;
using std::vector;

using Point = pair<float, float>;
using Variable = tuple<deque<Point>, color, Point, bool>;

// double x;
// double y;
// double z;

Vector3::Vector3() : x(0), y(0), z(0) {}
Vector3::Vector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}

Vector3 Vector3::operator+(Vector3 other)
{
  return Vector3(x + other.x, y + other.y, z + other.z);
}
Vector3 Vector3::operator-(Vector3 other)
{
  return Vector3(x - other.x, y - other.y, z - other.z);
}
Vector3 Vector3::operator-()
{
  return Vector3(-x, -y, -z);
}
void Vector3::operator+=(Vector3 other)
{
  x += other.x, y += other.y, z += other.z;
}
void Vector3::operator-=(Vector3 other)
{
  x -= other.x, y -= other.y, z -= other.z;
}
Vector3 Vector3::operator*(double m)
{
  return Vector3(x * m, y * m, z * m);
}

using namespace vex;

template <typename T>
T clamp(T value, T minValue, T maxValue)
{
  return max(minValue, min(maxValue, value));
}
/**
 * @brief min and max are inclusive
 */
template <typename T>
bool threshold(T value, T minValue, T maxValue)
{
  return value >= minValue && value <= maxValue;
}

vex::brain::lcd s = Brain.Screen;

// no args
Graph::Graph() : topLeft(make_pair(5, 5)),
                 width(470),
                 height(230),
                 xBounds(make_pair(-100.0, 100.0)),
                 yBounds(make_pair(-100.0, 100.0)),
                 xRange(200.0),
                 yRange(200.0) {}

// graph position args
Graph::Graph(pair<int, int> topLeft, int width, int height) : topLeft(topLeft),
                                                              width(width),
                                                              height(height),
                                                              xBounds(make_pair(-100.0, 100.0)),
                                                              yBounds(make_pair(-100.0, 100.0)),
                                                              xRange(200.0),
                                                              yRange(200.0) {}

// graph bounds args
Graph::Graph(pair<double, double> xBounds, pair<double, double> yBounds) : topLeft(make_pair(5, 5)),
                                                                           width(470),
                                                                           height(230),
                                                                           xBounds(xBounds),
                                                                           yBounds(yBounds),
                                                                           xRange(xBounds.second - xBounds.first),
                                                                           yRange(yBounds.second - yBounds.first) {}

// graph position and bounds args
Graph::Graph(pair<int, int> topLeft, int width, int height, pair<double, double> xBounds, pair<double, double> yBounds) : topLeft(topLeft),
                                                                                                                          width(width),
                                                                                                                          height(height),
                                                                                                                          xBounds(xBounds),
                                                                                                                          yBounds(yBounds),
                                                                                                                          xRange(xBounds.second - xBounds.first),
                                                                                                                          yRange(yBounds.second - yBounds.first) {}

//* plotting functions
void Graph::drawGraph()
{
  s.setClipRegion(topLeft.first, topLeft.second, width + 1, height + 1);
  previousPointExists = false;
  for (pair<string, Variable> entry : variables)
  {
    std::get<3>(entry.second) = false;
    auto entry2 = variables.find(entry.first);
    entry2->second = entry.second;
  }
  setColor(color().rgb(50, 50, 50));
  s.setFillColor(color().rgb(25, 25, 25));
  s.drawRectangle(topLeft.first, topLeft.second, width + 1, height + 1);
  setColor(color().rgb(100, 100, 100));
  if (xBounds.first <= 0 && 0 <= xBounds.second)
    plotLine(make_pair(0.0, yBounds.first), make_pair(0.0, yBounds.second));
  if (yBounds.first <= 0 && 0 <= yBounds.second)
    plotLine(make_pair(xBounds.first, 0.0), make_pair(xBounds.second, 0.0));
  setColor(color().white);

  for (pair<Point, color> p : points)
  {
    s.setPenColor(p.second);
    plotPoint(p.first);
  }
  for (pair<Point, color> p : segments)
  {
    s.setPenColor(p.second);
    plotSegment(p.first);
  }
  for (pair<string, Variable> entry : variables)
  {
    Variable variable = entry.second;
    setColor(std::get<1>(variable));
    for (Point point : std::get<0>(variable))
    {
      if (std::get<3>(variable) == false)
      {
        previousPointExists = false;
        std::get<2>(variable) = point;
        std::get<3>(variable) = true;
      }
      else
      {
        previousPoint = std::get<2>(variable);
        previousPointExists = true;
      }
      plotSegment(point);
      std::get<2>(variable) = point;
    }
    auto entry2 = variables.find(entry.first);
    entry2->second = variable;
  }
  s.render();
  s.setClipRegion(0, 0, 480, 240);
  s.setPenColor(color().white);
  s.setFillColor(color().transparent);
}

void Graph::setColor(vex::color c)
{
  Brain.Screen.setPenColor(c);
}

void Graph::plotPoint(float x, float y)
{
  plotPoint(Point(x, y));
}

void Graph::plotPoint(Point point)
{
  // if (!isInBounds(point)) return;
  Point newPoint = calculatePoint(point);
  Brain.Screen.drawPixel(newPoint.first, newPoint.second);
}

void Graph::plotSegment(float x, float y)
{
  plotSegment(Point(x, y));
}

void Graph::plotSegment(Point point)
{
  // if (!isInBounds(point)) return;
  plotPoint(point);
  if (previousPointExists)
  {
    plotLine(previousPoint, point);
  }
  else
    previousPointExists = true;
  previousPoint = point;
}

void Graph::plotLine(Point pointA, Point pointB)
{
  // if (isInBounds(pointA) && isInBounds(pointB))
  // {
  Point newPointA = calculatePoint(pointA);
  Point newPointB = calculatePoint(pointB);
  Brain.Screen.drawLine(newPointA.first, newPointA.second, newPointB.first, newPointB.second);
  // }
}

void Graph::addPoint(float x, float y)
{
  addPoint(Point(x, y), color().white);
}
void Graph::addPoint(Point point)
{
  addPoint(point, color().white);
}
void Graph::addPoint(float x, float y, color c)
{
  addPoint(Point(x, y), c);
}
void Graph::addPoint(Point point, color c)
{
  s.setPenColor(c);
  plotPoint(point);
  points.push_back(make_pair(point, c));
}

void Graph::addSegment(float x, float y)
{
  addSegment(Point(x, y), color().white);
}
void Graph::addSegment(Point point)
{
  addSegment(point, color().white);
}
void Graph::addSegment(float x, float y, color c)
{
  addSegment(Point(x, y), c);
}
void Graph::addSegment(Point point, color c)
{
  s.setPenColor(c);
  plotSegment(point);
  segments.push_back(make_pair(point, c));
}

void Graph::addVariable(string name, color col)
{
  auto existing = variables.find(name);
  if (existing == variables.end())
    variables.insert(make_pair(name, Variable(deque<Point>(), col, Point(0, 0), false)));
}
void Graph::addSegmentToVariable(string varName, Point point)
{
  auto existing = variables.find(varName);
  if (existing == variables.end())
    return;

  Variable variable = existing->second;
  std::get<0>(variable).push_back(point);
  setColor(std::get<1>(variable));
  if (std::get<3>(variable) == false)
  {
    previousPointExists = false;
    std::get<2>(variable) = point;
    std::get<3>(variable) = true;
  }
  else
  {
    previousPoint = std::get<2>(variable);
    previousPointExists = true;
  }
  plotSegment(point);
  std::get<2>(variable) = point;

  if ((std::get<0>(variable).at(0).first) < xBounds.first)
    std::get<0>(variable).pop_front();

  existing->second = variable;
}

//* resizing
void Graph::resize(Point newXBounds, Point newYBounds)
{
  if (xBounds.first == newXBounds.first &&
      xBounds.second == newXBounds.second &&
      yBounds.first == newYBounds.first &&
      yBounds.second == newYBounds.second)
    return;
  xBounds = newXBounds;
  yBounds = newYBounds;
  xRange = xBounds.second - xBounds.first;
  yRange = yBounds.second - yBounds.first;
  drawGraph();
}

//private:
// pair<int, int> topLeft;
// int width;
// int height;
// pair<double, double> xBounds;
// pair<double, double> yBounds;
// double xRange;
// double yRange;
// bool previousPointExists = false;
// Point previousPoint;

// float resolution = 1.0;

// vector<pair<Point, color>> points{};
// vector<pair<Point, color>> segments{};

// map<
//     string,
//     Variable>
//     variables{};

bool Graph::isInBounds(Point point)
{
  double x = (double)point.first;
  double y = -(double)point.second; // account for screen having increasing y values down
  if (!threshold(x, xBounds.first, xBounds.second))
    return false;
  if (!threshold(-y, yBounds.first, yBounds.second))
    return false;
  return true;
}

Point Graph::calculatePoint(Point point)
{
  return Point(
      topLeft.first + (point.first - xBounds.first) / xRange * (float)width,
      topLeft.second + (-point.second + yBounds.second) / yRange * (float)height);
}
