//
// Created by konrad on 14.05.2021.
//

#ifndef PIRATESSIMULATION_VEC2_H
#define PIRATESSIMULATION_VEC2_H


struct Vec2 {
    float x, y;

    Vec2() : Vec2(0, 0) {}
    Vec2(float x, float y);

    [[nodiscard]] float Length() const;
    [[nodiscard]] Vec2 Normalized() const;
    [[nodiscard]] Vec2 Rotated(float angle) const;
    [[nodiscard]] float Angle() const;
    static Vec2 FromAngle(float angle);
    [[nodiscard]] float Dot(const Vec2 & b) const;

    Vec2 operator+(const Vec2 & b) const;
    Vec2 operator-(const Vec2 & b) const;
    Vec2 operator*(float b) const;
    Vec2 operator/(float b) const;
    Vec2& operator+=(const Vec2 & b);
};


#endif //PIRATESSIMULATION_VEC2_H
