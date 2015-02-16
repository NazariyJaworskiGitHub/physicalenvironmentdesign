#ifndef SIMPLEGLRENDER2D_H
#define SIMPLEGLRENDER2D_H

#include "simpleglrender.h"

class SimpleGLRender2D : public SimpleGLRender
{
    private: void _drawOrigin() noexcept final;
    private: void _RotateAndTranslate() noexcept final;

    private: const DelaunayGridGenerator::Plc2D *_ptrToRenderingPlc2D = nullptr;
    public: void setRenderingPiecewiseLinearComplex(
            const DelaunayGridGenerator::Plc2D *renderingPlc2D) throw (std::logic_error){
        _ptrToRenderingPlc2D = renderingPlc2D;}
    private: FEM::Vector4D _GeneratorDeadFacetsColor = FEM::Vector4D(0.0, 1.0, 1.0, 1.0);
    public : void setGeneratorDeadFacetsColor(double R, double G, double B, double A){
        _GeneratorDeadFacetsColor(R,G,B,A);}

    /// \todo it should be const, but then, i cant run single iteration
    private: /*const*/ DelaunayGridGenerator::DelaunayGridGenerator2D
        *_ptrToRenderingDelaunayGridGenerator2D = nullptr;
    public : void setRenderingDelaunayGridGenerator2D(
            /*const*/ DelaunayGridGenerator::DelaunayGridGenerator2D
            *renderingDelaunayGridGenerator2D)
        throw (std::logic_error){
        _ptrToRenderingDelaunayGridGenerator2D = renderingDelaunayGridGenerator2D;}

    public : void mouseMoveEvent(QMouseEvent *e) override;
    public : void keyPressEvent(QKeyEvent *e) override;

    private: void _drawPlcNodes() throw(std::runtime_error) final;
    private: void _drawPlcSegments() throw(std::runtime_error) final;
    private: void _drawPlcFacets() throw(std::runtime_error) final;

    private: void _drawDelaunayGridGeneratorNodes() throw(std::runtime_error) final;
    private: void _drawDelaunayGridGeneratorFacets() throw(std::runtime_error) final;

    public : SimpleGLRender2D(QWidget *pwgt) noexcept;
    public : ~SimpleGLRender2D(){}
};

#endif // SIMPLEGLRENDER2D_H
