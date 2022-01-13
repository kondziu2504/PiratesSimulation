//
// Created by konrad on 14.05.2021.
//

#ifndef PIRATESSIMULATION_VEC2F_H
#define PIRATESSIMULATION_VEC2F_H


struct Vec2f {
    float x, y;

    Vec2f() : Vec2f(0, 0) {}
    Vec2f(float x, float y);

    [[nodiscard]] float Length() const;
    [[nodiscard]] Vec2f Normalized() const;
    [[nodiscard]] Vec2f Rotated(float angle) const;
    [[nodiscard]] float Angle() const;
    static Vec2f FromAngle(float angle);
    [[nodiscard]] float Dot(const Vec2f & b) const;

    Vec2f operator+(const Vec2f & b) const;
    Vec2f operator-(const Vec2f & b) const;
    Vec2f operator*(float b) const;
    Vec2f operator/(float b) const;
    Vec2f& operator+=(const Vec2f & b);
};


#endif //PIRATESSIMULATION_VEC2F_H
