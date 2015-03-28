#include "volumeglrender.h"

#include <iostream>

#include <QMenu>
#include <QDialog>

using namespace UserInterface;

VolumeGLRender::VolumeGLRender(
        const int RVEDiscreteSize,
        const float *ptrToRVEData,
        const float *ptrToRVEPotentialField,
        QWidget *pwgt) noexcept:
    VolumeGLRenderBaseController(pwgt),
    _RVEDiscretesize(RVEDiscreteSize),
    _ptrToRVEdata(ptrToRVEData),
    _ptrToRVEpotentialField(ptrToRVEPotentialField)
{
    _minPotentialValue = _ptrToRVEpotentialField[0];
    _maxPotentialValue = _ptrToRVEpotentialField[0];
    for(long i = 1; i<_RVEDiscretesize * _RVEDiscretesize * _RVEDiscretesize; ++i)
    {
        if(_ptrToRVEpotentialField[i] < _minPotentialValue)
            _minPotentialValue = _ptrToRVEpotentialField[i];
        if(_ptrToRVEpotentialField[i] > _maxPotentialValue)
            _maxPotentialValue = _ptrToRVEpotentialField[i];
    }
    (*const_cast<float*>(&_minPotentialValueBackup)) = _minPotentialValue;
    (*const_cast<float*>(&_maxPotentialValueBackup)) = _maxPotentialValue;

    initializeGLEW();
    setWindowTitle("Volume render");
}

void VolumeGLRender::mouseReleaseEvent(QMouseEvent *e)
{
    _isPressed = false;

    if(e->button() == Qt::RightButton)
    {
        QMenu _menu;

        QAction* _ActionFormat = new QAction("Format...", this);
        connect(_ActionFormat, SIGNAL(triggered()), this, SLOT(slot_createFormatDialog()));
        _menu.addAction(_ActionFormat);

        _menu.addSeparator();
        _menu.exec(mapToGlobal(e->pos()));
    }
//    QGLWidget::mouseReleaseEvent(event);  //Dont forget to pass on the event to parent
}

void VolumeGLRender::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Up)
    {
        if(_innerCutLevel < 1.0f)
        {
            _innerCutLevel += 0.01f;
            _loadFieldIntoTexture();
            _infoString = "Cut level: " + QString::number(_innerCutLevel);
            this->updateGL();
        }
    }
    else if(e->key() == Qt::Key_Down)
    {
        if(_innerCutLevel > 0.0f)
        {
            _innerCutLevel -= 0.01f;
            _loadFieldIntoTexture();
            _infoString = "Cut level: " + QString::number(_innerCutLevel);
            this->updateGL();
        }
    }
}

void VolumeGLRender::_grayscaleToRainbow(const float gray, int &r, int &g, int &b) noexcept
{
    float _inv = (1.0f-gray)*4.0f;      //invert and group
    int _X = std::floor(_inv);          //this is the integer part
    int _Y = std::floor(255*(_inv-_X)); //fractional part from 0 to 255
    switch(_X)
    {
        case 0: r = 255;        g = _Y;         b = 0;     break;
        case 1: r = 255 - _Y;   g = 255;        b = 0;     break;
        case 2: r = 0;          g = 255;        b = _Y;    break;
        case 3: r = 0;          g = 255 - _Y;   b = 255;   break;
        case 4: r = 0;          g = 0;          b = 255;   break;
    }
}

