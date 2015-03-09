#include "volumeglrender.h"

#include <iostream>
#include <sstream>

VolumeGLRender::VolumeGLRender(
        const unsigned long RVEDiscreteSize,
        const unsigned char *ptrToRVEData,
        QWidget *pwgt) noexcept:
    QGLWidget(QGLFormat(QGL::SampleBuffers),pwgt),
    _RVEDiscretesize(RVEDiscreteSize),
    _ptrToRVEdata(ptrToRVEData)
{
}

void VolumeGLRender::mousePressEvent(QMouseEvent *e)
{
    _isPressed = true;
    _oldMouseX = e->x();
    _oldMouseY = e->y();
    this->updateGL();
}

void VolumeGLRender::mouseReleaseEvent(QMouseEvent *e)
{
    _isPressed = false;
}

void VolumeGLRender::mouseMoveEvent(QMouseEvent *e)
{
    if(_isPressed)
    {
        _angleOY += e->x()-_oldMouseX;
        _angleOX += e->y()-_oldMouseY;
        _oldMouseX = e->x();
        _oldMouseY = e->y();
        this->updateGL();
    }
}

void VolumeGLRender::wheelEvent(QWheelEvent *e)
{
    _Zoom += _Zoom * e->delta() / 5000;
    this->updateGL();
}

void VolumeGLRender::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Space)
    {
        //
    }
    else if(e->key() == Qt::Key_I)
    {
        //
    }
    else if(e->key() == Qt::Key_G)
    {
        std::cout << this->printOpenGLInfo();
    }
    else if(e->key() == Qt::Key_Z && e->modifiers() == Qt::ControlModifier)
    {
        //
    }
}

/// \warning Intel old video driver bug, when using glColor3ub(255, 0, 0);
/// see https://bugreports.qt.io/browse/QTBUG-6217
/// keep your video driver up to date!
void VolumeGLRender::_drawOrigin() noexcept
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

        glLoadIdentity();
        gluPerspective( 60.0f, 1.0, 0.1, 1000.0f );
        int _width = this->width();
        int _height = this->height();
        glViewport(0,0,100,100);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

            glLoadIdentity();
            gluLookAt(0, 0, 2,
                      0, 0, 0,
                      0, 1, 0);
            glRotated(_angleOY, 0, 1, 0);
            glRotated(_angleOX, 1, 0, 0);

            glLineWidth(3);

            glBegin(GL_LINES);
            glColor3d(1, 0, 0);
            glVertex3d(0, 0, 0);
            glVertex3d(1, 0, 0);
            glEnd();
            this->renderText(1, 0, 0, "x");

            glBegin(GL_LINES);
            glColor3d(0, 1, 0);
            glVertex3d(0, 0, 0);
            glVertex3d(0, 1, 0);
            glEnd();
            this->renderText(0, 1, 0, "y");

            glBegin(GL_LINES);
            glColor3d(0, 0, 1);
            glVertex3d(0, 0, 0);
            glVertex3d(0, 0, 1);
            glEnd();
            this->renderText(0, 0, 1, "z");

            glLineWidth(1);

        glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glViewport(0,0,_width,_height);
}

void VolumeGLRender::_loadFieldIntoTexture() throw(std::runtime_error)
{
    GLbyte *_RGBABuff = new GLbyte[_RVEDiscretesize * _RVEDiscretesize * _RVEDiscretesize * 4];
    if(!_RGBABuff)
        throw(std::runtime_error("FATAL: _loadFieldIntoTexture(): can't allocate memory for RVE"));

    for( unsigned long i = 0; i<_RVEDiscretesize * _RVEDiscretesize * _RVEDiscretesize; ++i)
    {
        _RGBABuff[i * 4 + 0] = _ptrToRVEdata[i];
        _RGBABuff[i * 4 + 1] = _ptrToRVEdata[i];
        _RGBABuff[i * 4 + 2] = _ptrToRVEdata[i];
        _RGBABuff[i * 4 + 3] = _ptrToRVEdata[i];
    }

    glGenTextures(1, &_fieldTextureID);
    glBindTexture(GL_TEXTURE_3D, _fieldTextureID);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // see https://www.opengl.org/sdk/docs/man3/xhtml/glTexImage3D.xml
    glTexImage3D(
                GL_TEXTURE_3D,      // target, copy data to device
                0,                  // level
                GL_RGBA,            // internalFormat
                _RVEDiscretesize,   // width
                _RVEDiscretesize,   // height
                _RVEDiscretesize,   // depth
                0,                  // border
                GL_RGBA,            // format
                GL_UNSIGNED_BYTE,   // type
                _RGBABuff           // ptr to data
                );
    delete[] _RGBABuff;
}

