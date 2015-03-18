#include "volumeglrender.h"

#include <iostream>
#include <sstream>

//QQuaternion _quat;

VolumeGLRender::VolumeGLRender(
        const int RVEDiscreteSize,
        const float *ptrToRVEData,
        const float *ptrToRVEPotentialField,
        QWidget *pwgt) noexcept:
    QGLWidget(QGLFormat(QGL::SampleBuffers),pwgt),
    _RVEDiscretesize(RVEDiscreteSize),
    _ptrToRVEdata(ptrToRVEData),
    _ptrToRVEpotentialField(ptrToRVEPotentialField)
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

        QMatrix4x4 _m;
        _m.setToIdentity();
        _m.rotate(e->x()-_oldMouseX, 0, 1, 0);
        _m.rotate(e->y()-_oldMouseY, 1, 0, 0);
        _mControl = _m * _mControl;

//        _m.setToIdentity();
//        _m.rotate(-(e->x()-_oldMouseX), 0, 1, 0);
//        _m.rotate(-(e->y()-_oldMouseY), 1, 0, 0);
//        _mControlRev = _m * _mControlRev;

//        _quat = QQuaternion::fromAxisAndAngle(1, 0, 0, e->y()-_oldMouseY) *
//            QQuaternion::fromAxisAndAngle(0, 1, 0, e->x()-_oldMouseX) * _quat;
//        _quat.normalize();

        _oldMouseX = e->x();
        _oldMouseY = e->y();
        this->updateGL();
    }
}

