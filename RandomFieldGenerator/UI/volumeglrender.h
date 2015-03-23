#ifndef VOLUMEGLRENDER_H
#define VOLUMEGLRENDER_H

#include <_mingw.h> // __int64
#include <GL/glew.h>

#include <QtEvents>
#include <QGLWidget>
#include <QString>

#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>

#include "UI/volumeglrenderformatdialog.h"

namespace UserInterface
{
    class VolumeGLRender : public QGLWidget
    {
        Q_OBJECT

        friend class UserInterface::VolumeGLRenderFormatDialog;

        // Scene = Proj * World * Control * Zoom * Model
        private: QMatrix4x4 _mControl;
        private: QMatrix4x4 _mWorld;

        // set on construction, can't be changed
        private: const int _RVEDiscretesize;
        private: const float * const _ptrToRVEdata = nullptr;
        private: const float * const _ptrToRVEpotentialField = nullptr;

        private: float _innerCutLevel = 0.5f;
        public : float getInnerCutLevel() const noexcept {return _innerCutLevel;}
        public : void setInnerCutLevel(float newCutLevel) noexcept {_innerCutLevel = newCutLevel;}

        private: int _oldMouseX = 0;
        private: int _oldMouseY = 0;
        private: bool _isPressed = false;

        private: QColor _BackgroundColor = QColor(64, 128, 128, 255);
        public : void setEnvironmenColor(const QColor &newColor) noexcept{
            _BackgroundColor = newColor;}

        private: QFont _TextFont = QFont();
        public : void setTextFont(const QFont &newFont){
            _TextFont = newFont;}
        private: QColor _TextColor = QColor(0, 255, 0, 255);
        public : void setTextColor(const QColor &newColor){
            _TextColor = newColor;}
        public : QString _infoString = "Info string";
        public : void setInfoString(const QString &newString) noexcept{
            _infoString = newString;}

        private: QVector3D _CameraPosition = QVector3D(0.0f, 0.0f, 2.0f);
        public : void setCameraPosition(const QVector3D &newPosition) noexcept {
            _CameraPosition = newPosition;}
        public : QVector3D getCameraPosition() const noexcept {return _CameraPosition;}

        private: float _Zoom = 1.0f;
        public : void setZoom(const float &newZoom) noexcept {
            _Zoom = newZoom;}
        public : float getZoom() const noexcept {return _Zoom;}

        public : void mousePressEvent(QMouseEvent *e) override;
        public : void mouseReleaseEvent(QMouseEvent *e) override;
        public : void mouseMoveEvent(QMouseEvent *e) override;
        public : void wheelEvent(QWheelEvent *e) override;
        public : void keyPressEvent(QKeyEvent *e) override;

        private: void _grayscaleToRainbow(const float gray, int &r, int &g, int &b) noexcept;

        private: void _drawArrow(
                GLfloat x1, GLfloat y1, GLfloat z1,
                GLfloat x2, GLfloat y2, GLfloat z2) noexcept;
        private: void _drawOrigin() noexcept;

        public : float _boundingBoxRepresentationSize = 1.0f;
        public : void setBoundingBoxRepresentationSize(const float &newRepresentationSize) noexcept{
            _boundingBoxRepresentationSize = newRepresentationSize;}
        private: void _drawBoundingBox() noexcept;

        private: GLuint _firstDisplayListID; // just increment for next list id
        private: GLuint _textureIDs[2];
        private: void _loadFieldIntoTexture() throw(std::runtime_error);
        private: void _prepareTextureDisplayList() noexcept;
        private: void _loadPotentialFieldIntoTexture() throw(std::runtime_error);
        private: void _preparePotentialTextureDisplayList() noexcept;
        private: unsigned char _potentialFieldAlphaLevel = 120;

        private: float _minPotentialValue;  // see constructor
        private: float _maxPotentialValue;
        private: const float _minPotentialValueBackup = 0;
        private: const float _maxPotentialValueBackup = 0;
        private: void _drawRainbowTable() noexcept;

        public : std::string printOpenGLInfo() const noexcept;

        public : VolumeGLRender(
                const int RVEDiscreteSize,
                const float * ptrToRVEData,
                const float * ptrToRVEPotentialField,
                QWidget *pwgt) noexcept;

        public : void initializeGL() override;
        public : void initializeGLEW();
        public : void resizeGL(int nWidth,int nHeight) override;
        public : void paintGL() override;

        public : ~VolumeGLRender();

        private Q_SLOTS: void slot_createFormatDialog();
    };
}

#endif // VOLUMEGLRENDER_H
