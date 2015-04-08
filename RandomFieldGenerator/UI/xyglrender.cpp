#include "xyglrender.h"

#include "FUNCTIONS/rand.h"
#include "FUNCTIONS/trunc.h"

using namespace UserInterface;

XYGLRender::XYGLRender(
        const std::vector<Function> *functions,
        const std::vector<NodalFunction> *nodalFunctions,
        QWidget *pwgt) throw(std::runtime_error) :
    QGLWidget(QGLFormat(QGL::SampleBuffers),pwgt),
    _functions(functions),
    _nodalFunctions(nodalFunctions),
    _contextMenu(new QMenu(this)),
    _actionFormat(new QAction("Format...", this))
{
    if(_functions && _nodalFunctions)
        _functionColors.resize(functions->size() + nodalFunctions->size());
    else if(_functions)
        _functionColors.resize(functions->size());
    else if(_nodalFunctions)
        _functionColors.resize(nodalFunctions->size());
    else throw(std::runtime_error("Error XYGLRender(): no functions to plot is given\n"));

    for(int i=0; i<_functionColors.size(); ++i)
        _functionColors[i] = QColor(
                    MathUtils::rand<int>(0,255),
                    MathUtils::rand<int>(0,255),
                    MathUtils::rand<int>(0,255),
                    255);

    connect(_actionFormat, SIGNAL(triggered()), this, SLOT(slot_createFormatDialog()));
    _contextMenu->addAction(_actionFormat);
    _contextMenu->addSeparator();

    initializeGL();
}

void XYGLRender::mousePressEvent(QMouseEvent *e)
{
    _isPressed = true;
    _oldMouseX = e->x();
    _oldMouseY = e->y();
    this->updateGL();
}

void XYGLRender::mouseReleaseEvent(QMouseEvent *e)
{
    _isPressed = false;
    if(e->button() == Qt::RightButton)
    {
        _contextMenu->exec(mapToGlobal(e->pos()));
    }
}

void XYGLRender::mouseMoveEvent(QMouseEvent *e)
{
    if(_isPressed)
    {
        _OXOffset += (e->x() - _oldMouseX) / 500.0f;
        _OYOffset += (e->y() - _oldMouseY) / 500.0f;

        _oldMouseX = e->x();
        _oldMouseY = e->y();

        this->updateGL();
    }
}

void XYGLRender::wheelEvent(QWheelEvent *e)
{
    _Zoom += _Zoom * e->delta() / 5000.0f;
    this->updateGL();
}

void XYGLRender::_drawGrid() noexcept
{
    glColor4f(
            _gridColor.redF(),
            _gridColor.greenF(),
            _gridColor.blueF(),
            _gridColor.alphaF());

    int _exp;
    std::frexp((-1.0f*width()/height() - _OXOffset)/_Zoom, &_exp);
    float _flor = 0.001f * std::pow(2,_exp);

    float _baseX1 = MathUtils::trunc<float>((-1.0f*width()/height() - _OXOffset)/_Zoom, _flor);
    float _baseX2 = MathUtils::trunc<float>(( 1.0f*width()/height() - _OXOffset)/_Zoom, _flor);

    // warning! it inverted
    float _baseY1 = MathUtils::trunc<float>(-(-1.0f - _OYOffset)/_Zoom, _flor);
    float _baseY2 = MathUtils::trunc<float>(-( 1.0f - _OYOffset)/_Zoom, _flor);

    float _stepX = (_baseX2 - _baseX1) / _OXLinesNum;
    float _stepY = -_stepX;
    _OYLinesNum = (_baseY2 - _baseY1) / _stepY + 1;

    glBegin(GL_LINES);
    for(int i=0; i<_OXLinesNum; ++i)
    {
        glVertex2f(_baseX1 + i*_stepX, _baseY1);
        glVertex2f(_baseX1 + i*_stepX, _baseY2);
    }
    for(int i=0; i<_OYLinesNum; ++i)
    {
        glVertex2f(_baseX1, _baseY1 + i*_stepY);
        glVertex2f(_baseX2, _baseY1 + i*_stepY);
    }
    glEnd();

    if(_baseX1 <0 && _baseX2>0)
    {
        glBegin(GL_LINES);
        glColor4f(
                    1.0f - _BackgroundColor.redF(),
                    1.0f - _BackgroundColor.greenF(),
                    1.0f - _BackgroundColor.blueF(),
                    1.0f);
        glVertex2f(0, _baseY1);
        glVertex2f(0, _baseY2);
        glEnd();
    }

    // warning! it inverted
    if(_baseY2 <0 && _baseY1>0)
    {
        glBegin(GL_LINES);
        glColor4f(
                    1.0f - _BackgroundColor.redF(),
                    1.0f - _BackgroundColor.greenF(),
                    1.0f - _BackgroundColor.blueF(),
                    1.0f);
        glVertex2f(_baseX1, 0);
        glVertex2f(_baseX2, 0);
        glEnd();
    }

    glColor4f(
            _TextColor.redF(),
            _TextColor.greenF(),
            _TextColor.blueF(),
            _TextColor.alphaF());
    // warning! it inverted
    float _y = 0;
    if(_baseY2-_stepY <0 && _baseY1+_stepY>0)
        _y = 0;
    else if(_baseY2-_stepY<=0 && _baseY1+_stepY<0)
        _y = _baseY1 + _stepY;
    else _y = _baseY2 - _stepY;
    for(int i=0; i<_OXLinesNum; ++i)
        renderText(
                    _baseX1 + i*_stepX,
                    _y,
                    0,
                    QString::number(_baseX1 + i*_stepX), _TextFont);

    float _x = 0;
    if(_baseX1+_stepX <0 && _baseX2-2*_stepX>0)
        _x = 0;
    else if(_baseX1+_stepX<0 && _baseX2-2*_stepX<=0)
        _x = _baseX2 - 2*_stepX;
    else _x = _baseX1 + _stepX;
    for(int i=0; i<_OYLinesNum; ++i)
        renderText( _x,
                    _baseY1 + i*_stepY,
                    0,
                    QString::number(_baseY1 + i*_stepY), _TextFont);
}

