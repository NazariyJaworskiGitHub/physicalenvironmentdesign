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
    public: FEM::Vector3D CameraPos     = FEM::Vector3D(0.0, 0.0, 2.0);
    public: FEM::Vector3D ScenePos      = FEM::Vector3D(0.0, 0.0, 0.0);
    public: int OldMouseX               = 0;
    public: int OldMouseY               = 0;
    public: double angleOY              = 0;
    public: double angleOX              = 0;
    public: bool isPressed              = false;
    public: QString dataString          = "Test string";

    private: FEM::Vector4D _EnvironmentColor    = FEM::Vector4D(0.25, 0.5, 0.5, 1.0);
    public : void setEnvironmenColor(double R, double G, double B, double A)
    {
        _EnvironmentColor(R,G,B,A);
    }

    private: void _drawOrigin() noexcept;

    private: FEM::Vector4D _PlcNodesColor       = FEM::Vector4D(1.0, 1.0, 1.0, 1.0);
    public : void setRenderingPlcNodesColor(double R, double G, double B, double A)
    {
        _PlcNodesColor(R,G,B,A);
    }
    private: FEM::Vector4D _PlcSegmentsColor    = FEM::Vector4D(0.0, 1.0, 0.0, 1.0);
    public : void setRenderingPlcSegmentsColor(double R, double G, double B, double A)
    {
        _PlcSegmentsColor(R,G,B,A);
    }
    private: FEM::Vector4D _PlcFacetsColor      = FEM::Vector4D(0.8, 0.8, 0.8, 1.0);
    public : void setRenderingPlcFacetsColor(double R, double G, double B, double A)
    {
        _PlcFacetsColor(R,G,B,A);
    }

    private: const DelaunayGridGenerator::Plc3D *_ptrToRenderingPlc3D = nullptr;
    public: void setRenderingPiecewiseLinearComplex(
            const DelaunayGridGenerator::Plc3D *renderingPlc3D) throw (std::logic_error)
    {
        _ptrToRenderingPlc3D = renderingPlc3D;
    }
    private: void _drawPlc3DNodes() throw(std::runtime_error);
    private: void _drawPlc3DSegments() throw(std::runtime_error);
    private: void _drawPlc3DFacets() throw(std::runtime_error);

    private: FEM::Vector4D _GeneratorAliveNodesColor    = FEM::Vector4D(1.0, 0.0, 0.0, 1.0);
    public : void setGeneratorAliveNodesColor(double R, double G, double B, double A)
    {
        _GeneratorAliveNodesColor(R,G,B,A);
    }
    private: FEM::Vector4D _GeneratorDeadNodesColor     = FEM::Vector4D(1.0, 1.0, 0.0, 1.0);
    public : void setGeneratorDeadNodesColor(double R, double G, double B, double A)
    {
        _GeneratorDeadNodesColor(R,G,B,A);
    }
    private: FEM::Vector4D _GeneratorAliveFacetsColor   = FEM::Vector4D(0.8, 0.8, 0.8, 0.5);
    public : void setGeneratorAliveFacetsColor(double R, double G, double B, double A)
    {
        _GeneratorAliveFacetsColor(R,G,B,A);
    }
    private: FEM::Vector4D _GeneratorAliveFacetsEdgesColor = FEM::Vector4D(1.0, 0.0, 0.0, 1.0);
    public : void setGeneratorAliveFacetsEdgesColor(double R, double G, double B, double A)
    {
        _GeneratorAliveFacetsEdgesColor(R,G,B,A);
    }
    private: FEM::Vector4D _GeneratorDeadFacetsEdgesColor = FEM::Vector4D(0.0, 1.0, 1.0, 1.0);
    public : void setGeneratorDeadFacetsEdgesColor(double R, double G, double B, double A)
    {
        _GeneratorDeadFacetsEdgesColor(R,G,B,A);
    }
    /// \todo it should be const, but then, i cant run single iteration
    private: /*const*/ DelaunayGridGenerator::DelaunayGridGenerator3D
        *_ptrToRenderingDelaunayGridGenerator3D = nullptr;
    public: void setRenderingDelaunayGridGenerator3D(
            /*const*/ DelaunayGridGenerator::DelaunayGridGenerator3D *renderingDelaunayGridGenerator3D)
        throw (std::logic_error)
    {
        _ptrToRenderingDelaunayGridGenerator3D = renderingDelaunayGridGenerator3D;
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
