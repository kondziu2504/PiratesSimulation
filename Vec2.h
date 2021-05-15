//
// Created by konrad on 14.05.2021.
//

#ifndef PIRATESSIMULATION_VEC2_H
#define PIRATESSIMULATION_VEC2_H


struct Vec2 {
    float x, y;
    Vec2(){}
    Vec2(float x, float y);
    float Distance();
    Vec2 Normalized();
    Vec2 Rotate(float angle);
    float Angle();
    static Vec2 FromAngle(float angle);
    float Dot(const Vec2 & b);
    Vec2 operator+(const Vec2 & b);
    Vec2 operator-(const Vec2 & b);
    Vec2 operator*(float b);
    Vec2 operator/(float b);

};


#endif //PIRATESSIMULATION_VEC2_H