void VolumeGLRender::wheelEvent(QWheelEvent *e)
{
    _Zoom += _Zoom * e->delta() / 5000;

//    _mControl.scale(_Zoom);

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
    else if(e->key() == Qt::Key_Up)
    {
        if(_innerCutLevel < 1.0f)
        {
            _innerCutLevel += 0.01f;
            _loadFieldIntoTexture();
            dataString = "Cut level: " + QString::number(_innerCutLevel);
            this->updateGL();
        }
    }
    else if(e->key() == Qt::Key_Down)
    {
        if(_innerCutLevel > 0.0f)
        {
            _innerCutLevel -= 0.01f;
            _loadFieldIntoTexture();
            dataString = "Cut level: " + QString::number(_innerCutLevel);
            this->updateGL();
        }
    }
    else if(e->key() == Qt::Key_PageUp)
    {
        if(_potentialFieldAlphaLevel<255)
        {
            _potentialFieldAlphaLevel+=5;
            _loadPotentialFieldIntoTexture();
            dataString = "Alpha level: " + QString::number(_potentialFieldAlphaLevel);
            this->updateGL();
        }
    }
    else if(e->key() == Qt::Key_PageDown)
    {
        if(_potentialFieldAlphaLevel>0)
        {
            _potentialFieldAlphaLevel-=5;
            _loadPotentialFieldIntoTexture();
            dataString = "Alpha level: " + QString::number(_potentialFieldAlphaLevel);
            this->updateGL();
        }
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
        gluPerspective( 60.0f, 160.0/120.0, 0.1, 1000.0f );
        int _width = this->width();
        int _height = this->height();
        glViewport(0,0,160,120);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

//            glLoadIdentity();
//            gluLookAt(0, 0, 2,
//                      0, 0, 0,
//                      0, 1, 0);
//            glRotated(_angleOY, 0, 1, 0);
//            glRotated(_angleOX, 1, 0, 0);

            glLoadMatrixf((_mWorld * _mControl).constData());

            glEnable(GL_DEPTH_TEST);

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

            glDisable(GL_DEPTH_TEST);

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

    for(long i = 0; i<_RVEDiscretesize * _RVEDiscretesize * _RVEDiscretesize; ++i)
    {
        if(_ptrToRVEdata[i] > _innerCutLevel)
        {
//            int r,g,b;
//            float a = (1.0f-_ptrToRVEdata[i])*4.0f; //invert and group
//            int X = std::floor(a);                  //this is the integer part
//            int Y = std::floor(255*(a-X));          //fractional part from 0 to 255
//            switch(X)
//            {
//                case 0: r = 255;    g=Y;        b=0;    break;
//                case 1: r = 255-Y;  g=255;      b=0;    break;
//                case 2: r = 0;      g=255;      b=Y;    break;
//                case 3: r = 0;      g=255-Y;    b=255;  break;
//                case 4: r = 0;      g=0;        b=255;  break;
//            }

//            _RGBABuff[i * 4 + 0] = r;
//            _RGBABuff[i * 4 + 1] = g;
//            _RGBABuff[i * 4 + 2] = b;
//            _RGBABuff[i * 4 + 3] = 255;

            _RGBABuff[i * 4 + 0] = 255;
            _RGBABuff[i * 4 + 1] = 255;
            _RGBABuff[i * 4 + 2] = 255;
            _RGBABuff[i * 4 + 3] = 255;
        }
        else
        {
            _RGBABuff[i * 4 + 0] = 0;
            _RGBABuff[i * 4 + 1] = 0;
            _RGBABuff[i * 4 + 2] = 0;
            _RGBABuff[i * 4 + 3] = 0;
        }
    }

    glBindTexture(GL_TEXTURE_3D, _fieldTextureID[0]);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
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
    glNewList(_drawFieldDisplayListID, GL_COMPILE);
    glEnable(GL_TEXTURE_3D);
    glBindTexture( GL_TEXTURE_3D, _fieldTextureID[0]);
    glBegin(GL_QUADS);
    for ( float _fIndx = 0.0f; _fIndx <= 1.0f; _fIndx += 1.0 / _RVEDiscretesize )
    {
        glTexCoord3f(0.0f, 0.0f, _fIndx);
        glVertex3f(0.0f, 0.0f, _fIndx);

        glTexCoord3f(1.0f, 0.0f, _fIndx);
        glVertex3f(1.0f, 0.0f, _fIndx);

        glTexCoord3f(1.0f, 1.0f, _fIndx);
        glVertex3f(1.0f, 1.0f, _fIndx);

        glTexCoord3f(0.0f, 1.0f, _fIndx);
        glVertex3f(0.0f, 1.0f, _fIndx);
    }
    glEnd();
    glDisable(GL_TEXTURE_3D);
    glEndList();
}

void VolumeGLRender::_loadPotentialFieldIntoTexture() throw(std::runtime_error)
{
    GLbyte *_RGBABuff = new GLbyte[_RVEDiscretesize * _RVEDiscretesize * _RVEDiscretesize * 4];
    if(!_RGBABuff)
        throw(std::runtime_error("FATAL: _loadFieldIntoTexture(): can't allocate memory for RVE"));

    for(long i = 0; i<_RVEDiscretesize * _RVEDiscretesize * _RVEDiscretesize; ++i)
    {
//        if(_ptrToRVEpotentialField[i] > _innerCutLevel)
//        {
            int r,g,b;
            float a = (1.0f-_ptrToRVEpotentialField[i])*4.0f;   //invert and group
            int X = std::floor(a);                              //this is the integer part
            int Y = std::floor(255*(a-X));                      //fractional part from 0 to 255
            switch(X)
            {
                case 0: r = 255;    g=Y;        b=0;    break;
                case 1: r = 255-Y;  g=255;      b=0;    break;
                case 2: r = 0;      g=255;      b=Y;    break;
                case 3: r = 0;      g=255-Y;    b=255;  break;
                case 4: r = 0;      g=0;        b=255;  break;
            }

            _RGBABuff[i * 4 + 0] = r;
            _RGBABuff[i * 4 + 1] = g;
            _RGBABuff[i * 4 + 2] = b;
            _RGBABuff[i * 4 + 3] = _potentialFieldAlphaLevel;
//        }
//        else
//        {
//            _RGBABuff[i * 4 + 0] = 0;
//            _RGBABuff[i * 4 + 1] = 0;
//            _RGBABuff[i * 4 + 2] = 0;
//            _RGBABuff[i * 4 + 3] = 0;
//        }
    }

    glBindTexture(GL_TEXTURE_3D, _fieldTextureID[1]);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
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

void VolumeGLRender::_preparePotentialTextureDisplayList() noexcept
{
    glNewList(_drawFieldDisplayListID+1, GL_COMPILE);
        glEnable(GL_TEXTURE_3D);
        glBindTexture( GL_TEXTURE_3D, _fieldTextureID[1]);
        glBegin(GL_QUADS);
        for ( float _fIndx = 0.0f; _fIndx <= 1.0f; _fIndx += 1.0 / _RVEDiscretesize )
        {
            glTexCoord3f(0.0f, 0.0f, _fIndx);
            glVertex3f(0.0f, 0.0f, _fIndx);

            glTexCoord3f(1.0f, 0.0f, _fIndx);
            glVertex3f(1.0f, 0.0f, _fIndx);

            glTexCoord3f(1.0f, 1.0f, _fIndx);
            glVertex3f(1.0f, 1.0f, _fIndx);

            glTexCoord3f(0.0f, 1.0f, _fIndx);
            glVertex3f(0.0f, 1.0f, _fIndx);
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

//    glEnable( GL_ALPHA_TEST );
//    glAlphaFunc( GL_GEQUAL, 0.5f );

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

    _mTexture.setToIdentity();
    _mModel.setToIdentity();
    _mControl.setToIdentity();
//    _mControlRev.setToIdentity();
    _mWorld.setToIdentity();
//    _mProj.setToIdentity();

    _mModel.translate(-0.5,-0.5,-0.5);  /// \todo move it out
    _mWorld.lookAt(QVector3D(_CameraPosition[0],_CameraPosition[1],_CameraPosition[2]),
            QVector3D(0,0,0),QVector3D(0,1,0));
//    _mProj.perspective( 60.0f, 4.0/3.0, 0.1f, 1000.0f );
}

void VolumeGLRender::initializeGLEW()
{
    this->context()->makeCurrent();

    auto _rez = glewInit();
    std::cout << "GLEW: "<< glewGetErrorString(_rez) << std::endl;

    glGenTextures(2, _fieldTextureID);
    _drawFieldDisplayListID = glGenLists(2);
    _loadFieldIntoTexture();
    _prepareTextureDisplayList();
    _loadPotentialFieldIntoTexture();
    _preparePotentialTextureDisplayList();
}

void VolumeGLRender::resizeGL(int nWidth, int nHeight)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 60.0f, ((GLfloat)nWidth)/(GLfloat)nHeight, 0.1, 1000.0f );
//    _mProj.perspective( 60.0f, ((GLfloat)nWidth)/(GLfloat)nHeight, 0.1, 1000.0f );
    glViewport(0,0,(GLint)nWidth,(GLint)nHeight);
}

void VolumeGLRender::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _drawOrigin();

//    glMatrixMode(GL_MODELVIEW);
//    glLoadMatrixf((_mWorld * _mControl).constData());;

//    glEnable(GL_DEPTH_TEST);

//    glLineWidth(3);

//    glBegin(GL_LINES);
//    glColor3d(1, 0, 0);
//    glVertex3d(0, 0, 0);
//    glVertex3d(1, 0, 0);
//    glEnd();
//    this->renderText(1, 0, 0, "x");

//    glBegin(GL_LINES);
//    glColor3d(0, 1, 0);
//    glVertex3d(0, 0, 0);
//    glVertex3d(0, 1, 0);
//    glEnd();
//    this->renderText(0, 1, 0, "y");

//    glBegin(GL_LINES);
//    glColor3d(0, 0, 1);
//    glVertex3d(0, 0, 0);
//    glVertex3d(0, 0, 1);
//    glEnd();
//    this->renderText(0, 0, 1, "z");

//    glLineWidth(1);

//    glDisable(GL_DEPTH_TEST);

//    glMatrixMode(GL_PROJECTION);
//    glLoadMatrixf(_mProj.constData());

/*    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf((_mWorld * _mModel).constData());
//    glLoadMatrixf((_mWorld * _mControl * _mModel).constData());

    glMatrixMode(GL_TEXTURE);

//    QMatrix4x4 _m;
//    _m.setToIdentity();
//    _m.lookAt(QVector3D(0,0,0),QVector3D(0,0,-1),QVector3D(0,1,0));

    _mTexture.setToIdentity();

    _mTexture.translate(0.5f, 0.5f, 0.5f);
    _mTexture.scale(1/_Zoom, 1/_Zoom, 1/_Zoom);

    _mTexture.rotate(-_angleOX, 1, 0, 0);
    _mTexture.rotate(-_angleOY, 0, 1, 0);

//    _mTexture = _mTexture * _mControl;

////    _mTexture.rotate(_quat);

    _mTexture.translate(-0.5f, -0.5f, -0.5f);

//    _mTexture = _m * _mTexture;
    glLoadMatrixf(_mTexture.constData());*/

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glTranslatef( 0.5f, 0.5f, 0.5f );   // put to center
    glRotated(-_angleOX, 1, 0, 0);
    glRotated(-_angleOY, 0, 1, 0);
    glScaled(1/_Zoom, 1/_Zoom, 1/_Zoom);
    glTranslatef( -0.5f,-0.5f, -0.5f ); // put back

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 1,
              0, 0, 0,
              0, 1, 0);
    glTranslatef( -0.5f, -0.5f, -0.5f );

    glCallList(_drawFieldDisplayListID);
    glCallList(_drawFieldDisplayListID+1);

    glColor4d(
            _TextColor[0],
            _TextColor[1],
            _TextColor[2],
            _TextColor[3]);
    this->renderText(1.0, 10.0, dataString);
}
