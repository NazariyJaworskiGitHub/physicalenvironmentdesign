#include "volumeglrenderrve.h"

using namespace UserInterface;

VolumeGLRenderRVE::VolumeGLRenderRVE(
        RepresentativeVolumeElement *RVE,
        QWidget *pwgt) noexcept :
    VolumeGLRenderBaseController(pwgt),
    _ptrToRVE(RVE),
    _actionEdit(new QAction("Edit...", this))
{
    connect(_actionEdit, SIGNAL(triggered()), this, SLOT(slot_createEditDialog()));
    _contextMenu->addAction(_actionEdit);
    _contextMenu->addSeparator();

    initializeGLEW();
    setWindowTitle("Volume render");    /// \todo change
}

void VolumeGLRenderRVE::_loadRVEDataIntoTexture() throw(std::runtime_error)
{
    int _size = _ptrToRVE->getSize();
    GLbyte *_RGBABuff = new GLbyte[_size * _size * _size * 4];
    if(!_RGBABuff)
        throw(std::runtime_error("FATAL: _loadFieldIntoTexture(): "
                                 "can't allocate memory for RVE"));

    for(long i = 0; i<_size * _size * _size; ++i)
    {
        float _val = _ptrToRVE->getData()[i];
        _RGBABuff[i * 4 + 0] = _val * 255;  // Grayscale
        _RGBABuff[i * 4 + 1] = _val * 255;
        _RGBABuff[i * 4 + 2] = _val * 255;
        _RGBABuff[i * 4 + 3] = ((_val >= _innerBottomCutLevel) &&
                                (_val <= _innerTopCutLevel)) ? 255 : 0;
    }

    glBindTexture(GL_TEXTURE_3D, _textureIDs[0]);
    // see https://www.opengl.org/sdk/docs/man3/xhtml/glTexImage3D.xml
    glTexImage3D(
                GL_TEXTURE_3D,      // target, copy data to device
                0,                  // level
                GL_RGBA,            // internalFormat
                _size,              // width
                _size,              // height
                _size,              // depth
                0,                  // border
                GL_RGBA,            // format
                GL_UNSIGNED_BYTE,   // type
                _RGBABuff           // ptr to data
                );
    delete[] _RGBABuff;
}

void VolumeGLRenderRVE::_prepareRVEDataTextureDisplayList() noexcept
{
    glNewList(_displayListBaseID, GL_COMPILE);
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
    for (float _fIndx = 0.0f; _fIndx <= 1.0f; _fIndx += 1.0 / _ptrToRVE->getSize())
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

void VolumeGLRenderRVE::initializeGLEW() throw(std::runtime_error)
{
    this->context()->makeCurrent();
    auto _res = glewInit();
    if(_res != GLEW_OK)
        throw(std::runtime_error(std::string("FATAL: GLEW: ") + reinterpret_cast<const char*>(
                                     glewGetErrorString(_res))));
    glGenTextures(1, _textureIDs);
    _displayListBaseID = glGenLists(1);
    _loadRVEDataIntoTexture();
    _prepareRVEDataTextureDisplayList();
}

void VolumeGLRenderRVE::paintGL()
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
        glCallList(_displayListBaseID);
    }

    glColor4f(
            _TextColor.redF(),
            _TextColor.greenF(),
            _TextColor.blueF(),
            _TextColor.alphaF());
    _renderMultilineText(2, 0, _infoString, _TextFont);
}

VolumeGLRenderRVE::~VolumeGLRenderRVE()
{
    glDeleteLists(_displayListBaseID, 1);
    glDeleteTextures(1, _textureIDs);
}

void VolumeGLRenderRVE::slot_createEditDialog()
{
    UserInterface::VolumeGLRenderRVEEditDialog *_dialog =
            new UserInterface::VolumeGLRenderRVEEditDialog(this);
    _dialog->show();
}

