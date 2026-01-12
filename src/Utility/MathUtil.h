#ifndef FINALDAYONEARTH_MATHUTIL_H
#define FINALDAYONEARTH_MATHUTIL_H


class MathUtil {
public:
    static double lerp(double a, double b, double value) {
        return a + (b - a) * value;
    }

    static double smoothStep(double a, double b, double value) {
        // 3x^2 - 2x^3
        return a + value * value * (3 - 2 * value) * (b - a);
    }
};

#endif //FINALDAYONEARTH_MATHUTIL_H
