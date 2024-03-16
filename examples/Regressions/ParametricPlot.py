from PiecewiseRegression import PiecewiseRegression

class ParametricPlot:
    """
        Takes two functions and parametrically creates points to plot them.
        In this case, the plots we're using are PiecewiseRegressions.
    """
    def __init__(self, x_function : PiecewiseRegression, y_function : PiecewiseRegression):
        self._x_function = x_function
        self._y_function = y_function

        # Pick the correct bounds. The data might not be in the same range,
        # So we have to cut off the excess.
        self._min_domain = max(x_function._min_domain, y_function._min_domain)
        self._max_domain = min(x_function._max_domain, y_function._max_domain)

        self._xpoints = []
        self._ypoints = []

        self._create_points()

    def _create_points(self):
        """
            Creates a variety of points to plot on a graph.
            NOTE: THIS IS NOT ACTUALLY A PARAMETRIC FUNCTION, RATHER A COLLECTION
            OF POINTS THAT BEHAVE PARAMETRICALLY

            If you wanted a graph you could do a regression of the x and y points
        """
        domain = self._max_domain - self._min_domain
        increment = domain / 100
        curr_t = self._min_domain
        self._xpoints.append(self._x_function.get_value_at(curr_t))
        self._ypoints.append(self._y_function.get_value_at(curr_t))

        for i in range(100):
            curr_t += increment
            self._xpoints.append(self._x_function.get_value_at(curr_t))
            self._ypoints.append(self._y_function.get_value_at(curr_t))

    def get_x_points(self):
        return self._xpoints

    def get_y_points(self):
        return self._ypoints