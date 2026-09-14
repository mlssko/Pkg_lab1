#pragma once

struct Vector3
{
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
};

struct Matrix3
{
    double m[3][3] = { {1,0,0}, {0,1,0}, {0,0,1} };

    static Matrix3 identity()
    {
        return Matrix3{};
    }

    Vector3 multiply(const Vector3& v) const
    {
        return Vector3{
            m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z,
            m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z,
            m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z
        };
    }

    Matrix3 multiply(const Matrix3& o) const
    {
        Matrix3 r;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                double sum = 0.0;
                for (int k = 0; k < 3; ++k) {
                    sum += m[i][k] * o.m[k][j];
                }
                r.m[i][j] = sum;
            }
        }
        return r;
    }

    double determinant() const
    {
        return m[0][0]*(m[1][1]*m[2][2] - m[1][2]*m[2][1])
        - m[0][1]*(m[1][0]*m[2][2] - m[1][2]*m[2][0])
            + m[0][2]*(m[1][0]*m[2][1] - m[1][1]*m[2][0]);
    }

    Matrix3 inverse() const
    {
        double det = determinant();
        if (det == 0.0) {
            return Matrix3::identity();
        }
        double invDet = 1.0 / det;

        Matrix3 r;
        r.m[0][0] =  (m[1][1]*m[2][2] - m[1][2]*m[2][1]) * invDet;
        r.m[0][1] = -(m[0][1]*m[2][2] - m[0][2]*m[2][1]) * invDet;
        r.m[0][2] =  (m[0][1]*m[1][2] - m[0][2]*m[1][1]) * invDet;

        r.m[1][0] = -(m[1][0]*m[2][2] - m[1][2]*m[2][0]) * invDet;
        r.m[1][1] =  (m[0][0]*m[2][2] - m[0][2]*m[2][0]) * invDet;
        r.m[1][2] = -(m[0][0]*m[1][2] - m[0][2]*m[1][0]) * invDet;

        r.m[2][0] =  (m[1][0]*m[2][1] - m[1][1]*m[2][0]) * invDet;
        r.m[2][1] = -(m[0][0]*m[2][1] - m[0][1]*m[2][0]) * invDet;
        r.m[2][2] =  (m[0][0]*m[1][1] - m[0][1]*m[1][0]) * invDet;

        return r;
    }
};