#ifndef XYGLRENDER_H
#define XYGLRENDER_H

#include <_mingw.h> // __int64
#include <GL/glew.h>

#include <QtEvents>
#include <QGLWidget>

#include <QMenu>

#include "UI/xyglrenderformatdialog.h"

namespace UserInterface
{
    /// \todo refactoring, merege with VolumeGLRenderBaseController
    class XYGLRender : public QGLWidget
    {
        Q_OBJECT

        friend class UserInterface::XYGLRenderFormatDialog;

        private  : const std::vector<float (*)(float)> &_functions;

        protected: std::vector<QColor> _functionColors;
        public   : void setFunctionColor(const int index, const QColor &newColor) noexcept{
                    _functionColors[index] = newColor;}

        protected: QColor _gridColor = QColor(127, 127, 127, 255);
        public   : void setGridColor(const QColor &newColor) noexcept{
                    _gridColor = newColor;}

        protected: int _oldMouseX = 0;
        protected: int _oldMouseY = 0;
        protected: bool _isPressed = false;

        private  : float _OXOffset = 0;
        private  : float _OYOffset = 0;

        protected: QColor _BackgroundColor = QColor(64, 128, 128, 255);
        public   : void setEnvironmenColor(const QColor &newColor) noexcept{
                    _BackgroundColor = newColor;}

        protected: QFont _TextFont = QFont();
        public   : void setTextFont(const QFont &newFont){
                    _TextFont = newFont;}
        protected: QColor _TextColor = QColor(0, 255, 0, 255);
        public   : void setTextColor(const QColor &newColor){
                    _TextColor = newColor;}
        protected: QString _infoString = "";
        public   : void setInfoString(const QString &newString) noexcept{
                    _infoString = newString;}

//        protected: QVector3D _CameraPosition = QVector3D(0.0f, 0.0f, 2.0f);
//        public   : void setCameraPosition(const QVector3D &newPosition) noexcept {
//                    _CameraPosition = newPosition;}
//        public   : QVector3D getCameraPosition() const noexcept {return _CameraPosition;}

        protected: float _Zoom = 1.0f;
        public   : void setZoom(const float &newZoom) noexcept {
                    _Zoom = newZoom;}
        public   : float getZoom() const noexcept {return _Zoom;}

//        protected: void _renderMultilineText(
//                    int x, int y, const QString &text, const QFont &f = QFont()) noexcept;

        private  : int _OXLinesNum = 20;
        public   : void setOXLinesNum(int newLinesNumber) noexcept {_OXLinesNum = newLinesNumber;}
        public   : int getOXLinesNumber() const noexcept {return _OXLinesNum;}
        private  : int _OYLinesNum = 20;    /// \todo it local, remove
//        public   : void setOYLinesNum(int newLinesNumber) noexcept {_OYLinesNum = newLinesNumber;}
//        public   : int getOYLinesNumber() const noexcept {return _OYLinesNum;}
        protected: void _drawGrid() noexcept;

        private  : int _nodesNum = 100;
        public   : void setNodesNumber(int newNodesNumber) noexcept {_nodesNum = newNodesNumber;}
        public   : int getNodesNumber() const noexcept {return _nodesNum;}
        private  : void _drawFunctions() noexcept;

        public   : virtual void mousePressEvent(QMouseEvent *e) override;
        public   : virtual void mouseReleaseEvent(QMouseEvent *e) override;
        public   : virtual void mouseMoveEvent(QMouseEvent *e) override;
        public   : virtual void wheelEvent(QWheelEvent *e) override;

        public   : virtual void initializeGL() override;
        public   : virtual void resizeGL(int nWidth,int nHeight) override;

        public   : virtual void paintGL() override;

        public   : XYGLRender(
                const std::vector<float (*)(float)> &functions,
                QWidget *pwgt) noexcept;
        public   :~XYGLRender();

        protected: QMenu *_contextMenu;
        protected: QAction* _actionFormat;

        public   : Q_SLOT void slot_createFormatDialog();
    };
}

#endif // XYGLRENDER_H
