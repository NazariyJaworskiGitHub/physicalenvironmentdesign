#ifndef VOLUMEGLRENDERBASECONTROLLER_H
#define VOLUMEGLRENDERBASECONTROLLER_H

#include <_mingw.h> // __int64
#include <GL/glew.h>

#include <QtEvents>
#include <QGLWidget>
#include <QString>

#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>

namespace UserInterface
{
    class VolumeGLRenderBaseController : public QGLWidget
    {
        Q_OBJECT

        // Scene = Proj * World * Control * Zoom * Model
        protected: QMatrix4x4 _mControl;
        protected: QMatrix4x4 _mWorld;

        protected: int _oldMouseX = 0;
        protected: int _oldMouseY = 0;
        protected: bool _isPressed = false;

        protected: QColor _BackgroundColor = QColor(64, 128, 128, 255);
        public   : void setEnvironmenColor(const QColor &newColor) noexcept{
                    _BackgroundColor = newColor;}

        protected: QFont _TextFont = QFont();
        public   : void setTextFont(const QFont &newFont){
                    _TextFont = newFont;}
        protected: QColor _TextColor = QColor(0, 255, 0, 255);
        public   : void setTextColor(const QColor &newColor){
                    _TextColor = newColor;}
        protected: QString _infoString = "Info string";
        public   : void setInfoString(const QString &newString) noexcept{
                    _infoString = newString;}

        protected: QVector3D _CameraPosition = QVector3D(0.0f, 0.0f, 2.0f);
        public   : void setCameraPosition(const QVector3D &newPosition) noexcept {
                    _CameraPosition = newPosition;}
        public   : QVector3D getCameraPosition() const noexcept {return _CameraPosition;}

        protected: float _Zoom = 1.0f;
        public   : void setZoom(const float &newZoom) noexcept {
                    _Zoom = newZoom;}
        public   : float getZoom() const noexcept {return _Zoom;}

        protected: void _renderMultilineText(
                    int x, int y, const QString &text, const QFont &f = QFont()) noexcept;

        protected: void _drawArrow(
                    GLfloat x1, GLfloat y1, GLfloat z1,
                    GLfloat x2, GLfloat y2, GLfloat z2) noexcept;
        protected: void _drawOrigin() noexcept;

        public   : float _boundingBoxRepresentationSize = 1.0f;
        public   : void setBoundingBoxRepresentationSize(
                    const float &newRepresentationSize) noexcept{
                    _boundingBoxRepresentationSize = newRepresentationSize;}
        protected: void _drawBoundingBox() noexcept;

        public   : std::string printOpenGLInfo() const noexcept;

        public   : virtual void mousePressEvent(QMouseEvent *e) override;
        public   : virtual void mouseReleaseEvent(QMouseEvent *e) override;
        public   : virtual void mouseMoveEvent(QMouseEvent *e) override;
        public   : virtual void wheelEvent(QWheelEvent *e) override;
        public   : virtual void keyPressEvent(QKeyEvent *e) override;

        public   : virtual void initializeGL() override;
        public   : virtual void resizeGL(int nWidth,int nHeight) override;

        public   : VolumeGLRenderBaseController(QWidget *pwgt) noexcept;
        public   : ~VolumeGLRenderBaseController();
    };
}

#endif // VOLUMEGLRENDERBASECONTROLLER_H
