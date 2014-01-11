#ifndef NODE_H
#define NODE_H

#include <QVector4D>
#include <QVector3D>
#include <QVector2D>

namespace FEM
{
    /// implements [] operator for QVector4D (need for FiniElement class)
    class Node4D : public QVector4D
    {
        public: Node4D(){}
        public: Node4D(const qreal xp, const qreal yp, const qreal zp, const qreal wp): QVector4D(xp,yp,zp,wp){}
        public: Node4D(const Node4D &target): QVector4D(target.x(),target.y(),target.z(),target.w()){}
        public: qreal operator [](const int index) throw(std::out_of_range)
        {
            switch (index)
            {
            case 0:
                return x();
            case 1:
                return y();
            case 2:
                return z();
            case 3:
                return w();
            default:
                throw std::out_of_range("Node4D[i], i out of range");
            }
        }
    };

    /// implements [] operator for QVector3D (need for FiniElement class)
    class Node3D : public QVector3D
    {
        public: Node3D(){}
        public: Node3D(const qreal xp, const qreal yp, const qreal zp): QVector3D(xp,yp,zp){}
        public: Node3D(const Node3D &target): QVector3D(target.x(),target.y(),target.z()){}
        public: qreal operator [](const int index) throw(std::out_of_range)
        {
            switch (index)
            {
            case 0:
                return x();
            case 1:
                return y();
            case 2:
                return z();
            default:
                throw std::out_of_range("Node3D[i], i out of range");
            }
        }
    };

    /// implements [] operator for QVector2D (need for FiniElement class)
    class Node2D : public QVector2D
    {
        public: Node2D(){}
        public: Node2D(const qreal xp, const qreal yp): QVector2D(xp,yp){}
        public: Node2D(const Node2D &target): QVector2D(target.x(),target.y()){}
        public: qreal operator [](const int index) throw(std::out_of_range)
        {
            switch (index)
            {
            case 0:
                return x();
            case 1:
                return y();
            default:
                throw std::out_of_range("Node2D[i], i out of range");
            }
        }
    };

    /// implements [] operator for qreal (need for FiniElement class)
    class Node1D
    {
        private: qreal _xPos;
        public: Node1D(){}
        public: Node1D(const qreal xp):_xPos(xp){}
        public: Node1D(const Node1D &target): _xPos(target._xPos){}
        public: qreal operator [](const int index) throw(std::out_of_range)
        {
            switch (index)
            {
            case 0:
                return _xPos;
            default:
                throw std::out_of_range("Node1D[i], i out of range");
            }
        }
    };
}

#endif // NODE_H
