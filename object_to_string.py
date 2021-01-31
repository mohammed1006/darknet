import unittest


def convert_from_objects_to_string(detections: list) -> str:
    """
     detections : list of detection = [(label, confidence, bbox)]
     bbox = (x,y,w,h)
     """
    result = ""
    detections.sort(key=lambda x: x[2][0])
    for i in range(len(detections)):
        if i != 0:
            current_x, current_y, current_w, current_h = detections[i][2]
            previous_x, previous_y, previous_w, previous_h = detections[i - 1][2]
            bottom_current_y = current_y + current_h / 2

            if bottom_current_y <= previous_y:
                result += "^"
        result += detections[i][0]
    return result


class Tests(unittest.TestCase):

    def test_convert_from_objects_to_string(self):
        detections = [("=", 0.4, (0.398438, 0.509766, 0.312500, 0.097656)),
                      ("x", 0.4, (0.720703, 0.500000, 0.222656, 0.250000)),
                      ("2", 0.4, (0.921875, 0.423828, 0.140625, 0.128906)),
                      ("4", 0.4, (0.080078, 0.529297, 0.152344, 0.269531))]
        self.assertEqual(convert_from_objects_to_string(detections), "4=x^2")

        detections = [("2", 0.4, (0.081727, 0.655028, 0.109483, 0.388268)),
                      ("x", 0.4, (0.173863, 0.712291, 0.070162, 0.268156)),
                      ("2", 0.4, (0.237471, 0.512570, 0.053971, 0.192737)),
                      ("-", 0.4, (0.361218, 0.639665, 0.062452, 0.139665)),
                      ("3", 0.4, (0.459522, 0.553073, 0.074017, 0.329609)),
                      ("(", 0.4, (0.533539, 0.560056, 0.049345, 0.455307)),
                      ("x", 0.4, (0.597533, 0.574022, 0.055513, 0.287709)),
                      ("+", 0.4, (0.669237, 0.539106, 0.050887, 0.212291)),
                      ("1", 0.4, (0.740170, 0.519553, 0.041635, 0.307263)),
                      (")", 0.4, (0.792213, 0.501397, 0.040864, 0.494413)),
                      ("=", 0.4, (0.874711, 0.526536, 0.056284, 0.203911)),
                      ("0", 0.4, (0.941789, 0.467877, 0.060910, 0.343575)),
                      ]
        self.assertEqual(convert_from_objects_to_string(detections), "2x^2-3(x+1)=0")

        detections = [("=", 0.4, (0.398438, 0.509766, 0.312500, 0.097656)),
                      ("x", 0.4, (0.720703, 0.500000, 0.222656, 0.250000)),
                      ("2", 0.4, (0.921875, 0.423828, 0.140625, 0.228906)),
                      ("4", 0.4, (0.080078, 0.529297, 0.152344, 0.269531))]
        self.assertEqual(convert_from_objects_to_string(detections), "4=x2")
