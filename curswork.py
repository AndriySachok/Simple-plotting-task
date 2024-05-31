import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle, Polygon
import matplotlib.patches as patches
import numpy as np
import json


def read_coordinates(file_path):
    with open(file_path, "r") as file:
        return json.load(file)


def is_point_in_rectangle(px, py, rect):
    x1, y1, x2, y2 = rect
    return x1 <= px <= x2 and y1 <= py <= y2


def is_triangle_in_rectangle(triangle, rect):
    return all(is_point_in_rectangle(px, py, rect) for px, py in triangle)


def do_rectangles_intersect(rect, triangle):
    from shapely.geometry import Polygon as ShapelyPolygon, box

    rect_polygon = box(rect[0], rect[1], rect[2], rect[3])
    triangle_polygon = ShapelyPolygon(triangle)
    return rect_polygon.intersects(triangle_polygon)


def plot_shapes(rectangles, triangles):
    fig, ax = plt.subplots()

    for rect in rectangles:
        x1, y1, x2, y2 = rect
        width, height = x2 - x1, y2 - y1
        rectangle = patches.Rectangle(
            (x1, y1), width, height, linewidth=1, edgecolor="blue", facecolor="none"
        )
        ax.add_patch(rectangle)

    triangles_inside = []
    triangles_intersecting = []
    for triangle in triangles:
        if any(is_triangle_in_rectangle(triangle, rect) for rect in rectangles):
            triangle_patch = Polygon(
                triangle, closed=True, edgecolor="green", facecolor="none", linewidth=2
            )
            triangles_inside.append(triangle)
        elif any(do_rectangles_intersect(rect, triangle) for rect in rectangles):
            triangle_patch = Polygon(
                triangle, closed=True, edgecolor="red", facecolor="none", linewidth=2
            )
            triangles_intersecting.append(triangle)
        else:
            triangle_patch = Polygon(
                triangle, closed=True, edgecolor="black", facecolor="none", linewidth=1
            )

        ax.add_patch(triangle_patch)

    plt.xlim(-10, 10)
    plt.ylim(-10, 10)
    plt.gca().set_aspect("equal", adjustable="box")
    plt.show()

    return triangles_inside, triangles_intersecting


def write_triangles(file_path, triangles):
    with open(file_path, "w") as file:
        json.dump(triangles, file)


# Example usage
rectangles = read_coordinates("rectangles.json")
triangles = read_coordinates("triangles.json")

triangles_inside, triangles_intersecting = plot_shapes(rectangles, triangles)
write_triangles("triangles_inside.json", triangles_inside)
write_triangles("triangles_intersecting.json", triangles_intersecting)