void VolumeGLRender::_prepareTextureDisplayList() noexcept
{
    _drawFieldDisplayListID = glGenLists(1);
    glNewList(_drawFieldDisplayListID, GL_COMPILE);
        glEnable(GL_TEXTURE_3D);
        glBindTexture( GL_TEXTURE_3D, _fieldTextureID);
        glBegin(GL_QUADS);
        for ( float _fIndx = -1.0f; _fIndx <= 1.0f; _fIndx+=0.01f )
        {
            glTexCoord3f(0.0f, 0.0f, (_fIndx + 1.0f) / 2.0f);
            glVertex3f(-1.0f, -1.0f, _fIndx);
            glTexCoord3f(1.0f, 0.0f, (_fIndx + 1.0f) / 2.0f);
            glVertex3f(1.0f, -1.0f, _fIndx);
            glTexCoord3f(1.0f, 1.0f, (_fIndx + 1.0f) / 2.0f);
            glVertex3f(1.0f, 1.0f, _fIndx);
            glTexCoord3f(0.0f, 1.0f, (_fIndx + 1.0f) / 2.0f);
            glVertex3f(-1.0f, 1.0f, _fIndx);
        }
        glEnd();
        glDisable(GL_TEXTURE_3D);
    glEndList();
}

std::string VolumeGLRender::printOpenGLInfo() const noexcept
{
    this->context()->makeCurrent();
    std::stringstream _str;
    _str
            << "OpenGL vendor  " << glGetString(GL_VENDOR) << std::endl
            << "       render  " << glGetString(GL_RENDERER) << std::endl
            << "       version " << glGetString(GL_VERSION) << std::endl
            << "       shaders " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl
            << "       ext.    " << glGetString(GL_EXTENSIONS) << std::endl;
    return _str.str();
}

void VolumeGLRender::initializeGL()
{
    glClearColor(
                _EnvironmentColor[0],
                _EnvironmentColor[1],
                _EnvironmentColor[2],
                _EnvironmentColor[3]);
//    glClearDepth(1.0f);
//    glEnable(GL_DEPTH_TEST);
//    //glEnable(GL_LINE_SMOOTH);
//    glDepthFunc(GL_LEQUAL);
//    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
//    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
//    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    QGLFormat aGLFormat;
    aGLFormat.setSampleBuffers(true);
    aGLFormat.setVersion(aGLFormat.majorVersion(),aGLFormat.minorVersion());
    QGLFormat::setDefaultFormat(aGLFormat);

    glEnable( GL_ALPHA_TEST );
    glAlphaFunc( GL_GREATER, 0.05f );

    glEnable(GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

//    GLfloat light0_diffuse[] = {1.0, 1.0, 1.0};
//    GLfloat light0_direction[] = {0.0, 0.0, 1.0, 0.0};
//    glEnable(GL_LIGHTING);
//    glEnable(GL_LIGHT0);
//    glEnable(GL_COLOR_MATERIAL);
//    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, light0_diffuse);
//    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
//    glEnable(GL_NORMALIZE);
//    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
//    glLightfv(GL_LIGHT0, GL_POSITION, light0_direction);
}

void VolumeGLRender::initializeGLEW()
{
    this->context()->makeCurrent();

    auto _rez = glewInit();
    std::cout << "GLEW: "<< glewGetErrorString(_rez) << std::endl;

    _loadFieldIntoTexture();
    _prepareTextureDisplayList();
}

void VolumeGLRender::resizeGL(int nWidth, int nHeight)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 60.0f, ((GLfloat)nWidth)/(GLfloat)nHeight, 0.1, 1000.0f );
    glViewport(0,0,(GLint)nWidth,(GLint)nHeight);
}

void VolumeGLRender::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _drawOrigin();

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glTranslatef( 0.5f, 0.5f, 0.5f );   // put to center
    glRotated(-_angleOY, 0, 1, 0);
    glRotated(-_angleOX, 1, 0, 0);
    glScaled(1/_Zoom, 1/_Zoom, 1/_Zoom);
    glTranslatef( -0.5f,-0.5f, -0.5f ); // put back

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(_CameraPosition[0], _CameraPosition[1], _CameraPosition[2],
              0, 0, 0,
              0, 1, 0);
    glTranslated(-_ScenePosition[0], -_ScenePosition[1], -_ScenePosition[2]);

    glCallList(_drawFieldDisplayListID);

    glColor4d(
            _TextColor[0],
            _TextColor[1],
            _TextColor[2],
            _TextColor[3]);
    this->renderText(1.0, 10.0, dataString);
}
