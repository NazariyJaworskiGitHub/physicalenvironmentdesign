#ifndef SIMPLEGLRENDER3D_H
#define SIMPLEGLRENDER3D_H

#include <QMouseEvent>

#include "simpleglrender.h"

class SimpleGLRender3D : public SimpleGLRender
{
    private: double angleOY                     = 0;
    private: double angleOX                     = 0;

    private: void _drawOrigin() noexcept final;
    private: void _RotateAndTranslate() noexcept final;

    private: FEM::Vector4D _PlcSegmentsColor    = FEM::Vector4D(0.0, 1.0, 0.0, 1.0);
    public   : void setRenderingPlcSegmentsColor(double R, double G, double B, double A){
        _PlcSegmentsColor(R,G,B,A);}

    private: const DelaunayGridGenerator::Plc3D *_ptrToRenderingPlc3D = nullptr;
    public: void setRenderingPiecewiseLinearComplex(
            const DelaunayGridGenerator::Plc3D *renderingPlc3D) throw (std::logic_error){
        _ptrToRenderingPlc3D = renderingPlc3D;}

    private: FEM::Vector4D _GeneratorAliveFacetsEdgesColor = FEM::Vector4D(1.0, 0.0, 0.0, 1.0);
    public : void setGeneratorAliveFacetsEdgesColor(double R, double G, double B, double A){
        _GeneratorAliveFacetsEdgesColor(R,G,B,A);}

    private: FEM::Vector4D _GeneratorDeadFacetsEdgesColor = FEM::Vector4D(0.0, 1.0, 1.0, 1.0);
    public : void setGeneratorDeadFacetsEdgesColor(double R, double G, double B, double A){
        _GeneratorDeadFacetsEdgesColor(R,G,B,A);}

    /// \todo it should be const, but then, i cant run single iteration
    private: /*const*/ DelaunayGridGenerator::DelaunayGridGenerator3D
        *_ptrToRenderingDelaunayGridGenerator3D = nullptr;
    public : void setRenderingDelaunayGridGenerator3D(
            /*const*/ DelaunayGridGenerator::DelaunayGridGenerator3D
            *renderingDelaunayGridGenerator3D)
        throw (std::logic_error){
        _ptrToRenderingDelaunayGridGenerator3D = renderingDelaunayGridGenerator3D;}

    public : void mouseMoveEvent(QMouseEvent *e) override;
    public : void keyPressEvent(QKeyEvent *e) override;

    private: void _drawPlcNodes() throw(std::runtime_error) final;
    private: void _drawPlcSegments() throw(std::runtime_error) final;
    private: void _drawPlcFacets() throw(std::runtime_error) final;

    private: void _drawDelaunayGridGeneratorNodes() throw(std::runtime_error) final;
    private: void _drawDelaunayGridGeneratorFacets() throw(std::runtime_error) final;

    public : SimpleGLRender3D(QWidget *pwgt) noexcept;
    public : ~SimpleGLRender3D(){}
};

#endif // SIMPLEGLRENDER3D_H
