#include "json.hpp"
#include "matplotlibcpp.h"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <fstream>
#include <iostream>
#include <vector>

namespace bg = boost::geometry;
namespace plt = matplotlibcpp;
using json = nlohmann::json;

using Point = bg::model::d2::point_xy<double>;
using Box = bg::model::box<Point>;
using Polygon = bg::model::polygon<Point>;

// Read coordinates from JSON file
std::vector<Box> read_rectangles(const std::string &file_path) {
  std::ifstream file(file_path);
  json data;
  file >> data;
  std::vector<Box> rectangles;
  for (const auto &item : data) {
    rectangles.emplace_back(Point(item[0], item[1]), Point(item[2], item[3]));
  }
  return rectangles;
}

std::vector<Polygon> read_triangles(const std::string &file_path) {
  std::ifstream file(file_path);
  json data;
  file >> data;
  std::vector<Polygon> triangles;
  for (const auto &item : data) {
    Polygon triangle;
    for (const auto &point : item) {
      bg::append(triangle.outer(), Point(point[0], point[1]));
    }
    bg::correct(triangle);
    triangles.push_back(triangle);
  }
  return triangles;
}

// Check if a triangle is fully inside a rectangle
bool is_triangle_inside_rectangle(const Polygon &triangle, const Box &rect) {
  for (const auto &point : triangle.outer()) {
    if (!bg::within(point, rect)) {
      return false;
    }
  }
  return true;
}

// Check if a triangle intersects with a rectangle
bool do_rectangle_and_triangle_intersect(const Box &rect,
                                         const Polygon &triangle) {
  Polygon rect_polygon;
  bg::convert(rect, rect_polygon);
  return bg::intersects(rect_polygon, triangle);
}

// Plot shapes using matplotlib-cpp
void plot_shapes(const std::vector<Box> &rectangles,
                 const std::vector<Polygon> &triangles) {
  for (const auto &rect : rectangles) {
    double x1 = bg::get<bg::min_corner, 0>(rect);
    double y1 = bg::get<bg::min_corner, 1>(rect);
    double x2 = bg::get<bg::max_corner, 0>(rect);
    double y2 = bg::get<bg::max_corner, 1>(rect);
    plt::plot({x1, x2, x2, x1, x1}, {y1, y1, y2, y2, y1}, "b-");
  }

  for (const auto &triangle : triangles) {
    std::vector<double> x, y;
    for (const auto &point : triangle.outer()) {
      x.push_back(point.x());
      y.push_back(point.y());
    }
    x.push_back(triangle.outer()[0].x());
    y.push_back(triangle.outer()[0].y());

    bool inside = false;
    bool intersect = false;

    for (const auto &rect : rectangles) {
      if (is_triangle_inside_rectangle(triangle, rect)) {
        inside = true;
        break;
      }
      if (do_rectangle_and_triangle_intersect(rect, triangle)) {
        intersect = true;
      }
    }

    if (inside) {
      plt::plot(x, y, "g-");
    } else if (intersect) {
      plt::plot(x, y, "r-");
    } else {
      plt::plot(x, y, "k-");
    }
  }

  plt::show();
}

// Write triangles to JSON file
void write_triangles(const std::string &file_path,
                     const std::vector<Polygon> &triangles) {
  json data;
  for (const auto &triangle : triangles) {
    json points;
    for (const auto &point : triangle.outer()) {
      points.push_back({point.x(), point.y()});
    }
    data.push_back(points);
  }
  std::ofstream file(file_path);
  file << data.dump(4);
}

int main() {
  std::vector<Box> rectangles = read_rectangles("rectangles.json");
  std::vector<Polygon> triangles = read_triangles("triangles.json");

  std::vector<Polygon> triangles_inside;
  std::vector<Polygon> triangles_intersecting;

  for (const auto &triangle : triangles) {
    bool inside = false;
    bool intersect = false;
    for (const auto &rect : rectangles) {
      if (is_triangle_inside_rectangle(triangle, rect)) {
        inside = true;
        break;
      }
      if (do_rectangle_and_triangle_intersect(rect, triangle)) {
        intersect = true;
      }
    }
    if (inside) {
      triangles_inside.push_back(triangle);
    } else if (intersect) {
      triangles_intersecting.push_back(triangle);
    }
  }

  plot_shapes(rectangles, triangles);
  write_triangles("triangles_inside.json", triangles_inside);
  write_triangles("triangles_intersecting.json", triangles_intersecting);

  return 0;
}
