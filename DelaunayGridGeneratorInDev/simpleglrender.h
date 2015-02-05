#ifndef SIMPLEGLRENDER_H
#define SIMPLEGLRENDER_H

#include <GL/glu.h>
#include <QtOpenGL>
#include <QString>
#include <QMouseEvent>

#include "piecewiselinearcomplex.h"
#include "generator.h"

class SimpleGLRender : public QGLWidget
{
    public: FEM::Vector3D CameraPos;
    public: FEM::Vector3D ScenePos;
    public: int OldMouseX;
    public: int OldMouseY;
    public: double angleOY;
    public: double angleOX;
    public: bool isPressed;
    public: QString dataString;

    private: void _drawOrigin() noexcept;

    /// default [1.0, 1.0, 1.0, 1.0] - white, see constructor
    private: FEM::Vector4D _PlcNodesColor;
    public : void setRenderingPlcNodesColor(double R, double G, double B, double A)
    {
        _PlcNodesColor(R,G,B,A);
    }
    /// default [0.0, 1.0, 0.0, 1.0] - green, see constructor
    private: FEM::Vector4D _PlcSegmentsColor;
    public : void setRenderingPlcSegmentsColor(double R, double G, double B, double A)
    {
        _PlcSegmentsColor(R,G,B,A);
    }
    /// default [0.8, 0.8, 0.8, 1.0] - light gray, see constructor
    private: FEM::Vector4D _PlcFacetsColor;
    public : void setRenderingPlcFacetsColor(double R, double G, double B, double A)
    {
        _PlcFacetsColor(R,G,B,A);
    }

    private: const DelaunayGridGenerator::CommonPlc3D *_refToRenderingPlc3D = nullptr;
    public: void setRenderingPiecewiseLinearComplex(
            const DelaunayGridGenerator::CommonPlc3D *renderingPlc3D) throw (std::logic_error)
    {
        _refToRenderingPlc3D = renderingPlc3D;
    }
    private: void _drawPlc3DNodes() throw(std::runtime_error);
    private: void _drawPlc3DSegments() throw(std::runtime_error);
    private: void _drawPlc3DFacets() throw(std::runtime_error);

    private: const DelaunayGridGenerator::DelaunayGridGenerator3D
        *_refToRenderingDelaunayGridGenerator3D = nullptr;
    public: void setRenderingDelaunayGridGenerator3D(
            const DelaunayGridGenerator::DelaunayGridGenerator3D *renderingDelaunayGridGenerator3D)
        throw (std::logic_error)
    {
        _refToRenderingDelaunayGridGenerator3D = renderingDelaunayGridGenerator3D;
    }
    private: void _drawDelaunayGridGenerator3DNodes() throw(std::runtime_error);
    private: void _drawDelaunayGridGenerator3DFacets() throw(std::runtime_error);
    private: void _drawDelaunayGridGenerator3DElements() throw(std::runtime_error);

    public: SimpleGLRender(QWidget *pwgt) noexcept;
    public: void initializeGL() override;
    public: void resizeGL(int nWidth,int nHeight) override;
    public: void paintGL() override;
    public: void mouseMoveEvent(QMouseEvent *e) override;
    public: void mousePressEvent(QMouseEvent *e) override;
    public: void mouseReleaseEvent(QMouseEvent *e) override;
    public: void keyPressEvent(QKeyEvent *e) override;
};


#endif // SIMPLEGLRENDER_H