void XYGLRender::_drawFunctions() noexcept
{
    if(_functions)
        for(int f=0; f<_functions->size(); ++f)
        {
            glColor4f(
                        _functionColors[f].redF(),
                        _functionColors[f].greenF(),
                        _functionColors[f].blueF(),
                        _functionColors[f].alphaF());

            float _baseX1 = (-1.0f*width()/height() - _OXOffset)/_Zoom;
            float _baseX2 = ( 1.0f*width()/height() - _OXOffset)/_Zoom;

            float _stepX = (_baseX2 - _baseX1) / (_nodesNum-1);
            glBegin(GL_LINE_STRIP);
            for(int i=0; i<_nodesNum; ++i)
                glVertex2f(
                            _baseX1 + i*_stepX,
                            (*_functions)[f].implementation(_baseX1 + i*_stepX));
            glEnd();
        }
}

void XYGLRender::_drawNodalFunctions() noexcept
{
    if(_nodalFunctions)
    {
        int _offset = 0;
        if(_functions)
            _offset = _functions->size();
        for(int f=0; f<_nodalFunctions->size(); ++f)
        {
            glColor4f(
                        _functionColors[f+_offset].redF(),
                        _functionColors[f+_offset].greenF(),
                        _functionColors[f+_offset].blueF(),
                        _functionColors[f+_offset].alphaF());

            glBegin(GL_LINE_STRIP);
            for(int i=0; i<(*_nodalFunctions)[f].nodes.size(); ++i)
                glVertex2f(
                            (*_nodalFunctions)[f].nodes[i][0],
                        (*_nodalFunctions)[f].nodes[i][1]);
            glEnd();
        }
    }
}

void XYGLRender::initializeGL()
{
    QGLFormat aGLFormat;
    aGLFormat.setSampleBuffers(true);
    aGLFormat.setVersion(aGLFormat.majorVersion(),aGLFormat.minorVersion());
    QGLFormat::setDefaultFormat(aGLFormat);

    glClearDepth(1.0f);
//    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glDepthFunc(GL_LEQUAL);

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);

//    _mControl.setToIdentity();
//    _mWorld.setToIdentity();
//    _mWorld.lookAt(QVector3D(_CameraPosition[0],_CameraPosition[1],_CameraPosition[2]),
//            QVector3D(0,0,0),QVector3D(0,1,0));
}

void XYGLRender::resizeGL(int nWidth, int nHeight)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 45.0f, ((GLfloat)nWidth)/(GLfloat)nHeight, 0.01f, 1000.0f );
    glViewport(0,0,(GLint)nWidth,(GLint)nHeight);
}

void XYGLRender::paintGL()
{
    glClearColor(
                _BackgroundColor.redF(),
                _BackgroundColor.greenF(),
                _BackgroundColor.blueF(),
                _BackgroundColor.alphaF());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
                0,0,2.2,
                0,0,0,
                0,1,0);
    glScalef(_Zoom, _Zoom, _Zoom);
    glTranslatef(_OXOffset/_Zoom, -_OYOffset/_Zoom, 0);

    _drawGrid();

    _drawFunctions();
    _drawNodalFunctions();

//    glColor4f(
//            _TextColor.redF(),
//            _TextColor.greenF(),
//            _TextColor.blueF(),
//            _TextColor.alphaF());
//    _renderMultilineText(2, 0, _infoString, _TextFont);
}

XYGLRender::~XYGLRender()
{

}

void XYGLRender::slot_createFormatDialog()
{
    UserInterface::XYGLRenderFormatDialog *_dialog =
            new UserInterface::XYGLRenderFormatDialog(this);
    _dialog->show();
}