void VolumeGLRender::_loadFieldIntoTexture() throw(std::runtime_error)
{
    GLbyte *_RGBABuff = new GLbyte[_RVEDiscretesize * _RVEDiscretesize * _RVEDiscretesize * 4];
    if(!_RGBABuff)
        throw(std::runtime_error("FATAL: _loadFieldIntoTexture(): can't allocate memory for RVE"));

    for(long i = 0; i<_RVEDiscretesize * _RVEDiscretesize * _RVEDiscretesize; ++i)
    {
        _RGBABuff[i * 4 + 0] = 255;
        _RGBABuff[i * 4 + 1] = 255;
        _RGBABuff[i * 4 + 2] = 255;
        _RGBABuff[i * 4 + 3] = (_ptrToRVEdata[i] > _innerCutLevel) ? 255 : 0;
    }

    glBindTexture(GL_TEXTURE_3D, _textureIDs[0]);
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
    glNewList(_firstDisplayListID, GL_COMPILE);
    glEnable(GL_TEXTURE_3D);
    glBindTexture( GL_TEXTURE_3D, _textureIDs[0]);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);  // material color (for lightning)
    glBegin(GL_QUADS);
    for ( float _fIndx = 0.0f; _fIndx <= 1.0f; _fIndx += 1.0 / _RVEDiscretesize)
    {
        glNormal3f(0.0f, 0.0f, 1.0f);

        glTexCoord3f(0.0f, 0.0f, _fIndx);   glVertex3f(0.0f, 0.0f, _fIndx);
        glTexCoord3f(1.0f, 0.0f, _fIndx);   glVertex3f(1.0f, 0.0f, _fIndx);
        glTexCoord3f(1.0f, 1.0f, _fIndx);   glVertex3f(1.0f, 1.0f, _fIndx);
        glTexCoord3f(0.0f, 1.0f, _fIndx);   glVertex3f(0.0f, 1.0f, _fIndx);

        //////////////////////////////////
        glNormal3f(0.0f, 1.0f, 0.0f);

        glTexCoord3f(1.0f, _fIndx, 0.0f);   glVertex3f(1.0f, _fIndx, 0.0f);
        glTexCoord3f(0.0f, _fIndx, 0.0f);   glVertex3f(0.0f, _fIndx, 0.0f);
        glTexCoord3f(0.0f, _fIndx, 1.0f);   glVertex3f(0.0f, _fIndx, 1.0f);
        glTexCoord3f(1.0f, _fIndx, 1.0f);   glVertex3f(1.0f, _fIndx, 1.0f);

        //////////////////////////////////
        glNormal3f(1.0f, 0.0f, 0.0f);

        glTexCoord3f(_fIndx, 0.0f, 0.0f);   glVertex3f(_fIndx, 0.0f, 0.0f);
        glTexCoord3f(_fIndx, 1.0f, 0.0f);   glVertex3f(_fIndx, 1.0f, 0.0f);
        glTexCoord3f(_fIndx, 1.0f, 1.0f);   glVertex3f(_fIndx, 1.0f, 1.0f);
        glTexCoord3f(_fIndx, 0.0f, 1.0f);   glVertex3f(_fIndx, 0.0f, 1.0f);
    }
    glEnd();
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_3D);
    glEndList();
}

void VolumeGLRender::_loadPotentialFieldIntoTexture() throw(std::runtime_error)
{
    GLbyte *_RGBABuff = new GLbyte[_RVEDiscretesize * _RVEDiscretesize * _RVEDiscretesize * 4];
    if(!_RGBABuff)
        throw(std::runtime_error("FATAL: _loadFieldIntoTexture(): can't allocate memory for RVE"));

    float _delta = _maxPotentialValue - _minPotentialValue;
    for(long i = 0; i<_RVEDiscretesize * _RVEDiscretesize * _RVEDiscretesize; ++i)
    {
        int _r, _g, _b;
        float _val = (_ptrToRVEpotentialField[i] - _minPotentialValue) / _delta;
        if(_val >= 0.0f && _val <= 1.0f)
            _RGBABuff[i * 4 + 3] = _potentialFieldAlphaLevel;
        else
        {
            if(_val < 0.0f) _val = 0.0f;
            if(_val > 1.0f) _val = 1.0f;
            _RGBABuff[i * 4 + 3] = 0;
        }

        _grayscaleToRainbow(_val, _r, _g, _b);

        _RGBABuff[i * 4 + 0] = _r;
        _RGBABuff[i * 4 + 1] = _g;
        _RGBABuff[i * 4 + 2] = _b;
    }

    glBindTexture(GL_TEXTURE_3D, _textureIDs[1]);
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
    /// see https://mail.gnome.org/archives/commits-list/2012-February/msg05204.html
    glNewList(_firstDisplayListID+1, GL_COMPILE);
    glEnable(GL_TEXTURE_3D);
    glBindTexture(GL_TEXTURE_3D, _textureIDs[1]);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glEnable(GL_DEPTH_TEST);
    for (int _i = 0; _i<2; ++_i)
    {
        if (_i == 0)
        {
            // Clear alpha buffer
            glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glBlendFuncSeparate(GL_ZERO, GL_ONE, GL_ONE, GL_ZERO);
        }
        else if (_i == 1)
        {
            // Draw pixels
            glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
            glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ZERO, GL_ZERO);
        }
        else
        {
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        }

        glBegin(GL_QUADS);
        for ( float _fIndx = 0.0f; _fIndx <= 1.0f; _fIndx += 1.0 / _RVEDiscretesize)
        {
    //        glNormal3f(0.0f, 0.0f, 1.0f);
            glTexCoord3f(0.0f, 0.0f, _fIndx);   glVertex3f(0.0f, 0.0f, _fIndx);
            glTexCoord3f(1.0f, 0.0f, _fIndx);   glVertex3f(1.0f, 0.0f, _fIndx);
            glTexCoord3f(1.0f, 1.0f, _fIndx);   glVertex3f(1.0f, 1.0f, _fIndx);
            glTexCoord3f(0.0f, 1.0f, _fIndx);   glVertex3f(0.0f, 1.0f, _fIndx);

            //////////////////////////////////
    //        glNormal3f(0.0f, 1.0f, 0.0f);
            glTexCoord3f(1.0f, _fIndx, 0.0f);   glVertex3f(1.0f, _fIndx, 0.0f);
            glTexCoord3f(0.0f, _fIndx, 0.0f);   glVertex3f(0.0f, _fIndx, 0.0f);
            glTexCoord3f(0.0f, _fIndx, 1.0f);   glVertex3f(0.0f, _fIndx, 1.0f);
            glTexCoord3f(1.0f, _fIndx, 1.0f);   glVertex3f(1.0f, _fIndx, 1.0f);
            //////////////////////////////////
    //        glNormal3f(1.0f, 0.0f, 0.0f);
            glTexCoord3f(_fIndx, 0.0f, 0.0f);   glVertex3f(_fIndx, 0.0f, 0.0f);
            glTexCoord3f(_fIndx, 1.0f, 0.0f);   glVertex3f(_fIndx, 1.0f, 0.0f);
            glTexCoord3f(_fIndx, 1.0f, 1.0f);   glVertex3f(_fIndx, 1.0f, 1.0f);
            glTexCoord3f(_fIndx, 0.0f, 1.0f);   glVertex3f(_fIndx, 0.0f, 1.0f);
        }
        glEnd();
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_3D);
    glEndList();
}

