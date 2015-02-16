#ifndef SIMPLEGLRENDER_H
#define SIMPLEGLRENDER_H

#include <GL/glu.h>
#include <QtOpenGL>
#include <QString>

#include "piecewiselinearcomplex.h"
#include "generator.h"

class SimpleGLRender : public QGLWidget
{
    protected: int _oldMouseX                   = 0;
    protected: int _oldMouseY                   = 0;
    protected: bool _isPressed                  = false;

    protected: FEM::Vector4D _EnvironmentColor  = FEM::Vector4D(0.25, 0.5, 0.5, 1.0);
    public   : void setEnvironmenColor(double R, double G, double B, double A){
        _EnvironmentColor(R,G,B,A);}

    protected: FEM::Vector4D _TextColor         = FEM::Vector4D(0.0, 1.0, 0.0, 1.0);
    public   : void setTextColor(double R, double G, double B, double A){
        _TextColor(R,G,B,A);}

    public   : QString dataString               = "Test string";

    protected: FEM::Vector3D _CameraPosition    = FEM::Vector3D(0.0, 0.0, 2.0);
    public   : void setCameraPosition(const FEM::Vector3D &newPosition) noexcept {
        _CameraPosition = newPosition;}
    public   : FEM::Vector3D getCameraPosition() const noexcept {return _CameraPosition;}

    protected: FEM::Vector3D _ScenePosition     = FEM::Vector3D(0.0, 0.0, 0.0);
    public   : void setScenePosition(const FEM::Vector3D &newPosition) noexcept {
        _ScenePosition = newPosition;}
    public   : FEM::Vector3D getScenePosition() const noexcept {return _ScenePosition;}

    protected: double _Zoom                     = 1.0;
    public   : void setZoom(const double &newZoom) noexcept {
        _Zoom = newZoom;}
    public   : double getZoom() const noexcept {return _Zoom;}

    public   : void mousePressEvent(QMouseEvent *e) override;
    public   : void mouseReleaseEvent(QMouseEvent *e) override;
    public   : void wheelEvent(QWheelEvent *e) override;

    protected: virtual void _drawOrigin() noexcept          = 0;
    protected: virtual void _RotateAndTranslate() noexcept  = 0;

    protected: FEM::Vector4D _PlcNodesColor     = FEM::Vector4D(1.0, 1.0, 1.0, 1.0);
    public   : void setRenderingPlcNodesColor(double R, double G, double B, double A){
        _PlcNodesColor(R,G,B,A);}

    protected: FEM::Vector4D _PlcFacetsColor    = FEM::Vector4D(0.8, 0.8, 0.8, 1.0);
    public   : void setRenderingPlcFacetsColor(double R, double G, double B, double A){
        _PlcFacetsColor(R,G,B,A);}

    protected: virtual void _drawPlcNodes() throw(std::runtime_error)       = 0;
    protected: virtual void _drawPlcSegments() throw(std::runtime_error)    = 0;
    protected: virtual void _drawPlcFacets() throw(std::runtime_error)      = 0;

    protected: FEM::Vector4D _GeneratorAliveNodesColor    = FEM::Vector4D(1.0, 0.0, 0.0, 1.0);
    public   : void setGeneratorAliveNodesColor(double R, double G, double B, double A){
        _GeneratorAliveNodesColor(R,G,B,A);}

    protected: FEM::Vector4D _GeneratorDeadNodesColor     = FEM::Vector4D(1.0, 1.0, 0.0, 1.0);
    public   : void setGeneratorDeadNodesColor(double R, double G, double B, double A){
        _GeneratorDeadNodesColor(R,G,B,A);}

    protected: FEM::Vector4D _GeneratorAliveFacetsColor   = FEM::Vector4D(0.8, 0.8, 0.8, 0.5);
    public   : void setGeneratorAliveFacetsColor(double R, double G, double B, double A){
        _GeneratorAliveFacetsColor(R,G,B,A);}

    protected: FEM::Vector4D _GeneratorTopAliveFacetColor = FEM::Vector4D(1.0, 0.8, 0.0, 0.5);
    public   : void setGeneratorTopAliveFacetColor(double R, double G, double B, double A){
        _GeneratorTopAliveFacetColor(R,G,B,A);}

    protected: virtual void _drawDelaunayGridGeneratorNodes() throw(std::runtime_error)  = 0;
    protected: virtual void _drawDelaunayGridGeneratorFacets() throw(std::runtime_error) = 0;

    public   : SimpleGLRender(QWidget *pwgt) noexcept;

    public   : void initializeGL() override;
    public   : void resizeGL(int nWidth,int nHeight) override;
    public   : void paintGL() override;

    public   : virtual ~SimpleGLRender(){}
};


#endif // SIMPLEGLRENDER_H
