#ifndef VOLUMEGLRENDER_H
#define VOLUMEGLRENDER_H

#include <_mingw.h> // __int64
#include <GL/glew.h>

#include <QtEvents>
#include <QGLWidget>
#include <QString>

#include <node.h>

using namespace MathUtils;

/// \todo needs merege, see GridGenerator SimpleGLRender
class VolumeGLRender : public QGLWidget
{
    // set on construction, can't be changed
    private: const int _RVEDiscretesize;
    private: const float * const _ptrToRVEdata = nullptr;

    private: float _innerCutLevel = 0.5f;
    public : float getInnerCutLevel() const noexcept {return _innerCutLevel;}
    public : void setInnerCutLevel(float newCutLevel) noexcept {_innerCutLevel = newCutLevel;}

    private: int _oldMouseX = 0;
    private: int _oldMouseY = 0;
    private: bool _isPressed = false;
    private: double _angleOY = 0;
    private: double _angleOX = 0;

    private: Vector4D _EnvironmentColor = Vector4D(0.25, 0.5, 0.5, 1.0);
    public : void setEnvironmenColor(double R, double G, double B, double A){
        _EnvironmentColor(R,G,B,A);}

    private: Vector4D _TextColor = Vector4D(0.0, 1.0, 0.0, 1.0);
    public : void setTextColor(double R, double G, double B, double A){
        _TextColor(R,G,B,A);}

    public : QString dataString = "Test string";

    private: Vector3D _CameraPosition = Vector3D(0.0, 0.0, 2.0);
    public : void setCameraPosition(const Vector3D &newPosition) noexcept {
        _CameraPosition = newPosition;}
    public : Vector3D getCameraPosition() const noexcept {return _CameraPosition;}

    private: Vector3D _ScenePosition = Vector3D(0.0, 0.0, 0.0);
    public : void setScenePosition(const Vector3D &newPosition) noexcept {
        _ScenePosition = newPosition;}
    public : Vector3D getScenePosition() const noexcept {return _ScenePosition;}

    private: double _Zoom = 0.5;
    public : void setZoom(const double &newZoom) noexcept {
        _Zoom = newZoom;}
    public : double getZoom() const noexcept {return _Zoom;}

    public : void mousePressEvent(QMouseEvent *e) override;
    public : void mouseReleaseEvent(QMouseEvent *e) override;
    public : void mouseMoveEvent(QMouseEvent *e) override;
    public : void wheelEvent(QWheelEvent *e) override;
    public : void keyPressEvent(QKeyEvent *e) override;

    private: void _drawOrigin() noexcept;

    private: GLuint _drawFieldDisplayListID;
    private: GLuint _fieldTextureID = 0;
    private: void _loadFieldIntoTexture() throw(std::runtime_error);
    private: void _prepareTextureDisplayList() noexcept;

    public : std::string printOpenGLInfo() const noexcept;

    public : VolumeGLRender(
            const int RVEDiscreteSize,
            const float * ptrToRVEData,
            QWidget *pwgt) noexcept;

    public : void initializeGL() override;
    public : void initializeGLEW();
    public : void resizeGL(int nWidth,int nHeight) override;
    public : void paintGL() override;

    public : ~VolumeGLRender(){}
};

#endif // VOLUMEGLRENDER_H