void VolumeGLRender::_drawRainbowTable() noexcept
{
    int _width = this->width();
    int _height = this->height();

    int _tableWidth = QFontMetrics(_TextFont).width('0') * 15;
    int _cellHeight = 10 + QFontInfo(_TextFont).pixelSize();
    int _nBlocks = _height / _cellHeight;
    float _textYoffset = 5.0f / _height;
    float _delta = (_maxPotentialValue - _minPotentialValue) / (_nBlocks-1);
    int _valuePrecision = 8;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    {
        glLoadIdentity();
        gluPerspective( 45.0f, (GLfloat)_tableWidth/(GLfloat)_height, 0.01f, 1000.0f );

        glViewport(_width - _tableWidth, 0, _tableWidth, _height);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        {
            glLoadIdentity();
            gluLookAt(0,0,1,
                      0,0,0,
                      0,1,0);

            float _x = (float)_tableWidth/(float)_height/2.0;
            for (int i=0; i <_nBlocks; ++i)
            {
                int _r, _g, _b;
                _grayscaleToRainbow((float)i / (float)(_nBlocks-1), _r, _g, _b);

                float _yBottom = (-0.5f + (float)i / (float)_nBlocks) * 0.8f;
                float _yTop = (-0.5f + (float)(i+1) / (float)_nBlocks) * 0.8f;

                glColor4ub(_r, _g, _b, _potentialFieldAlphaLevel);
                glBegin(GL_QUADS);
                glVertex2f(-_x, _yTop);
                glVertex2f( _x, _yTop);
                glVertex2f( _x, _yBottom);
                glVertex2f(-_x, _yBottom);
                glEnd();

                glColor4ub(0, 0, 0, _potentialFieldAlphaLevel);
                this->renderText(-_x*0.75, _yBottom + _textYoffset, 0.0f,
                                 QString::number(
                                     _minPotentialValue + i*_delta, 'e', _valuePrecision),
                                 _TextFont);
            }
        }
        glPopMatrix();
    }
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glViewport(0,0,_width,_height);
}

void VolumeGLRender::initializeGLEW()
{
    this->context()->makeCurrent();
    auto _rez = glewInit();
    std::cout << "GLEW: "<< glewGetErrorString(_rez) << std::endl;      /// \todo remove cout
    glGenTextures(2, _textureIDs);
    _firstDisplayListID = glGenLists(2);
    _loadFieldIntoTexture();
    _prepareTextureDisplayList();
    _loadPotentialFieldIntoTexture();
    _preparePotentialTextureDisplayList();
}

void VolumeGLRender::paintGL()
{
    glClearColor(
                _BackgroundColor.redF(),
                _BackgroundColor.greenF(),
                _BackgroundColor.blueF(),
                _BackgroundColor.alphaF());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    _drawOrigin();

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf((_mWorld * _mControl).constData());   

    glScalef(_Zoom, _Zoom, _Zoom);
    glTranslatef(-0.5f, -0.5f, -0.5f);

    glPushMatrix();
        glLoadMatrixf(_mWorld.constData());
        GLfloat light0_position[] = {0.0f, 0.0f, 3.0f, 1.0f};
        glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glPopMatrix();

    _drawBoundingBox();

    //if(!_isPressed)
    {
        if(_potentialFieldAlphaLevel!=255)
            glCallList(_firstDisplayListID);
        if(_potentialFieldAlphaLevel!=0)
            glCallList(_firstDisplayListID+1);
    }

    _drawRainbowTable();

    glColor4f(
            _TextColor.redF(),
            _TextColor.greenF(),
            _TextColor.blueF(),
            _TextColor.alphaF());
    _renderMultilineText(2, 0, _infoString, _TextFont);
}

VolumeGLRender::~VolumeGLRender()
{
    glDeleteLists(_firstDisplayListID, 2);
    glDeleteTextures(2, _textureIDs);
}

void VolumeGLRender::slot_createFormatDialog()
{
    UserInterface::VolumeGLRenderFormatDialog *_dialog =
            new UserInterface::VolumeGLRenderFormatDialog(this);
    _dialog->show();
}
