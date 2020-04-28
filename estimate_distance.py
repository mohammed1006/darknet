import math


class EstimatorOfDistance(object):
    def __init__(self):
        super().__init__()
        self.pillars_distance = 5.0
        self.markers = []
        self.markers.append((92, 606))
        self.markers.append((201, 519))
        self.markers.append((266, 466))
        self.markers.append((312, 432))
        self.markers.append((341, 408))
        self.markers.append((366, 390))
        self.markers.append((385, 377))
        self.markers.append((400, 366))
        self.markers.append((413, 357))
        self.markers.append((423, 351))
        self.markers.append((432, 345))
        self.markers.append((440, 340))
        self.markers.append((446, 336))
        self.markers.append((452, 332))
        self.markers_x = []
        for marker in self.markers:
            self.markers_x.append(marker[0])

    def test(self):
        markers_cnt = len(self.markers)

        dists_cnt = markers_cnt - 1
        dist_ratios_cnt = dists_cnt - 1
        dists = []
        dist_ratios = []
        for i in range(dists_cnt):
            dist = math.sqrt(((self.markers[i][0] - self.markers[i+1][0]) ** 2) +
                             ((self.markers[i][1] - self.markers[i+1][1]) ** 2))
            dists.append(dist)

        for i in range(dist_ratios_cnt):
            dist_ratio = dists[i] / dists[i+1]
            dist_ratios.append(dist_ratio)

        print(dists)
        print(dist_ratios)

    def get_foot_point(self, point):
        """
        @point, line_p1, line_p2 : [x, y, z]
        """

        line_p1 = self.markers[0]
        line_p2 = self.markers[-1]

        x0 = point[0]
        y0 = point[1]
        z0 = 0

        x1 = line_p1[0]
        y1 = line_p1[1]
        z1 = 0

        x2 = line_p2[0]
        y2 = line_p2[1]
        z2 = 0

        k = -((x1 - x0) * (x2 - x1) + (y1 - y0) * (y2 - y1) + (z1 - z0) * (z2 - z1)) / \
            ((x2 - x1) ** 2 + (y2 - y1) ** 2 + (z2 - z1) ** 2) * 1.0

        xn = k * (x2 - x1) + x1
        yn = k * (y2 - y1) + y1
        zn = k * (z2 - z1) + z1

        return [xn, yn]

    def get_ref_line_index(self, x):
        markers_cnt = len(self.markers)
        for i in range(markers_cnt - 1):
            if self.markers_x[i] <= x <= self.markers_x[i+1]:
                px_interval = self.markers_x[i+1] - self.markers_x[i]
                x_dist = x - self.markers_x[i]
                ratio = x_dist / px_interval
                return [i, ratio]
        return None

    def estimate(self, point01, point02):
        foot01 = self.get_foot_point(point01)
        foot02 = self.get_foot_point(point02)
        x2 = foot02[0]
        x1 = foot01[0]
        idx01 = self.get_ref_line_index(x1)
        idx02 = self.get_ref_line_index(x2)
        if idx01 is not None and idx02 is not None:
            i01, ratio01 = idx01
            i02, ratio02 = idx02
            compensation = ratio02 - ratio01
            idx_diff = i02 - i01
            distance_pillar_cnt = idx_diff + compensation
            distance = distance_pillar_cnt * self.pillars_distance
        else:
            distance = None
        return distance


if __name__ == '__main__':
    est = EstimatorOfDistance()
    pt1 = (190, 441)
    pt2 = (404, 331)
    print(est.estimate(pt1, pt2))

