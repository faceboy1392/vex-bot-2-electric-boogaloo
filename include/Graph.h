#ifndef GRAPH_H
#define GRAPH_H

#include "vex.h"
#include <utility>
#include <deque>
#include <string>
#include <map>
#include <vector>

using std::pair;
using std::make_pair;
using std::deque;
using std::string;
using std::map;
using std::vector;
using std::tuple;

using Point = pair<float, float>;
using Variable = std::tuple<deque<Point>, vex::color, Point, bool>;

class Vector3
{
public:
  double x;
  double y;
  double z;

  Vector3();
  Vector3(double _x, double _y, double _z);

  Vector3 operator+(Vector3 other);
  Vector3 operator-(Vector3 other);
  Vector3 operator-();
  void operator+=(Vector3 other);
  void operator-=(Vector3 other);
  Vector3 operator*(double m);
};

template <typename T>
T clamp(T value, T minValue, T maxValue);

template <typename T>
bool threshold(T value, T minValue, T maxValue);

class Graph
{
public:
  Graph();
  Graph(pair<int, int> topLeft, int width, int height);
  Graph(pair<double, double> xBounds, pair<double, double> yBounds);
  Graph(pair<int, int> topLeft, int width, int height, pair<double, double> xBounds, pair<double, double> yBounds);

  void drawGraph();
  void setColor(vex::color c);
  void plotPoint(float x, float y);
  void plotPoint(Point point);
  void plotSegment(float x, float y);
  void plotSegment(Point point);
  void plotLine(Point pointA, Point pointB);
  void addPoint(float x, float y);
  void addPoint(Point point);
  void addPoint(float x, float y, vex::color c);
  void addPoint(Point point, vex::color c);
  void addSegment(float x, float y);
  void addSegment(Point point);
  void addSegment(float x, float y, vex::color c);
  void addSegment(Point point, vex::color c);
  void addVariable(string name, vex::color col);
  void addSegmentToVariable(string varName, Point point);
  void resize(Point newXBounds, Point newYBounds);

private:
  pair<int, int> topLeft;
  int width;
  int height;
  pair<double, double> xBounds;
  pair<double, double> yBounds;
  double xRange;
  double yRange;
  bool previousPointExists;
  Point previousPoint;
  float resolution;

  vector<pair<Point, vex::color>> points;
  vector<pair<Point, vex::color>> segments;

  map<string, Variable> variables;

  bool isInBounds(Point point);
  Point calculatePoint(Point point);
};

#endif // GRAPH_H